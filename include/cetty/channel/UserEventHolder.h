#if !defined(CETTY_CHANNEL_CHANNELEVENTHOLDER_H)
#define CETTY_CHANNEL_CHANNELEVENTHOLDER_H

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

class UserEventHolder;
typedef boost::intrusive_ptr<UserEventHolder> UserEventHolderPtr;

class UserEventHolder : public cetty::util::ReferenceCounter<UserEventHolder> {
public:
    virtual ~UserEventHolder() {}

    /**
     * Deep-copies the DynamicAnyHolder.
     */
    virtual UserEventHolder* clone() const = 0;

    /**
     * Returns the type information of the stored content.
     */
    virtual const std::type_info& type() const = 0;

    virtual bool isVector() const = 0;
    virtual bool isString() const = 0;
    virtual bool isPointer() const = 0;
    virtual bool isIntrusivePointer() const = 0;
    //virtual bool isSharedPointer() const = 0;

    virtual int vectorSize() const = 0;
};

template <typename ValueT>
class UserEventHolderImpl : public UserEventHolder {
public:
    UserEventHolderImpl(const ValueT& value)
        : held(value) {
    }

    virtual ~UserEventHolderImpl() {}

    UserEventHolder* clone() const {
        return new UserEventHolderImpl(held);
    }

    virtual const std::type_info& type() const {
        return typeid(ValueT);
    }

    virtual bool isString() const {
        return type() == typeid(std::string);
    }
    virtual bool isVector() const { return false; }
    virtual bool isPointer() const { return false; }
    virtual bool isIntrusivePointer() const { return false; }

    virtual int vectorSize() const { return 0; }

    const ValueT& value() const { return held; }
    ValueT& value() { return held; }

private: // no copy
    UserEventHolderImpl& operator=(const UserEventHolderImpl&);

private:
    ValueT held;
};

template<>
class UserEventHolderImpl<std::string> : public UserEventHolder {
public:
    explicit UserEventHolderImpl(const char* str) : str(str) {}
    explicit UserEventHolderImpl(const std::string& str) : str(str) {}

    virtual ~UserEventHolderImpl() {}

    virtual UserEventHolder* clone() const {
        return new UserEventHolderImpl(str);
    }

    virtual const std::type_info& type() const {
        return typeid(std::string);
    }

    virtual bool isVector() const { return false; }
    virtual bool isString() const { return true; }
    virtual bool isPointer() const { return false; }
    virtual bool isIntrusivePointer() const { return false; }

    virtual int vectorSize() const { return 0; }

    std::string& value() { return str; }
    const std::string& value() const { return str; }

private: // no copy
    UserEventHolderImpl& operator=(const UserEventHolderImpl&);

private:
    std::string str;
};

template<typename T>
class UserEventHolderImpl<std::vector<T> > : public UserEventHolder {
public:
    UserEventHolderImpl(const std::vector<T>& vec) : vec(vec) {}
    virtual ~UserEventHolderImpl() {}

    virtual UserEventHolder* clone() const {
        return new UserEventHolderImpl(vec);
    }

    virtual const std::type_info& type() const {
        return typeid(std::vector<T>);
    }

    virtual bool isVector() const { return true; }
    virtual bool isString() const { return false; }
    virtual bool isPointer() const { return false; }
    virtual bool isIntrusivePointer() const { return false; }

    virtual int vectorSize() const { return (int)vec.size(); }

    const std::vector<T>& value() const { return vec; }
    std::vector<T>& value() { return vec; }

    const T& value(int index) const { return vec[index]; }
    T& value(int index) { return vec[index]; }

private: // no copy
    UserEventHolderImpl& operator=(const UserEventHolderImpl&);

private:
    std::vector<T> vec;
};

template<typename T>
class UserEventHolderImpl<boost::intrusive_ptr<T> > : public UserEventHolder {
public:
    UserEventHolderImpl(const boost::intrusive_ptr<T>& ptr) : ptr(ptr) {}
    virtual ~UserEventHolderImpl() {}

    virtual UserEventHolder* clone() const {
        return new UserEventHolderImpl(ptr);
    }

    virtual const std::type_info& type() const {
        return typeid(boost::intrusive_ptr<T>);
    }

    virtual bool isVector() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isPointer() const { return false; }
    virtual bool isIntrusivePointer() const { return true; }

    virtual int vectorSize() const { return 0; }

    const boost::intrusive_ptr<T>& value() const { return ptr; }
    boost::intrusive_ptr<T>& value() { return ptr; }

private: // no copy
    UserEventHolderImpl& operator=(const UserEventHolderImpl&);

private:
    boost::intrusive_ptr<T> ptr;
};

template<typename T>
class UserEventHolderImpl<T*> : public UserEventHolder {
public:
    UserEventHolderImpl(T* ptr) : ptr(ptr) {}
    virtual ~UserEventHolderImpl() {}

    virtual UserEventHolder* clone() const {
        return new UserEventHolderImpl(ptr);
    }

    virtual const std::type_info& type() const {
        return typeid(T*);
    }

    virtual bool isVector() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isPointer() const { return true; }
    virtual bool isIntrusivePointer() const { return false; }

    virtual int vectorSize() const { return 0; }

    const T*& value() const { return ptr; }
    T*& value() { return ptr; }

private: // no copy
    UserEventHolderImpl& operator=(const UserEventHolderImpl&);

private:
    T* ptr;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELEVENTHOLDER_H)

// Local Variables:
// mode: c++
// End:

