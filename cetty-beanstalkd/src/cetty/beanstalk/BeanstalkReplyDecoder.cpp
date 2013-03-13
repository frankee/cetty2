/*
 * BeanstalkReplyDecoder.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: chenhl
 */

#include <cetty/beanstalk/BeanstalkReplyDecoder.h>

namespace cetty {
namespace beanstalk {

void BeanstalkReplyDecoder::init() {
    checkPoint = decoder.checkPointInvoker();
    decoder.setInitialState(FIRST_LINE);
    decoder.setDecoder(boost::bind(&BeanstalkReplyDecoder::decode,
    		                       this, _1, _2, _3));
}

BeanstalkReplyPtr BeanstalkReplyDecoder::decode(
    ChannelHandlerContext& ctx,
    const ReplayingDecoderBufferPtr& buffer,
    int state) {

	if (!reply) reply = BeanstalkReplyPtr(new BeanstalkReply());

	StringPiece bytes;
    buffer->readableBytes(&bytes);

	int frameLength = indexOf(bytes, 0);
	if (frameLength < 0) {
	    LOG_DEBUG << "has not find the \"\\r\\n\", continue to reading";
	    return reset();
	}
	else if (frameLength == 0) {
	    LOG_ERROR << "parsing reply from server, but is an empty line";
	    return reset();
	}


	if (FIRST_LINE == state) {
	        if (bytes[0] == REDIS_PREFIX_STATUS_REPLY) {
	            reply_->setType(RedisReplyType::STATUS);
	            buffer->skipBytes(frameLength + 2);
	            reply_->setValue(getFrameBytes(bytes, frameLength));
	        }
	        else if (bytes[0] == REDIS_PREFIX_INTEGER_REPLY) {
	            reply_->setType(RedisReplyType::INTEGER);
	            buffer->skipBytes(frameLength + 2);
	            reply_->setValue(getFrameInt(bytes, frameLength));
	        }
	        else if (bytes[0] == REDIS_PREFIX_ERROR_REPLY) {
	            reply_->setType(RedisReplyType::ERROR);
	            buffer->skipBytes(frameLength + 2);
	            reply_->setValue(getFrameBytes(bytes, frameLength));
	        }
	        else if (bytes[0] == REDIS_PREFIX_SINGLE_BULK_REPLY) {
	            reply_->setType(RedisReplyType::STRING);
	            bulkSize_ = getFrameInt(bytes, frameLength);
	            buffer->skipBytes(frameLength + 2);

	            if (bulkSize_ < 0) {
	                reply_->setType(RedisReplyType::NIL);
	                return reset();
	            }

	            if (bulkSize_ == 0) {
	                reply_->setValue(std::string());
	                return reset();
	            }

	            if (bytes.length() >= frameLength + 4 + bulkSize_) {
	                StringPiece data;
	                buffer->readableBytes(&data);
	                buffer->skipBytes(bulkSize_ + 2);
	                reply_->setValue(StringPiece(data.data(), bulkSize_));
	            }
	            else {
	                checkPoint_(READ_BULK);
	                return RedisReplyPtr();
	            }
	        }
	        else if (bytes[0] == REDIS_PREFIX_MULTI_BULK_REPLY) {
	            reply_->setType(RedisReplyType::ARRAY);
	            bulkSize_ = 0;
	            multiBulkSize_ = getFrameInt(bytes, frameLength);
	            buffer->skipBytes(frameLength + 2);

	            if (multiBulkSize_ < 0) {
	                reply_->setType(RedisReplyType::NIL);
	                return reset();
	            }

	            if (multiBulkSize_ == 0) {
	                reply_->setValue(std::vector<StringPiece>());
	                return reset();
	            }

	            std::vector<StringPiece>* bulks = reply_->getMutableArray();
	            BOOST_ASSERT(bulks && "RedisReplyMessage getMutableArray exception");
	            return readMultiBukls(buffer, bytes, bulks);
	        }
	    }
	    else if (READ_BULK == state) {
	        if (bytes.length() >= bulkSize_ + 2) {
	            StringPiece data;
	            buffer->readableBytes(&data);
	            buffer->skipBytes(bulkSize_ + 2);
	            reply_->setValue(StringPiece(data.data(), bulkSize_));
	        }
	        else {
	            checkPoint_(READ_BULK);
	            return RedisReplyPtr();
	        }
	    }
	    else if (READ_MULTI_BULK == state) {
	        std::vector<StringPiece>* bulks = reply_->getMutableArray();
	        BOOST_ASSERT(bulks && "RedisReplyMessage getMutableArray exception");

	        if (bulkSize_ > 0) {
	            DLOG_DEBUG << "bulkSize is not empty, directly to read the bulk data";

	            if (!readMultiBulkElement(buffer, bytes, bulks)) {
	                DLOG_TRACE << "data has not read completely - bulk size, continue to reading";
	                checkPoint_(READ_MULTI_BULK);
	                return RedisReplyPtr();
	            }
	        }

	        return readMultiBukls(buffer, bytes, bulks);
	    }

	    return reset();
}

int BeanstalkReplyDecoder::indexOf(const StringPiece& bytes, int offset) {
	for (int i = offset; i < bytes.length(); ++i) {
	    if (bytes[i] == '\r' || bytes[i] == '\0') {
	        if (i+1 == bytes.length()) {
	            return -1;
	        }
	        else if (bytes[i+1] == '\n') {
	            return i;
	        }
	    }
	}

	return -1;
}

BeanstalkReplyPtr BeanstalkReplyDecoder::reset() {
	BeanstalkReplyPtr message = reply;

	reply.reset();
	dataLength = 0;

	checkPoint(FIRST_LINE);
	return message;
}

}
}
