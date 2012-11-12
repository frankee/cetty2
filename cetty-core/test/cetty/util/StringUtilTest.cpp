#include <gtest/gtest.h>
#include <cetty/util/StringUtil.h>

using namespace cetty::util;

TEST(StringUtilTest, TestIquals) {
    ASSERT_TRUE(StringUtil::iequals("Test", "test"));
    ASSERT_TRUE(StringUtil::iequals("TEST", "test"));
    ASSERT_TRUE(StringUtil::iequals("test", "test"));

    ASSERT_FALSE(StringUtil::iequals("test", "test-"));
    ASSERT_FALSE(StringUtil::iequals("test-", "test"));
}

TEST(StringUtilTest, TestIcompare) {
    ASSERT_TRUE(StringUtil::icompare("Test", "test") == 0);
    ASSERT_TRUE(StringUtil::icompare("TEST", "test") == 0);
    ASSERT_TRUE(StringUtil::icompare("test", "test") == 0);

    ASSERT_TRUE(StringUtil::icompare("test", "test-") < 0);
    ASSERT_TRUE(StringUtil::icompare("test", "TFst") < 0);

    ASSERT_TRUE(StringUtil::icompare("test-", "test") > 0);
    ASSERT_TRUE(StringUtil::icompare("tesx", "TEST") > 0);
}
