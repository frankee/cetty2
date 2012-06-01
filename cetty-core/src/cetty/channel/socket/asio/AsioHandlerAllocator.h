#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOHANDLERALLOCATOR_HPP)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOHANDLERALLOCATOR_HPP
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <boost/aligned_storage.hpp>

namespace cetty { namespace channel { namespace socket { namespace asio { 

// Class to manage the memory to be used for handler-based custom allocation.
// It contains a single block of memory which may be returned for allocation
// requests. If the memory is in use when an allocation request is made, the
// allocator delegates allocation to the global heap.

template<typename CounterType>
class AsioHandlerAllocator : private boost::noncopyable {
public:
    AsioHandlerAllocator() : used(0) {}

    void* allocate(std::size_t size) {
        // Under multi-thread environment, the worst situation will be:
        //   Thread1: ----------a--+-------d(x)-------------
        //   Thread2: ----------------a--+------x-----d-----
        //   Thread3: ----------------------a--+---x-----d--
        //   a: allocate called
        //   d: deallocate called
        //   +: ++in_use_
        //   x: --in_use_
        // Then Thread3 will not use the storage, which should,
        // however, it's still ok but a little performance lost.
        if (size < storage.size) {
            if (++used == 1) {
                return storage.address();
            }
            else {
                --used;
            }
        }

        return ::operator new(size);
    }

    void deallocate(void* pointer) {
        if (pointer == storage.address()) {
            --used;
        }
        else {
            ::operator delete(pointer);
        }
    }

private:
    // Storage space used for handler-based custom memory allocation.
    boost::aligned_storage<1024> storage;

    // Whether the handler-based custom allocation storage has been used.
    CounterType used;
};

// Wrapper class template for handler objects to allow handler memory
// allocation to be customised. Calls to operator() are forwarded to the
// encapsulated handler.
template <typename Handler, typename CounterType>
class AsioCustomAllocHandler {
public:
    AsioCustomAllocHandler(AsioHandlerAllocator<CounterType>& a, Handler h)
        : allocator(a),
          handler(h) {
    }

    void operator()() {
        handler();
    }

    template <typename Arg1>
    void operator()(Arg1 arg1) {
        handler(arg1);
    }

    template <typename Arg1, typename Arg2>
    void operator()(Arg1 arg1, Arg2 arg2) {
        handler(arg1, arg2);
    }

    friend void* asio_handler_allocate(std::size_t size,
                                       AsioCustomAllocHandler<Handler, CounterType>* h) {
        return h->allocator.allocate(size);
    }

    friend void asio_handler_deallocate(void* pointer, std::size_t /*size*/,
                                        AsioCustomAllocHandler<Handler, CounterType>* h) {
        h->allocator.deallocate(pointer);
    }

private:
    AsioHandlerAllocator<CounterType>& allocator;
    Handler handler;
};

// Helper function to wrap a handler object to add custom allocation.
template <typename Handler, typename CounterType>
inline AsioCustomAllocHandler<Handler, CounterType> makeCustomAllocHandler(
    AsioHandlerAllocator<CounterType>& a, Handler h) {
    return AsioCustomAllocHandler<Handler, CounterType>(a, h);
}

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOHANDLERALLOCATOR_HPP)

// Local Variables:
// mode: c++
// End:
