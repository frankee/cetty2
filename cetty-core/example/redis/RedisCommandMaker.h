#if !defined(CETTY_REDIS_REDISCOMMANDMAKER_H)
#define CETTY_REDIS_REDISCOMMANDMAKER_H

/**
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
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>

#include <cetty/util/SimpleString.h>
#include <cetty/util/Integer.h>

namespace cetty { namespace redis {

    using namespace cetty::buffer;
    using namespace cetty::util;

    class RedisCommandMaker {
    public:
        RedisCommandMaker(const std::string& name) : paramCnt(0) {
            buffer = ChannelBuffers::buffer(512*1024);
            buffer->setIndex(12, 12);
            append(name);
        }

        ~RedisCommandMaker() {}

    public:
        RedisCommandMaker& append(const SimpleString& param) {
            buffer->writeByte('$');
            buffer->writeBytes(Integer::toString(param.size));
            buffer->writeBytes("\r\n");
            buffer->writeBytes(Array(param.data, param.size));
            buffer->writeBytes("\r\n");

            ++paramCnt;
            return *this;
        }

        RedisCommandMaker& append(const std::string& param) {
            buffer->writeByte('$');
            buffer->writeBytes(Integer::toString(param.size()));
            buffer->writeBytes("\r\n");
            buffer->writeBytes(param);
            buffer->writeBytes("\r\n");

            ++paramCnt;
            return *this;
        }

        RedisCommandMaker& operator<< (const SimpleString& param) {
            return append(param);
        }

        RedisCommandMaker& operator<< (const std::string& param) {
            return append(param);
        }
        
        template <typename T>
        RedisCommandMaker& operator<<(T const & datum) {
            return append( boost::lexical_cast<std::string>(datum) );
        }

        RedisCommandMaker& operator<<(const std::vector<std::string> & data) {
            for (std::size_t i = 0; i < data.size(); ++i) {
                append(data[i]);
            }
            return *this;
        }

        template <typename T>
        RedisCommandMaker& operator<<(const std::vector<T> & data) {
            for (std::size_t i = 0; i < data.size(); ++i) {
                append( boost::lexical_cast<std::string>( data[i] ) );
            }
            return *this;
        }

        const ChannelBufferPtr& done() {
            int headerSize = estimateHeadSize();
            buffer->markWriterIndex();

            buffer->offsetReaderIndex(0 - headerSize);
            buffer->writerIndex(buffer->readerIndex());

            buffer->writeByte('*');
            buffer->writeBytes(Integer::toString(paramCnt));
            buffer->writeBytes("\r\n");

            buffer->resetWriterIndex();

            //Array arry;
            //buffer->readableBytes(arry);
            //printf("%s\n", std::string(arry.data(), arry.length()).c_str());
            //printf("%s\n", ChannelBuffers::hexDump(*buffer).c_str());

            return buffer;
        }

    private:
        int estimateHeadSize() {
            if (paramCnt < 10)        { return 4; }
            else if (paramCnt < 100)  { return 5; }
            else if (paramCnt < 1000) { return 6; }
            else { BOOST_ASSERT(false); }
        }

    private:
        int paramCnt;
        ChannelBufferPtr buffer;
    };

}}


#endif //#if !defined(CETTY_REDIS_REDISCOMMANDMAKER_H)