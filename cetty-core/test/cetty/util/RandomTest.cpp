#include "gtest/gtest.h"
#include "cetty/util/Random.h"

using namespace cetty::util;

TEST(RandomTest, DistinctSeeds) {
    // Strictly speaking, it is possible for these to randomly fail,
    // but the probability should be *extremely* small (< 2**-63).
    ASSERT_FALSE(Random().nextLong() == Random().nextLong());
    ASSERT_FALSE(Random().nextLong() == Random().nextLong());
}

TEST(RandomTest, NextBytes) {
    static const int EXPECTED_CNT = 11;
    char expected[EXPECTED_CNT] = {27, -115, -58, -29, -77, -39, -20, 118, -106, 75, 37};
    Random r(2398579034L);

    for (int i = 1; i <= EXPECTED_CNT; i++) {
        r.setSeed(2398579034L);
        char actual[EXPECTED_CNT];
        r.nextBytes(actual, EXPECTED_CNT);

        for (int j = 0; j < i; ++j) {
            ASSERT_EQ(expected[j], actual[j]);
        }
    }
}

TEST(RandomTest, NextIntPowerOfTwoMod) {
    Random r(69);
    int total = 0;
    for (int i = 0; i < 1000; i++) {
        total += r.nextInt(16);
    }

    ASSERT_EQ(total, 7639);
}

TEST(RandomTest, nextFloat) {
    Random r;

    for (int i = 0; i < 1000; ++i) {
        double f = r.nextFloat();
        ASSERT_TRUE(f > 0 && f < 1);
    }
}

TEST(RandomTest, nextDouble) {
    Random r;
    
    for (int i = 0; i < 1000; ++i) {
        double d = r.nextDouble();
        ASSERT_TRUE(d > 0 && d < 1);
    }
}

TEST(RandomTest, nextInt) {
    Random r;

    for (int j = 0; j < 100; ++j) {
        int range = r.nextInt(1<<30);

        for (int i = 0; i < 1000; ++i) {
            int v = r.nextInt(range);
            ASSERT_TRUE(v >= 0 && v < range);
        }
    }
}