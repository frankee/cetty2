#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOHANDLERALLOCATOR_HPP)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOHANDLERALLOCATOR_HPP
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/aligned_storage.hpp>

namespace cetty {
namespace channel {
namespace asio {

// Class to manage the memory to be used for handler-based custom allocation.
// It contains a single block of memory which may be returned for allocation
// requests. If the memory is in use when an allocation request is made, the
// allocator delegates allocation to the global heap.

template<typename CounterT>
class AsioHandlerAllocator : private boost::noncopyable {
public:
    AsioHandlerAllocator()
        : used_(0) {}

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
        if (size < storage_.size) {
            if (++used_ == 1) {
                return storage_.address();
            }
            else {
                --used_;
            }
        }

        return ::operator new(size);
    }

    void deallocate(void* pointer) {
        if (pointer == storage_.address()) {
            --used_;
        }
        else {
            ::operator delete(pointer);
        }
    }

private:
    // Storage space used for handler-based custom memory allocation.
    boost::aligned_storage<1024> storage_;

    // Whether the handler-based custom allocation storage has been used.
    CounterT used_;
};

// Wrapper class template for handler objects to allow handler memory
// allocation to be customized. Calls to operator() are forwarded to the
// encapsulated handler.
template <typename Handler, typename CounterT>
class AsioCustomAllocHandler {
public:
    AsioCustomAllocHandler(AsioHandlerAllocator<CounterT>& a, Handler h)
        : allocator_(a),
          handler_(h) {
    }

    void operator()() {
        handler_();
    }

    template <typename Arg1>
    void operator()(Arg1 arg1) {
        handler_(arg1);
    }

    template <typename Arg1, typename Arg2>
    void operator()(Arg1 arg1, Arg2 arg2) {
        handler_(arg1, arg2);
    }

    friend void* asio_handler_allocate(std::size_t size,
                                       AsioCustomAllocHandler<Handler, CounterT>* h) {
        return h->allocator_.allocate(size);
    }

    friend void asio_handler_deallocate(void* pointer,
                                        std::size_t /*size*/,
                                        AsioCustomAllocHandler<Handler, CounterT>* h) {
        h->allocator_.deallocate(pointer);
    }

private:
    AsioHandlerAllocator<CounterT>& allocator_;
    Handler handler_;
};

// Helper function to wrap a handler object to add custom allocation.
template <typename Handler, typename CounterT>
inline AsioCustomAllocHandler<Handler, CounterT> makeCustomAllocHandler(
    AsioHandlerAllocator<CounterT>& a,
    Handler h) {
    return AsioCustomAllocHandler<Handler, CounterT>(a, h);
}

}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOHANDLERALLOCATOR_HPP)

// Local Variables:
// mode: c++
// End:
