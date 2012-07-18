#if !defined(CETTY_BUFFER_CHANNELBUFFERFACTORY_H)
#define CETTY_BUFFER_CHANNELBUFFERFACTORY_H

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

#include <cetty/buffer/ByteOrder.h>
#include <cetty/buffer/ChannelBufferFwd.h>
#include <cetty/buffer/ChannelBufferFactoryFwd.h>

namespace cetty {
namespace buffer {

/**
 * A factory that creates or pools {@link ChannelBuffer}s.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class ChannelBufferFactory {
public:
    virtual ~ChannelBufferFactory() {}

    /**
     * Returns a {@link ChannelBuffer} with the specified <tt>capacity</tt>.
     * This method is identical to <tt>getBuffer(getDefaultOrder(), capacity)</tt>.
     *
     * @param capacity the capacity of the returned {@link ChannelBuffer}
     * @return a {@link ChannelBuffer} with the specified <tt>capacity</tt>,
     *         whose <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt>
     */
    virtual ChannelBufferPtr getBuffer(int capacity);

    /**
     * Returns a {@link ChannelBuffer} with the specified <tt>endianness</tt>
     * and <tt>capacity</tt>.
     *
     * @param endianness the endianness of the returned {@link ChannelBuffer}
     * @param capacity   the capacity of the returned {@link ChannelBuffer}
     * @return a {@link ChannelBuffer} with the specified <tt>endianness</tt> and
     *         <tt>capacity</tt>, whose <tt>readerIndex</tt> and <tt>writerIndex</tt>
     *         are <tt>0</tt>
     */
    virtual ChannelBufferPtr getBuffer(const ByteOrder& endianness, int capacity) = 0;

    /**
     * Returns the default endianness of the {@link ChannelBuffer} which is
     * returned by {@link #getBuffer(int)}.
     *
     * @return the default endianness of the {@link ChannelBuffer} which is
     *         returned by {@link #getBuffer(int)}
     */
    const ByteOrder& getDefaultOrder() const { return this->defaultOrder; }

protected:
    /**
     * Creates a new factory whose default {@link ByteOrder} is
     * {@link ByteOrder#BIG_ENDIAN}.
     */
    ChannelBufferFactory() : defaultOrder(ByteOrder::big_endian) {
    }

    /**
     * Creates a new factory with the specified default {@link ByteOrder}.
     *
     * @param defaultOrder the default {@link ByteOrder} of this factory
     */
    ChannelBufferFactory(const ByteOrder& defaultOrder) : defaultOrder(defaultOrder) {
    }

private:
    ByteOrder defaultOrder;
};

}
}

#endif //#if !defined(CETTY_BUFFER_CHANNELBUFFERFACTORY_H)

// Local Variables:
// mode: c++
// End:
