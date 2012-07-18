#if !defined(CETTY_CHANNEL_CHANNELMESSAGEHOLDER_H)
#define CETTY_CHANNEL_CHANNELMESSAGEHOLDER_H

/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <string>
#include <vector>
#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace buffer {
class ChannelBuffer;
}
}

namespace cetty {
namespace channel {

using namespace cetty::buffer;

class ChannelMessageHolder : public cetty::util::ReferenceCounter<ChannelMessageHolder> {
public:
    virtual ~ChannelMessageHolder() {}

    /**
     * Deep-copies the DynamicAnyHolder.
     */
    virtual ChannelMessageHolder* clone() const = 0;

    /**
     * Returns the type information of the stored content.
     */
    virtual const std::type_info& type() const = 0;

    virtual bool isVector() const = 0;
    virtual bool isString() const = 0;
    virtual bool isWideString() const = 0;
    virtual bool isRawPointer() const = 0;
    virtual bool isSmartPointer() const = 0;
    virtual bool isChannelBuffer() const = 0;

    virtual int vectorSize() const = 0;
};

typedef boost::intrusive_ptr<ChannelMessageHolder> ChannelMessageHolderPtr;

template <typename ValueT>
class ChannelMessageHolderImpl : public ChannelMessageHolder {
public:
    ChannelMessageHolderImpl(const ValueT& value)
        : held(value) {
    }

    virtual ~ChannelMessageHolderImpl() {}

    ChannelMessageHolder* clone() const {
        return new ChannelMessageHolderImpl(held);
    }

    virtual const std::type_info& type() const {
        return typeid(ValueT);
    }

    virtual bool isString() const {
        return type() == typeid(std::string);
    }
    virtual bool isWideString() const {
        return type() == typeid(std::wstring);
    }

    virtual bool isVector() const { return false; }
    virtual bool isRawPointer() const { return false; }
    virtual bool isSmartPointer() const { return false; }
    virtual bool isChannelBuffer() const { return false; }

    virtual int vectorSize() const { return 0; }

    const ValueT& value() const { return held; }
    ValueT& value() { return held; }

private: // no copy
    ChannelMessageHolderImpl& operator=(const ChannelMessageHolderImpl&);

private:
    ValueT held;
};

template<>
class ChannelMessageHolderImpl<std::string> : public ChannelMessageHolder {
public:
    explicit ChannelMessageHolderImpl(const char* str) : str(str) {}
    explicit ChannelMessageHolderImpl(const std::string& str) : str(str) {}

    virtual ~ChannelMessageHolderImpl() {}

    virtual ChannelMessageHolder* clone() const {
        return new ChannelMessageHolderImpl(str);
    }

    virtual const std::type_info& type() const {
        return typeid(std::string);
    }

    virtual bool isVector() const { return false; }
    virtual bool isString() const { return true; }
    virtual bool isWideString() const { return false; }
    virtual bool isRawPointer() const { return false; }
    virtual bool isSmartPointer() const { return false; }
    virtual bool isChannelBuffer() const { return false; }

    virtual int vectorSize() const { return 0; }

    std::string& value() { return str; }
    const std::string& value() const { return str; }

private: // no copy
    ChannelMessageHolderImpl& operator=(const ChannelMessageHolderImpl&);

private:
    std::string str;
};

template<>
class ChannelMessageHolderImpl<std::wstring> : public ChannelMessageHolder {
public:
    explicit ChannelMessageHolderImpl(const wchar_t* str) : str(str) {}
    explicit ChannelMessageHolderImpl(const std::wstring& str) : str(str) {}

    virtual ~ChannelMessageHolderImpl() {}

    virtual ChannelMessageHolder* clone() const {
        return new ChannelMessageHolderImpl(str);
    }

    virtual const std::type_info& type() const {
        return typeid(std::wstring);
    }

    virtual bool isVector() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isWideString() const { return true; }
    virtual bool isRawPointer() const { return false; }
    virtual bool isSmartPointer() const { return false; }
    virtual bool isChannelBuffer() const { return false; }

    virtual int vectorSize() const { return 0; }

    std::wstring& value() { return str; }
    const std::wstring& value() const { return str; }

private: // no copy
    ChannelMessageHolderImpl& operator=(const ChannelMessageHolderImpl&);

private:
    std::wstring str;
};

template<typename T>
class ChannelMessageHolderImpl<std::vector<T> > : public ChannelMessageHolder {
public:
    ChannelMessageHolderImpl(const std::vector<T>& vec) : vec(vec) {}
    virtual ~ChannelMessageHolderImpl() {}

    virtual ChannelMessageHolder* clone() const {
        return new ChannelMessageHolderImpl(vec);
    }

    virtual const std::type_info& type() const {
        return typeid(std::vector<T>);
    }

    virtual bool isVector() const { return true; }
    virtual bool isString() const { return false; }
    virtual bool isWideString() const { return false; }
    virtual bool isRawPointer() const { return false; }
    virtual bool isSmartPointer() const { return false; }
    virtual bool isChannelBuffer() const { return false; }

    virtual int vectorSize() const { return (int)vec.size(); }

    const std::vector<T>& value() const { return vec; }
    std::vector<T>& value() { return vec; }

    const T& value(int index) const { return vec[index]; }
    T& value(int index) { return vec[index]; }

private: // no copy
    ChannelMessageHolderImpl& operator=(const ChannelMessageHolderImpl&);

private:
    std::vector<T> vec;
};

template<typename T>
class ChannelMessageHolderImpl<boost::intrusive_ptr<T> > : public ChannelMessageHolder {
public:
    ChannelMessageHolderImpl(const boost::intrusive_ptr<T>& ptr) : ptr(ptr) {}
    virtual ~ChannelMessageHolderImpl() {}

    virtual ChannelMessageHolder* clone() const {
        return new ChannelMessageHolderImpl(ptr);
    }

    virtual const std::type_info& type() const {
        return typeid(boost::intrusive_ptr<T>);
    }

    virtual bool isVector() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isWideString() const { return false; }
    virtual bool isRawPointer() const { return false; }
    virtual bool isSmartPointer() const { return true; }
    virtual bool isChannelBuffer() const { return false; }

    virtual int vectorSize() const { return 0; }

    const boost::intrusive_ptr<T>& value() const { return ptr; }
    boost::intrusive_ptr<T>& value() { return ptr; }

private: // no copy
    ChannelMessageHolderImpl& operator=(const ChannelMessageHolderImpl&);

private:
    boost::intrusive_ptr<T> ptr;
};

template<>
class ChannelMessageHolderImpl<boost::intrusive_ptr<ChannelBuffer> > : public ChannelMessageHolder {
public:
    ChannelMessageHolderImpl(const boost::intrusive_ptr<ChannelBuffer>& ptr) : ptr(ptr) {}
    virtual ~ChannelMessageHolderImpl() {}

    virtual ChannelMessageHolder* clone() const {
        return new ChannelMessageHolderImpl(ptr);
    }

    virtual const std::type_info& type() const {
        return typeid(boost::intrusive_ptr<ChannelBuffer>);
    }

    virtual bool isVector() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isWideString() const { return false; }
    virtual bool isRawPointer() const { return false; }
    virtual bool isSmartPointer() const { return true; }
    virtual bool isChannelBuffer() const { return true; }

    virtual int vectorSize() const { return 0; }

    const boost::intrusive_ptr<ChannelBuffer>& value() const { return ptr; }
    boost::intrusive_ptr<ChannelBuffer>& value() { return ptr; }

private: // no copy
    ChannelMessageHolderImpl& operator=(const ChannelMessageHolderImpl&);

private:
    boost::intrusive_ptr<ChannelBuffer> ptr;
};

template<typename T>
class ChannelMessageHolderImpl<T*> : public ChannelMessageHolder {
public:
    ChannelMessageHolderImpl(T* ptr) : ptr(ptr) {}
    virtual ~ChannelMessageHolderImpl() {}

    virtual ChannelMessageHolder* clone() const {
        return new ChannelMessageHolderImpl(ptr);
    }

    virtual const std::type_info& type() const {
        return typeid(T*);
    }

    virtual bool isVector() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isWideString() const { return false; }
    virtual bool isRawPointer() const { return true; }
    virtual bool isSmartPointer() const { return false; }
    virtual bool isChannelBuffer() const { return false; }

    virtual int vectorSize() const { return 0; }

    const T*& value() const { return ptr; }
    T*& value() { return ptr; }

private: // no copy
    ChannelMessageHolderImpl& operator=(const ChannelMessageHolderImpl&);

private:
    T* ptr;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELMESSAGEHOLDER_H)

// Local Variables:
// mode: c++
// End:
