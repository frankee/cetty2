/*
 * BeanstalkReplyDecoder.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: chenhl
 */

#include <cetty/beanstalk/BeanstalkReplyDecoder.h>
#include <cetty/util/StringUtil.h>

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


	int id = -1, pos = -1, count = -1;
	std::string data, temp;
	if (FIRST_LINE == state) {
        std::string response;
        getResponse(bytes, &response);
        reply->setResponse(response);
        buffer->skipBytes(frameLength + 2);

        int replyCode = reply->getResponseType(response);

        switch (replyCode) {
        case BeanstalkReply::INSERTED:
        	id = -1;
            getInt(bytes, &id, 8);
            reply->setId(id);
            break;

        case BeanstalkReply::BURIED:
            id = -1;
            getInt(bytes, &id, 6);
            reply->setId(id);
            break;

        case BeanstalkReply::USING:
        	getData(bytes, &data, 5);
        	reply->setValue(data);
        	break;

        case BeanstalkReply::RESERVED:
        case BeanstalkReply::FOUND:
        	id = -1;
        	pos = -1;

        	temp.assign(bytes.c_str());
        	pos = temp.find(' ', 0);
        	getInt(bytes, &id, pos);

            pos = temp.find(' ', pos + 1);
            getInt(bytes, &dataLength, pos);

            if(bytes.length() >= frameLength + dataLength + 4) {
            	reply->setValue(bytes.substr(frameLength + 2, dataLength).as_string());
            	buffer->skipBytes(dataLength + 2);
            }
            else {
            	checkPoint(SECOND_LINE);
            	return BeanstalkReplyPtr();
            }
            break;

        case BeanstalkReply::OK:
        	getInt(bytes, &dataLength, 2);

        	if (bytes.length() >= frameLength + dataLength + 4) {
        		reply->setValue(bytes.substr(frameLength + 2, dataLength).as_string());
        		buffer->skipBytes(dataLength + 2);
        	}
        	else {
        		checkPoint(SECOND_LINE);
        		return BeanstalkReplyPtr();
        	}
        	break;

        case BeanstalkReply::WATCHING:
        case BeanstalkReply::KICKED:
        	pos = -1, count = -1;
        	temp.assign(bytes.c_str());
        	pos = temp.find(' ', 0);

        	getInt(bytes, &count, pos);
            reply->setCount(count);
        	break;

        default: break;
        }
	}
	else if (SECOND_LINE == state) {
		if (bytes.length() >= dataLength + 2) {
			buffer->skipBytes(dataLength + 2);
			reply->setValue(bytes.substr(0, dataLength).as_string());
		}
		else {
			checkPoint(SECOND_LINE);
			return BeanstalkReplyPtr();
		}
	}

	return reset();
}

void BeanstalkReplyDecoder::getResponse(StringPiece &bytes,
		                                std::string *response) {
	if (response == NULL) return;

    std::string temp(bytes.c_str());
    int pos = temp.find(' ', 0);
    if(pos != std::string::npos) {
    	response->assign(temp.c_str(), pos);
    }
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

void BeanstalkReplyDecoder::getInt(StringPiece &bytes,
		                           int *value,
		                           int offset) {
	if (offset < 0) return;
    int i = offset, j = 0;
    while(i < bytes.length() && bytes[i] == ' ') i ++;

    if (i >= bytes.length()) return;

    j = i;
    while(bytes[j] >= '0' && bytes[j] <= '9') ++j;

    if (i ==j) return;

    std::string temp(bytes.data(), j - i);
    *value = StringUtil::strto32(temp);
}

void BeanstalkReplyDecoder::getData(StringPiece &bytes,
		                            std::string *data,
		                            int offset) {
	if (data == NULL || offset < 0) return;

	int i = offset;
	while ((i < bytes.length() - 2) && bytes[i] == ' ') ++ i;

	if ((i >= bytes.length() - 2)) return;

	while (bytes[i] != '\r') data->append(1, bytes[i]), ++i;
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
