
#include <string>
#include <vector>
#include "boost/shared_ptr.hpp"

#include "gtest/gtest.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/util/Exception.h"

using namespace cetty::channel;
using namespace cetty::util;

class BaseInterface {
public:
    virtual ~BaseInterface() {}

    
    virtual std::string toString() const = 0;
};

class BaseImpl : public BaseInterface {
public:
    BaseImpl() {}
    virtual ~BaseImpl() {}

    virtual std::string toString() const { return "BaseImpl"; }
};

class BaseImpl2 : public BaseInterface {
public:
    BaseImpl2() {}
    virtual ~BaseImpl2() {}

    virtual std::string toString() const { return "BaseImpl2"; }
};

class Impl : public BaseImpl {
public:
    Impl() {}
    virtual ~Impl() {}

    virtual std::string toString() const { return "Impl"; }
};

class OtherClass {
public:
    OtherClass() {}
    ~OtherClass() {}


};

typedef boost::shared_ptr<BaseInterface> BaseInterfacePtr;
typedef boost::shared_ptr<BaseImpl>      BaseImplPtr;
typedef boost::shared_ptr<BaseImpl2>     BaseImpl2Ptr;
typedef boost::shared_ptr<Impl>          ImplPtr;

TEST(ChannelMessageTest, testSharedPointer) {
    ImplPtr implPtr(new Impl);
    BaseInterfacePtr p = boost::dynamic_pointer_cast<BaseInterface>(implPtr);
    ChannelMessage msg1(p);
    ChannelMessage msg2(implPtr);

    ASSERT_TRUE(msg1.isSharedPointer());
    ASSERT_FALSE(msg1.isRawPointer());

    ASSERT_EQ((msg1.sharedPointer<Impl, BaseInterface>()).get(), implPtr.get());
    ASSERT_EQ(msg2.sharedPointer<Impl>().get(), implPtr.get());
    ASSERT_EQ((msg1.sharedPointer<BaseImpl, BaseInterface>()).get(), implPtr.get());
    ASSERT_EQ((msg2.sharedPointer<BaseImpl, Impl>()).get(), implPtr.get());
    ASSERT_EQ(msg1.sharedPointer<BaseInterface>().get(), implPtr.get());
    ASSERT_EQ((msg2.sharedPointer<BaseInterface, Impl>()).get(), implPtr.get());

    ASSERT_TRUE((msg1.sharedPointer<BaseImpl2, BaseInterface>()).get() == 0);
    ASSERT_TRUE((msg2.sharedPointer<BaseImpl2, Impl>()).get() == 0);
    ASSERT_TRUE((msg1.sharedPointer<Impl>()).get() == 0);
    ASSERT_TRUE((msg2.sharedPointer<BaseImpl>()).get() == 0);

    ASSERT_TRUE(msg1.rawPointer<Impl>() == 0);
    ASSERT_TRUE(msg1.rawPointer<BaseImpl2>() == 0);
    ASSERT_TRUE(msg1.rawPointer<BaseInterface>() == 0);

    ASSERT_EQ(msg1.value<BaseInterfacePtr>().get(), implPtr.get());
    ASSERT_THROW(msg1.value<ImplPtr>().get(), BadCastException);
    ASSERT_THROW(msg1.value<BaseImplPtr>(), BadCastException);
    ASSERT_THROW(msg1.value<BaseImpl2Ptr>(), BadCastException);

    ASSERT_EQ(msg2.value<ImplPtr>().get(), implPtr.get());
    ASSERT_THROW(msg2.value<BaseImplPtr>(), BadCastException);
    ASSERT_THROW(msg2.value<BaseImpl2Ptr>(), BadCastException);
    ASSERT_THROW(msg2.value<BaseInterfacePtr>().get(), BadCastException);

    ASSERT_EQ(msg1.pointer<BaseInterfacePtr>()->get(), implPtr.get());
    ASSERT_TRUE(msg1.pointer<ImplPtr>() == NULL);
    ASSERT_TRUE(msg1.pointer<BaseImplPtr>() == NULL);

    ASSERT_EQ(msg2.pointer<ImplPtr>()->get(), implPtr.get());
    ASSERT_TRUE(msg2.pointer<BaseImplPtr>() == NULL);
    ASSERT_TRUE(msg2.pointer<BaseInterfacePtr>() == NULL);
}

TEST(ChannelMessageTest, testRawPointer) {
    Impl* impl = new Impl;
    BaseInterface* base = impl;
    ChannelMessage msg1(impl);
    ChannelMessage msg2(base);

    ASSERT_TRUE(msg1.isRawPointer());
    ASSERT_TRUE(msg2.isRawPointer());

    ASSERT_EQ(msg2.rawPointer<BaseInterface>(), impl);
    ASSERT_EQ((msg1.rawPointer<Impl>()), impl);
    ASSERT_FALSE(msg1.sharedPointer<Impl>());
    ASSERT_FALSE(msg1.sharedPointer<BaseInterface>());
}

TEST(ChannelMessageTest, testVector1) {
    std::vector<BaseInterfacePtr> ptrs;
    Impl* impl1 = new Impl;
    Impl* impl2 = new Impl;
    BaseImpl* baseImpl = new BaseImpl;

    ptrs.push_back(BaseInterfacePtr(impl1));
    ptrs.push_back(BaseInterfacePtr(impl2));
    ptrs.push_back(BaseInterfacePtr(baseImpl));

    ChannelMessage msg1(ptrs);

    ASSERT_EQ(msg1.sharedPointer<BaseInterface>(0).get(), impl1);
    ASSERT_EQ(msg1.sharedPointer<BaseInterface>(1).get(), impl2);
    ASSERT_EQ(msg1.sharedPointer<BaseInterface>(2).get(), baseImpl);
    ASSERT_FALSE(msg1.sharedPointer<BaseInterface>(3));
    ASSERT_THROW(msg1.value<BaseInterfacePtr>(3), RangeException);
}

TEST(ChannelMessageTest, testVector2) {
    std::vector<ChannelMessage> msgs;
    Impl* impl1 = new Impl;
    Impl* impl2 = new Impl;
    BaseImpl* baseImpl = new BaseImpl;

    BaseInterfacePtr impl1Ptr = BaseInterfacePtr(impl1);
    BaseInterfacePtr impl2Ptr = BaseInterfacePtr(impl2);
    BaseInterfacePtr baseImplPtr = BaseInterfacePtr(baseImpl);

    msgs.push_back(ChannelMessage(impl1Ptr));
    msgs.push_back(ChannelMessage(impl2Ptr));
    msgs.push_back(ChannelMessage(baseImplPtr));

    ChannelMessage msg1(msgs);
    ChannelMessage msg2(ChannelMessage(impl1Ptr),
                        ChannelMessage(impl2Ptr),
                        ChannelMessage(baseImplPtr));

    ASSERT_EQ(msg1.value<ChannelMessage>(0).sharedPointer<BaseInterface>().get(), impl1);
    ASSERT_EQ(msg1.value<ChannelMessage>(1).sharedPointer<BaseInterface>().get(), impl2);
    ASSERT_EQ(msg1.value<ChannelMessage>(2).sharedPointer<BaseInterface>().get(), baseImpl);
    ASSERT_TRUE(msg1.value<ChannelMessage>(3).empty());
    ASSERT_THROW(msg1.value<std::vector<ChannelMessage> >(3), RangeException);
}

TEST(ChannelMessageTest, testString) {
    
}

TEST(ChannelMessageTest, testBasicAny) {

}
