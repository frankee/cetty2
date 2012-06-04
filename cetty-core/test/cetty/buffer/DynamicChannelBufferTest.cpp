#include "cetty/buffer/DynamicChannelBufferTest.h"

using namespace cetty::buffer;

TEST_F(DynamicChannelBufferTest, shouldNotFailOnInitialIndexUpdate) {
    shouldNotFailOnInitialIndexUpdate();
}

TEST_F(DynamicChannelBufferTest, shouldNotFailOnInitialIndexUpdate2) {
    shouldNotFailOnInitialIndexUpdate2();
}

TEST_F(DynamicChannelBufferTest, shouldNotFailOnInitialIndexUpdate3) {
    shouldNotFailOnInitialIndexUpdate3();
}

#define CHANNEL_BUFFER_IMPL_TEST DynamicChannelBufferTest
#include "cetty/buffer/AbstractChannelBufferTest.inc.h"

