#if !defined(CETTY_BUFFER_HEAPCHANNELBUFFERFACTORY_H)
#define CETTY_BUFFER_HEAPCHANNELBUFFERFACTORY_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/buffer/ChannelBufferFactory.h>

namespace cetty {
namespace buffer {

/**
 * A {@link ChannelBufferFactory} which merely allocates a heap buffer with
 * the specified capacity.  {@link HeapChannelBufferFactory} should perform
 * very well in most situations because it relies on the JVM garbage collector,
 * which is highly optimized for heap allocation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class HeapChannelBufferFactory : public ChannelBufferFactory {
public:
    static ChannelBufferFactory& getInstance();
    static ChannelBufferFactory& getInstance(const ByteOrder& endianness);

public:
    /**
     * Creates a new factory whose default {@link ByteOrder} is
     * {@link ByteOrder#BIG_ENDIAN}.
     */
    HeapChannelBufferFactory() : ChannelBufferFactory() {}

    /**
     * Creates a new factory with the specified default {@link ByteOrder}.
     *
     * @param defaultOrder the default {@link ByteOrder} of this factory
     */
    HeapChannelBufferFactory(const ByteOrder& defaultOrder)
        : ChannelBufferFactory(defaultOrder) {}

    virtual ~HeapChannelBufferFactory() {}

    ChannelBufferPtr getBuffer(const ByteOrder& order, int capacity);

private:
    static ChannelBufferFactoryPtr INSTANCE_BE;
    static ChannelBufferFactoryPtr INSTANCE_LE;
};

}
}

#endif //#if !defined(CETTY_BUFFER_HEAPCHANNELBUFFERFACTORY_H)

// Local Variables:
// mode: c++
// End:
