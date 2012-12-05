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
#include <cetty/handler/codec/http/HttpPackageDecoder.h>

#include <cetty/buffer/Unpooled.h>
#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/StringPiece.h>

#include <cetty/handler/codec/http/HttpChunk.h>
#include <cetty/handler/codec/http/HttpVersion.h>
#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpCodecUtil.h>
#include <cetty/handler/codec/http/HttpChunkTrailer.h>
#include <cetty/handler/codec/http/HttpResponseStatus.h>

#include <cetty/handler/codec/TooLongFrameException.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;
using namespace cetty::handler::codec;

const int HttpPackageDecoder::MAX_INITIAL_LINE_LENGTH = 4096;
const int HttpPackageDecoder::MAX_HEADER_SIZE = 8192;
const int HttpPackageDecoder::MAX_CHUNK_SIZE  = 8192;

HttpPackageDecoder::HttpPackageDecoder(DecodingType decodingType)
    : isDecodingRequest_(decodingType == REQUEST),
maxInitialLineLength_(MAX_INITIAL_LINE_LENGTH),
      maxHeaderSize_(MAX_HEADER_SIZE),
      maxChunkSize_(MAX_CHUNK_SIZE),
      chunkSize_(0),
      headerSize_(0) {
}

HttpPackageDecoder::HttpPackageDecoder(DecodingType decodingType,
    int maxInitialLineLength,
                                       int maxHeaderSize,
                                       int maxChunkSize)
    : isDecodingRequest_(decodingType == REQUEST),
        maxInitialLineLength_(maxInitialLineLength),
      maxHeaderSize_(maxHeaderSize),
      maxChunkSize_(maxChunkSize),
      chunkSize_(0),
      headerSize_(0) {
    if (maxInitialLineLength <= 0) {
        maxInitialLineLength_ = MAX_INITIAL_LINE_LENGTH;
    }

    if (maxHeaderSize <= 0) {
        maxHeaderSize_ = MAX_HEADER_SIZE;
    }

    if (maxChunkSize < 0) {
        maxChunkSize_ = MAX_CHUNK_SIZE;
    }
}

HttpPackage HttpPackageDecoder::decode(ChannelHandlerContext& ctx,
                                       const ReplayingDecoderBufferPtr& buffer,
                                       int state) {
                                           const CheckPointInvoker& checkPoint = checkPointInvoker_;
                                           HttpHeaders& headers = message_.headers();
    switch (state) {
    case SKIP_CONTROL_CHARS: {
        if (skipControlCharacters(buffer)) {
            checkPoint(READ_INITIAL);
        }
        else { 
            checkPoint(SKIP_CONTROL_CHARS); //do not care skipped control chars.
        }

        break;
    }

    case READ_INITIAL: {
        std::vector<StringPiece> initialLine;
        StringPiece line = readLine(buffer, maxInitialLineLength_);

        if (!splitInitialLine(line, &initialLine)) {
            // need more bytes.
            break;
        }

        if (initialLine.size() < 3) {
            // Invalid initial line - ignore.
            checkPoint(SKIP_CONTROL_CHARS);
            break;
        }

        message_ = httpPackageCreator_(initialLine[0],
                                initialLine[1],
                                initialLine[2]);

        checkPoint(READ_HEADER);

        // clear data, then step into the READ_HEADER state.
        headerSize_ = 0;
        headers.clear();
        break;
    }

    case READ_HEADER: {
        int nextState = readHeaders(buffer);

        if (nextState == READ_HEADER) {
            // need read more bytes to parse headers.
            break;
        }

        checkPoint(nextState);

        if (nextState == READ_CHUNK_SIZE) {
            // Chunked encoding - generate HttpMessage first.  HttpChunks will follow.
            return message_;
        }
        else if (nextState == SKIP_CONTROL_CHARS) {
            // No content is expected.
            return message_;
        }
        else {
            int contentLength = headers.contentLength(-1);

            if (contentLength == 0 || (contentLength == -1 && isDecodingRequest_)) {
                content_ = Unpooled::EMPTY_BUFFER;
                return reset();
            }

            switch (nextState) {
            case READ_FIXED_LENGTH_CONTENT:
                if (contentLength > maxChunkSize_
                        || headers.is100ContinueExpected()) {
                    // Generate HttpMessage first.  HttpChunks will follow.
                    checkPoint(READ_FIXED_LENGTH_CONTENT_AS_CHUNKS);
                    headers.setTransferEncoding(HttpTransferEncoding::STREAMED);

                    // chunkSize will be decreased as the READ_FIXED_LENGTH_CONTENT_AS_CHUNKS
                    // state reads data chunk by chunk.
                    chunkSize_ = headers.contentLength(-1);
                    return message_;
                }

                break;

            case READ_VARIABLE_LENGTH_CONTENT:
                if (buffer->readableBytes() > maxChunkSize_
                        || headers.is100ContinueExpected()) {
                    // Generate HttpMessage first.  HttpChunks will follow.
                    checkPoint(READ_VARIABLE_LENGTH_CONTENT_AS_CHUNKS);
                    headers.setTransferEncoding(HttpTransferEncoding::STREAMED);
                    return message_;
                }

                break;

            default:
                throw IllegalStateException(
                    StringUtil::printf("Unexpected state: %d", nextState));
            }
        }

        // We return null here, this forces decode to be called again where we will decode the content
        return HttpPackage();
    }

    case READ_VARIABLE_LENGTH_CONTENT: {
        int chunkSize = std::min(maxChunkSize_, buffer->readableBytes());
        ChannelBufferPtr buff = buffer->readSlice(chunkSize);

        if (buffer->needMoreBytes()) {
            return HttpPackage();
        }

        HttpChunkPtr chunk = new HttpChunk(buff);

        if (headers.transferEncoding() != HttpTransferEncoding::STREAMED) {
            headers.setTransferEncoding(HttpTransferEncoding::STREAMED);

            //return new Object[] {message, new DefaultHttpChunk(buffer.readBytes(toRead))};
        }
        else {
            return chunk;
        }
    }

    case READ_VARIABLE_LENGTH_CONTENT_AS_CHUNKS: {
        // Keep reading data as a chunk until the end of connection is reached.
        int chunkSize = std::min(maxChunkSize_, buffer->readableBytes());
        ChannelBufferPtr buff = buffer->readSlice(chunkSize);

        if (buffer->needMoreBytes()) {
            return HttpPackage();
        }

        HttpChunkPtr chunk = new HttpChunk(buff);

        if (!buffer->readable()) {
            // Reached to the end of the connection.
            reset();

            if (!chunk->isLast()) {
                // Append the last chunk.
                chunk->setFollowLastChunk(true);
                return chunk;
            }
        }

        return chunk;
    }

    case READ_FIXED_LENGTH_CONTENT: {
        //we have a content-length so we just read the correct number of bytes
        return readFixedLengthContent(buffer);
    }

    case READ_FIXED_LENGTH_CONTENT_AS_CHUNKS: {
        BOOST_ASSERT(chunkSize_ <= MAX_INT32);

        int chunkSize = this->chunkSize_;
        int readLimit = buffer->readableBytes();

        // Check if the buffer is readable first as we use the readable byte count
        // to create the HttpChunk. This is needed as otherwise we may end up with
        // create a HttpChunk instance that contains an empty buffer and so is
        // handled like it is the last HttpChunk.
        if (readLimit == 0) {
            return HttpPackage();
        }

        int toRead = chunkSize;

        if (toRead > maxChunkSize_) {
            toRead = maxChunkSize_;
        }

        if (toRead > readLimit) {
            toRead = readLimit;
        }


        ChannelBufferPtr buff = buffer->readSlice(toRead);

        if (buffer->needMoreBytes()) {
            return HttpPackage();
        }

        HttpChunkPtr chunk = HttpChunkPtr(new HttpChunk(buff));

        if (chunkSize > toRead) {
            chunkSize -= toRead;
        }
        else {
            chunkSize = 0;
        }

        this->chunkSize_ = chunkSize;

        if (chunkSize == 0) {
            // Read all content.
            reset();

            if (!chunk->isLast()) {
                // Append the last chunk.
                chunk->setFollowLastChunk(true);
                return chunk;
            }
        }

        return chunk;
    }

    /**
     * everything else after this point takes care of reading chunked content. basically, read chunk size,
     * read chunk, read and ignore the CRLF and repeat until 0
     */
    case READ_CHUNK_SIZE: {
        StringPiece line = readLine(buffer, maxInitialLineLength_);

        if (buffer->needMoreBytes()) {
            return HttpPackage();
        }

        chunkSize_ = getChunkSize(line);

        if (chunkSize_ == 0) {
            checkPoint(READ_CHUNK_FOOTER);
            return HttpPackage();
        }
        else if (chunkSize_ > maxChunkSize_) {
            // A chunk is too large. Split them into multiple chunks again.
            checkPoint(READ_CHUNKED_CONTENT_AS_CHUNKS);
        }
        else {
            checkPoint(READ_CHUNKED_CONTENT);
        }

        break;
    }

    case READ_CHUNKED_CONTENT: {
        BOOST_ASSERT(chunkSize_ <= MAX_INT32);
        ChannelBufferPtr buff = buffer->readSlice(chunkSize_);

        if (buffer->needMoreBytes()) {
            return HttpPackage();
        }

        HttpChunkPtr chunk = HttpChunkPtr(new HttpChunk(buff));
        checkPoint(READ_CHUNK_DELIMITER);
        return chunk;
    }

    case READ_CHUNKED_CONTENT_AS_CHUNKS: {
        BOOST_ASSERT(chunkSize_ <= MAX_INT32);

        int chunkSize = this->chunkSize_;
        int readLimit = buffer->readableBytes();

        // Check if the buffer is readable first as we use the readable byte count
        // to create the HttpChunk. This is needed as otherwise we may end up with
        // create a HttpChunk instance that contains an empty buffer and so is
        // handled like it is the last HttpChunk.
        if (readLimit == 0) {
            return HttpPackage();
        }

        int toRead = chunkSize;

        if (toRead > maxChunkSize_) {
            toRead = maxChunkSize_;
        }

        if (toRead > readLimit) {
            toRead = readLimit;
        }

        ChannelBufferPtr buff = buffer->readSlice(toRead);

        if (buffer->needMoreBytes()) {
            return HttpPackage();
        }

        HttpChunkPtr chunk = HttpChunkPtr(new HttpChunk(buff));

        if (chunkSize > toRead) {
            chunkSize -= toRead;
        }
        else {
            chunkSize = 0;
        }

        this->chunkSize_ = chunkSize;

        if (chunkSize == 0) {
            // Read all content.
            checkPoint(READ_CHUNK_DELIMITER);
        }

        if (!chunk->isLast()) {
            return chunk;
        }

        break;
    }

    case READ_CHUNK_DELIMITER: {
        for (;;) {
            int8_t next = buffer->readByte();

            if (buffer->needMoreBytes()) {
                return HttpPackage();
            }

            if (next == HttpCodecUtil::CR) {
                if (buffer->readByte() == HttpCodecUtil::LF) {
                    checkPoint(READ_CHUNK_SIZE);
                    return HttpPackage();
                }

                if (buffer->needMoreBytes()) {
                    return HttpPackage();
                }
            }
            else if (next == HttpCodecUtil::LF) {
                checkPoint(READ_CHUNK_SIZE);
                return HttpPackage();
            }
        }

        break;
    }

    case READ_CHUNK_FOOTER: {
        HttpChunkTrailerPtr trailer = readTrailingHeaders(buffer);

        if (maxChunkSize_ == 0) {
            // Chunked encoding disabled.
            return reset();
        }
        else {
            reset();
            // The last chunk, which is empty
            return trailer;
        }
    }

    default: {
        throw RuntimeException("Shouldn't reach here.");
    }
    }

    return HttpPackage();
}

bool HttpPackageDecoder::isContentAlwaysEmpty(const HttpPackage& msg) const {
    HttpResponsePtr response = msg.httpResponse();

    if (response) {
        int code = response->status().getCode();

        // Correctly handle return codes of 1xx.
        //
        // See:
        //     - http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html Section 4.4
        if (code >= 100 && code < 200) {
            if (code == 101
                    && !response->headers().containsHeader(
                        HttpHeaders::Names::SEC_WEBSOCKET_ACCEPT)) {
                // It's Hixie 76 websocket handshake response
                return false;
            }

            return true;
        }

        if (code < 200) {
            return true;
        }

        switch (code) {
        case 204: case 205: case 304:
            return true;
        }
    }

    return false;
}

HttpPackage HttpPackageDecoder::reset() {
    ChannelBufferPtr content = content_;

    if (isDecodingRequest_) {
        HttpRequestPtr request = message_.httpRequest();
        if (content) {
            request->setContent(content);
            content_.reset();
        }
        
        message_ = HttpPackage();

        checkPointInvoker_(SKIP_CONTROL_CHARS);
        return request;
    }

}

bool HttpPackageDecoder::skipControlCharacters(const ReplayingDecoderBufferPtr& buffer) const {
    for (;;) {
        uint8_t c = buffer->readUnsignedByte();

        if (buffer->needMoreBytes()) {
            return false;
        }

        if (!StringUtil::isISOControl(c) && !StringUtil::isWhitespace(c)) {
            buffer->offsetReaderIndex(-1);
            return true;
        }
    }
}

HttpPackage HttpPackageDecoder::readFixedLengthContent(const ReplayingDecoderBufferPtr& buffer) {
    //int length = HttpHeaders::contentLength(*message_, -1);
    HttpHeaders& headers = message_.headers();
    int length = headers.contentLength(-1);
    BOOST_ASSERT(length <= MAX_INT32);
    int readLimit = buffer->readableBytes();

    int toRead = (int) length - contentRead_;

    if (toRead > readLimit) {
        toRead = readLimit;
    }

    contentRead_ += toRead;
    //content.reset();
    //content = buffer->readSlice(toRead);
    BOOST_ASSERT(false && "NOT implement in HttpMessageDecoder readFixedLengthContent.");

    if (length < contentRead_) {
        if (headers.transferEncoding() != HttpTransferEncoding::STREAMED) {
            headers.setTransferEncoding(HttpTransferEncoding::STREAMED);
            //return new Object[] {message, new DefaultHttpChunk(read(buffer, toRead))};
        }
        else {
            //return new DefaultHttpChunk(read(buffer, toRead));
        }
    }

    if (!content_) {
        content_ = buffer->readSlice(length);
        //content = read(buffer, (int) length);
    }
    else {
        //content.writeBytes(buffer.readBytes((int) length));
    }

    return reset();
}

/**
 * Try to do an optimized "read" of len from the given {@link ByteBuf}.
 *
 * This is part of #412 to safe byte copies
 *
 */
#if 0
ChannelBufferPtr read(const ChannelPtr& buffer, int len) {
    ByteBuf internal = internalBuffer();

    if (internal.readableBytes() >= len) {
        int index = internal.readerIndex();
        ByteBuf buf = internal.slice(index, len);

        // update the readerindex so an the next read its on the correct position
        buffer.readerIndex(index + len);
        return buf;
    }
    else {
        return buffer.readBytes(len);
    }
}
#endif

int HttpPackageDecoder::readHeaders(const ReplayingDecoderBufferPtr& buffer) {
    int nextState;
    StringPiece line = readHeader(buffer);
    StringPiece name;
    StringPiece value;

    headerSize_ = 0;

    HttpHeaders& headers = message_.headers();

    if (buffer->needMoreBytes()) { // need more data
        return READ_HEADER;
    }

    if (!line.empty()) {
        headers.clear();

        std::vector<StringPiece> header;
        std::string valueStr;
        std::string nameStr;

        do {
            char firstChar = line[0];

            if (!name.empty() && (firstChar == ' ' || firstChar == '\t')) {
                valueStr.clear();
                StringPiece trimedLine = line.trim();

                valueStr.append(value.data(), value.size());
                valueStr.append(1, ' ');
                valueStr.append(trimedLine.data(), trimedLine.size());
            }
            else {
                if (!name.empty()) {
                    nameStr.assign(name.data(), name.size());

                    if (!valueStr.empty()) {
                        headers.addHeader(nameStr, valueStr);
                    }
                    else {
                        valueStr.assign(value.data(), value.size());
                        headers.addHeader(nameStr, valueStr);
                    }

                    nameStr.clear();
                    valueStr.clear();
                }

                header.clear();
                splitHeader(line, &header);
                name = header[0];
                value = header[1];
            }

            line = readHeader(buffer);

            if (buffer->needMoreBytes()) { // need more data
                return READ_HEADER;
            }
        }
        while (!line.empty());

        // Add the last header.
        if (!name.empty()) {
            nameStr.assign(name.data(), name.size());

            if (!valueStr.empty()) {
                headers.addHeader(nameStr, valueStr);
            }
            else {
                valueStr.assign(value.data(), value.size());
                headers.addHeader(nameStr, valueStr);
            }
        }

    }

//     if (isContentAlwaysEmpty(message_)) {
//         headers.setTransferEncoding(HttpTransferEncoding::SINGLE);
//         nextState = SKIP_CONTROL_CHARS;
//     }
//     else if (HttpCodecUtil::isTransferEncodingChunked(*message_)) {
//         headers.setTransferEncoding(HttpTransferEncoding::CHUNKED);
//         nextState = READ_CHUNK_SIZE;
//     }
//     else if (HttpHeaders::contentLength(*message_, -1) >= 0) {
//         nextState = READ_FIXED_LENGTH_CONTENT;
//     }
//     else {
//         nextState = READ_VARIABLE_LENGTH_CONTENT;
//     }

    return nextState;
}

HttpChunkTrailerPtr
HttpPackageDecoder::readTrailingHeaders(const ReplayingDecoderBufferPtr& buffer) {
    headerSize_ = 0;
    StringPiece line = readHeader(buffer);
    StringPiece lastHeader;

    if (buffer->needMoreBytes()) { // need more data
        return HttpChunkTrailerPtr();
    }

    if (!line.empty()) {
        std::vector<StringPiece> header;
        HttpChunkTrailerPtr trailer = new HttpChunkTrailer;

        std::string valueStr;
        std::string nameStr;

        do {
            char firstChar = line[0];

            //FIXME
            //  HeaderName:HeaderValue
            //   Header Continued Value
            if (!lastHeader.empty() && (firstChar == ' ' || firstChar == '\t')) {
#if 0
                const std::string& lastHeader = trailer->getLastHeader(
                    lastHeader.c_str());

                if (!lastHeader.empty()) {
                    std::string newString(lastHeader);
                    newString += line.trim().c_str();
                }
                else {
                    // Content-Length, Transfer-Encoding, or Trailer
                }
#endif
            }
            else {
                header.clear();
                splitHeader(line, &header);
                StringPiece name = header[0];

                if (!name.iequals(HttpHeaders::Names::CONTENT_LENGTH) &&
                        !name.iequals(HttpHeaders::Names::TRANSFER_ENCODING) &&
                        !name.iequals(HttpHeaders::Names::TRAILER)) {
                    trailer->headers().addHeader(name.c_str(), header[1].c_str());
                }

                lastHeader = name;
            }

            line = readHeader(buffer);

            if (buffer->needMoreBytes()) { // need more data
                return HttpChunkTrailerPtr();
            }
        }
        while (!line.empty());

        return trailer;
    }

    return boost::dynamic_pointer_cast<HttpChunkTrailer>(HttpChunk::LAST_CHUNK);
}

StringPiece HttpPackageDecoder::readHeader(const ReplayingDecoderBufferPtr& buffer) {
    StringPiece str;

    int bytesCnt;
    const char* bytes = buffer->readableBytes(&bytesCnt);

    int headerSize = this->headerSize_;
    for (int i = 0, j = bytesCnt - 1; i < j; ++i, ++headerSize) {
        // Abort decoding if the header part is too large.
        if (headerSize >= maxHeaderSize_) {
            // TODO: Respond with Bad Request and discard the traffic
            //    or close the connection.
            //       No need to notify the upstream handlers - just log.
            //       If decoding a response, just throw an exception.
            throw TooLongFrameException(
                StringUtil::printf("HTTP header is larger than %d bytes.", maxHeaderSize_));
        }

        if (bytes[i] == HttpCodecUtil::CR && bytes[i+1] == HttpCodecUtil::LF) {
            buffer->offsetReaderIndex(i+2);
            return StringPiece(bytes, i);
        }
        else if (bytes[i] == HttpCodecUtil::LF) {
            buffer->offsetReaderIndex(i + 1);
            return StringPiece(bytes, i);
        }
        else if (bytes[i] != HttpCodecUtil::CR && bytes[i+1] == HttpCodecUtil::LF) {
            buffer->offsetReaderIndex(i + 2);
            return StringPiece(bytes, i + 1);
        }
    }

    buffer->needMoreBytes(true);
    return StringPiece();
}

int HttpPackageDecoder::getChunkSize(const StringPiece& hex) const {
    StringPiece h = hex.trim();

    for (int i = 0; i < h.length(); ++i) {
        char c = h.at(i);

        if (c == ';' || StringUtil::isWhitespace(c) || StringUtil::isISOControl(c)) {
            h = h.substr(0, i);
            break;
        }
    }

    return StringUtil::strto32(h.c_str(), 16);
}

StringPiece HttpPackageDecoder::readLine(const ReplayingDecoderBufferPtr& buffer,
        int maxLineLength) {
    int bytesCnt;
    const char* bytes = buffer->readableBytes(&bytesCnt);

    if (bytesCnt >= maxLineLength) {
        // TODO: Respond with Bad Request and discard the traffic
        //    or close the connection.
        //       No need to notify the upstream handlers - just log.
        //       If decoding a response, just throw an exception.
        throw TooLongFrameException(
            StringUtil::printf("An HTTP line is larger than %d bytes.", maxLineLength));
    }

    for (int i = 0, j = bytesCnt - 1; i < j; ++i) {
        if (bytes[i] == HttpCodecUtil::CR && bytes[i+1] == HttpCodecUtil::LF) {
            buffer->offsetReaderIndex(i+2);
            return StringPiece(bytes, i);
        }
    }

    buffer->needMoreBytes(true);
    return StringPiece();
}

bool HttpPackageDecoder::splitInitialLine(StringPiece& sb, std::vector<StringPiece>* lines) {
    int aStart;
    int aEnd;
    int bStart;
    int bEnd;
    int cStart;
    int cEnd;

    if (sb.empty() || NULL == lines) {
        return false;
    }

    aStart = findNonWhitespace(sb, 0);
    aEnd = findWhitespace(sb, aStart);

    bStart = findNonWhitespace(sb, aEnd);
    bEnd = findWhitespace(sb, bStart);

    cStart = findNonWhitespace(sb, bEnd);
    cEnd = findEndOfString(sb);

    lines->push_back(sb.substr(aStart, aEnd - aStart));
    lines->push_back(sb.substr(bStart, bEnd - bStart));
    lines->push_back(sb.substr(cStart, cEnd - cStart));

    return true;
}

void HttpPackageDecoder::splitHeader(StringPiece& sb, std::vector<StringPiece>* header) {
    int length = sb.length();
    int nameStart;
    int nameEnd;
    int colonEnd;
    int valueStart;
    int valueEnd;

    if (NULL == header) {
        return;
    }

    nameStart = findNonWhitespace(sb, 0);

    for (nameEnd = nameStart; nameEnd < length; ++nameEnd) {
        char ch = sb.at(nameEnd);

        if (ch == ':' || StringUtil::isWhitespace(ch)) {
            break;
        }
    }

    for (colonEnd = nameEnd; colonEnd < length; ++colonEnd) {
        if (sb.at(colonEnd) == ':') {
            ++colonEnd;
            break;
        }
    }

    valueStart = findNonWhitespace(sb, colonEnd);

    if (valueStart == length) {
        header->push_back(sb.substr(nameStart, nameEnd - nameStart));
        header->push_back(StringPiece());
        return;
    }

    valueEnd = findEndOfString(sb);

    header->push_back(sb.substr(nameStart, nameEnd - nameStart));
    header->push_back(sb.substr(valueStart, valueEnd - valueStart));
}

int HttpPackageDecoder::findNonWhitespace(const StringPiece& sb, int offset) {
    int result;

    for (result = offset; result < sb.length(); ++result) {
        if (!StringUtil::isWhitespace(sb.at(result))) {
            break;
        }
    }

    return result;
}

int HttpPackageDecoder::findWhitespace(const StringPiece& sb, int offset) {
    int result;

    for (result = offset; result < sb.length(); ++result) {
        if (StringUtil::isWhitespace(sb.at(result))) {
            break;
        }
    }

    return result;
}

int HttpPackageDecoder::findEndOfString(const StringPiece& sb) {
    int result;

    for (result = sb.length(); result > 0; --result) {
        if (!StringUtil::isWhitespace(sb.at(result - 1))) {
            break;
        }
    }

    return result;
}

}
}
}
}
