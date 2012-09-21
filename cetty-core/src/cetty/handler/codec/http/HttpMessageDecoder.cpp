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
#include <cetty/handler/codec/http/HttpMessageDecoder.h>

#include <cetty/buffer/Unpooled.h>
#include <cetty/util/Integer.h>
#include <cetty/util/Character.h>
#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/StringPiece.h>

#include <cetty/handler/codec/http/HttpVersion.h>
#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/handler/codec/http/HttpMessage.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpResponseStatus.h>
#include <cetty/handler/codec/http/HttpCodecUtil.h>
#include <cetty/handler/codec/http/HttpChunk.h>
#include <cetty/handler/codec/http/HttpChunkTrailer.h>
#include <cetty/handler/codec/TooLongFrameException.h>


namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;
using namespace cetty::handler::codec;

const int HttpMessageDecoder::MAX_INITIAL_LINE_LENGTH = 4096;
const int HttpMessageDecoder::MAX_HEADER_SIZE = 8192;
const int HttpMessageDecoder::MAX_CHUNK_SIZE  = 8192;

HttpMessageDecoder::HttpMessageDecoder()
    : ReplayingDecoder<HttpPackage>(SKIP_CONTROL_CHARS, true),
      maxInitialLineLength(MAX_INITIAL_LINE_LENGTH),
      maxHeaderSize(MAX_HEADER_SIZE),
      maxChunkSize(MAX_CHUNK_SIZE),
      chunkSize(0),
      headerSize(0) {
}

HttpMessageDecoder::HttpMessageDecoder(int maxInitialLineLength,
                                       int maxHeaderSize,
                                       int maxChunkSize)
    : ReplayingDecoder<HttpPackage>(SKIP_CONTROL_CHARS, true),
      maxInitialLineLength(maxInitialLineLength),
      maxHeaderSize(maxHeaderSize),
      maxChunkSize(maxChunkSize),
      chunkSize(0),
      headerSize(0) {
    if (maxInitialLineLength <= 0) {
        this->maxInitialLineLength = MAX_INITIAL_LINE_LENGTH;
    }

    if (maxHeaderSize <= 0) {
        this->maxHeaderSize = MAX_HEADER_SIZE;
    }

    if (maxChunkSize < 0) {
        this->maxChunkSize = MAX_CHUNK_SIZE;
    }
}

HttpPackage HttpMessageDecoder::decode(ChannelHandlerContext& ctx,
                                       const ReplayingDecoderBufferPtr& buffer,
                                       int state) {
    switch (state) {
    case SKIP_CONTROL_CHARS: {
        if (skipControlCharacters(buffer)) {
            checkpoint(READ_INITIAL);
        }
        else {
            checkpoint(); //do not care skipped control chars.
        }

        break;
    }

    case READ_INITIAL: {
        std::vector<StringPiece> initialLine;
        StringPiece line = readLine(buffer, maxInitialLineLength);

        if (!splitInitialLine(line, &initialLine)) {
            // need more bytes.
            break;
        }

        if (initialLine.size() < 3) {
            // Invalid initial line - ignore.
            checkpoint(SKIP_CONTROL_CHARS);
            break;
        }

        message = createMessage(initialLine[0],
                                initialLine[1],
                                initialLine[2]);
        checkpoint(READ_HEADER);

        // clear data, then step into the READ_HEADER state.
        headerSize = 0;
        message->clearHeaders();
        break;
    }

    case READ_HEADER: {
        int nextState = readHeaders(buffer);

        if (nextState == READ_HEADER) {
            // need read more bytes to parse headers.
            break;
        }

        checkpoint(nextState);

        if (nextState == READ_CHUNK_SIZE) {
            // Chunked encoding - generate HttpMessage first.  HttpChunks will follow.
            return message;
        }
        else if (nextState == SKIP_CONTROL_CHARS) {
            // No content is expected.
            return message;
        }
        else {
            int contentLength = HttpHeaders::getContentLength(*message, -1);

            if (contentLength == 0 || (contentLength == -1 && isDecodingRequest())) {
                content = Unpooled::EMPTY_BUFFER;
                return reset();
            }

            switch (nextState) {
            case READ_FIXED_LENGTH_CONTENT:
                if (contentLength > maxChunkSize
                        || HttpHeaders::is100ContinueExpected(*message)) {
                    // Generate HttpMessage first.  HttpChunks will follow.
                    checkpoint(READ_FIXED_LENGTH_CONTENT_AS_CHUNKS);
                    message->setTransferEncoding(HttpTransferEncoding::STREAMED);

                    // chunkSize will be decreased as the READ_FIXED_LENGTH_CONTENT_AS_CHUNKS
                    // state reads data chunk by chunk.
                    chunkSize = HttpHeaders::getContentLength(*message, -1);
                    return message;
                }

                break;

            case READ_VARIABLE_LENGTH_CONTENT:
                if (buffer->readableBytes() > maxChunkSize
                        || HttpHeaders::is100ContinueExpected(*message)) {
                    // Generate HttpMessage first.  HttpChunks will follow.
                    checkpoint(READ_VARIABLE_LENGTH_CONTENT_AS_CHUNKS);
                    message->setTransferEncoding(HttpTransferEncoding::STREAMED);
                    return message;
                }

                break;

            default:
                throw IllegalStateException(
                    StringUtil::strprintf("Unexpected state: %d", nextState));
            }
        }

        // We return null here, this forces decode to be called again where we will decode the content
        return HttpMessagePtr();
    }

    case READ_VARIABLE_LENGTH_CONTENT: {
        int chunkSize = std::min(maxChunkSize, buffer->readableBytes());
        ChannelBufferPtr buff = buffer->readSlice(chunkSize);

        if (buffer->needMoreBytes()) {
            return HttpMessagePtr();
        }

        HttpChunkPtr chunk = new HttpChunk(buff);

        if (message->getTransferEncoding() != HttpTransferEncoding::STREAMED) {
            message->setTransferEncoding(HttpTransferEncoding::STREAMED);

            //return new Object[] {message, new DefaultHttpChunk(buffer.readBytes(toRead))};
        }
        else {
            return chunk;
        }
    }

    case READ_VARIABLE_LENGTH_CONTENT_AS_CHUNKS: {
        // Keep reading data as a chunk until the end of connection is reached.
        int chunkSize = std::min(maxChunkSize, buffer->readableBytes());
        ChannelBufferPtr buff = buffer->readSlice(chunkSize);

        if (buffer->needMoreBytes()) {
            return HttpMessagePtr();
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
        BOOST_ASSERT(chunkSize <= Integer::MAX_VALUE);

        int chunkSize = this->chunkSize;
        int readLimit = buffer->readableBytes();

        // Check if the buffer is readable first as we use the readable byte count
        // to create the HttpChunk. This is needed as otherwise we may end up with
        // create a HttpChunk instance that contains an empty buffer and so is
        // handled like it is the last HttpChunk.
        if (readLimit == 0) {
            return HttpMessagePtr();
        }

        int toRead = chunkSize;

        if (toRead > maxChunkSize) {
            toRead = maxChunkSize;
        }

        if (toRead > readLimit) {
            toRead = readLimit;
        }


        ChannelBufferPtr buff = buffer->readSlice(toRead);

        if (buffer->needMoreBytes()) {
            return HttpMessagePtr();
        }

        HttpChunkPtr chunk = HttpChunkPtr(new HttpChunk(buff));

        if (chunkSize > toRead) {
            chunkSize -= toRead;
        }
        else {
            chunkSize = 0;
        }

        this->chunkSize = chunkSize;

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
        StringPiece line = readLine(buffer, maxInitialLineLength);

        if (buffer->needMoreBytes()) {
            return HttpMessagePtr();
        }

        chunkSize = getChunkSize(line);

        if (chunkSize == 0) {
            checkpoint(READ_CHUNK_FOOTER);
            return HttpMessagePtr();
        }
        else if (chunkSize > maxChunkSize) {
            // A chunk is too large. Split them into multiple chunks again.
            checkpoint(READ_CHUNKED_CONTENT_AS_CHUNKS);
        }
        else {
            checkpoint(READ_CHUNKED_CONTENT);
        }

        break;
    }

    case READ_CHUNKED_CONTENT: {
        BOOST_ASSERT(chunkSize <= Integer::MAX_VALUE);
        ChannelBufferPtr buff = buffer->readSlice(chunkSize);

        if (buffer->needMoreBytes()) {
            return HttpMessagePtr();
        }

        HttpChunkPtr chunk = HttpChunkPtr(new HttpChunk(buff));
        checkpoint(READ_CHUNK_DELIMITER);
        return chunk;
    }

    case READ_CHUNKED_CONTENT_AS_CHUNKS: {
        BOOST_ASSERT(chunkSize <= Integer::MAX_VALUE);

        int chunkSize = this->chunkSize;
        int readLimit = buffer->readableBytes();

        // Check if the buffer is readable first as we use the readable byte count
        // to create the HttpChunk. This is needed as otherwise we may end up with
        // create a HttpChunk instance that contains an empty buffer and so is
        // handled like it is the last HttpChunk.
        if (readLimit == 0) {
            return HttpMessagePtr();
        }

        int toRead = chunkSize;

        if (toRead > maxChunkSize) {
            toRead = maxChunkSize;
        }

        if (toRead > readLimit) {
            toRead = readLimit;
        }

        ChannelBufferPtr buff = buffer->readSlice(toRead);

        if (buffer->needMoreBytes()) {
            return HttpMessagePtr();
        }

        HttpChunkPtr chunk = HttpChunkPtr(new HttpChunk(buff));

        if (chunkSize > toRead) {
            chunkSize -= toRead;
        }
        else {
            chunkSize = 0;
        }

        this->chunkSize = chunkSize;

        if (chunkSize == 0) {
            // Read all content.
            checkpoint(READ_CHUNK_DELIMITER);
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
                return HttpMessagePtr();
            }

            if (next == HttpCodecUtil::CR) {
                if (buffer->readByte() == HttpCodecUtil::LF) {
                    checkpoint(READ_CHUNK_SIZE);
                    return HttpMessagePtr();
                }

                if (buffer->needMoreBytes()) {
                    return HttpMessagePtr();
                }
            }
            else if (next == HttpCodecUtil::LF) {
                checkpoint(READ_CHUNK_SIZE);
                return HttpMessagePtr();
            }
        }

        break;
    }

    case READ_CHUNK_FOOTER: {
        HttpChunkTrailerPtr trailer = readTrailingHeaders(buffer);

        if (maxChunkSize == 0) {
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

    return HttpMessagePtr();
}

bool HttpMessageDecoder::isContentAlwaysEmpty(const HttpMessage& msg) const {
    const HttpResponse* response = dynamic_cast<const HttpResponse*>(&msg);

    if (response) {
        int code = response->getStatus().getCode();

        // Correctly handle return codes of 1xx.
        //
        // See:
        //     - http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html Section 4.4
        if (code >= 100 && code < 200) {
            if (code == 101
                    && !response->containsHeader(
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

HttpMessagePtr HttpMessageDecoder::reset() {
    HttpMessagePtr message = this->message;
    ChannelBufferPtr content = this->content;

    if (content) {
        message->setContent(content);
        this->content.reset();
    }

    this->message.reset();

    checkpoint(SKIP_CONTROL_CHARS);
    return message;
}

bool HttpMessageDecoder::skipControlCharacters(const ReplayingDecoderBufferPtr& buffer) const {
    for (;;) {
        uint8_t c = buffer->readUnsignedByte();

        if (buffer->needMoreBytes()) {
            return false;
        }

        if (!Character::isISOControl(c) && !Character::isWhitespace(c)) {
            buffer->offsetReaderIndex(-1);
            return true;
        }
    }
}

HttpPackage HttpMessageDecoder::readFixedLengthContent(const ReplayingDecoderBufferPtr& buffer) {
    int length = HttpHeaders::getContentLength(*message, -1);
    BOOST_ASSERT(length <= Integer::MAX_VALUE);
    int readLimit = buffer->readableBytes();

    int toRead = (int) length - contentRead;

    if (toRead > readLimit) {
        toRead = readLimit;
    }

    contentRead += toRead;
    //content.reset();
    //content = buffer->readSlice(toRead);
    BOOST_ASSERT(false && "NOT implement in HttpMessageDecoder readFixedLengthContent.");

    if (length < contentRead) {
        if (message->getTransferEncoding() != HttpTransferEncoding::STREAMED) {
            message->setTransferEncoding(HttpTransferEncoding::STREAMED);
            //return new Object[] {message, new DefaultHttpChunk(read(buffer, toRead))};
        }
        else {
            //return new DefaultHttpChunk(read(buffer, toRead));
        }
    }

    if (!content) {
        content = buffer->readSlice(length);
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

int HttpMessageDecoder::readHeaders(const ReplayingDecoderBufferPtr& buffer) {
    int nextState;
    StringPiece line = readHeader(buffer);
    StringPiece name;
    StringPiece value;

    headerSize = 0;

    if (buffer->needMoreBytes()) { // need more data
        return READ_HEADER;
    }

    if (!line.empty()) {
        message->clearHeaders();

        std::vector<StringPiece> header;
        std::string multiValueStr;

        do {
            char firstChar = line[0];

            if (!name.empty() && (firstChar == ' ' || firstChar == '\t')) {
                multiValueStr.clear();
                StringPiece trimedLine = line;//line.trim();

                multiValueStr.append(value.c_str());
                multiValueStr.append(1, ' ');
                multiValueStr.append(trimedLine.data(), trimedLine.size());
            }
            else {
                if (!name.empty()) {
                    if (!multiValueStr.empty()) {
                        message->addHeader(name.c_str(), multiValueStr);
                    }
                    else {
                        message->addHeader(name.c_str(), value.c_str());
                    }
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
            if (!multiValueStr.empty()) {
                message->addHeader(name.c_str(), multiValueStr);
            }
            else {
                message->addHeader(name.c_str(), value.c_str());
            }
        }
    }

    if (isContentAlwaysEmpty(*message)) {
        message->setTransferEncoding(HttpTransferEncoding::SINGLE);
        nextState = SKIP_CONTROL_CHARS;
    }
    else if (HttpCodecUtil::isTransferEncodingChunked(*message)) {
        message->setTransferEncoding(HttpTransferEncoding::CHUNKED);
        nextState = READ_CHUNK_SIZE;
    }
    else if (HttpHeaders::getContentLength(*message, -1) >= 0) {
        nextState = READ_FIXED_LENGTH_CONTENT;
    }
    else {
        nextState = READ_VARIABLE_LENGTH_CONTENT;
    }

    return nextState;
}

HttpChunkTrailerPtr
HttpMessageDecoder::readTrailingHeaders(const ReplayingDecoderBufferPtr& buffer) {
    headerSize = 0;
    StringPiece line = readHeader(buffer);
    StringPiece lastHeader;

    if (!line.empty()) {
        std::vector<StringPiece> header;
        HttpChunkTrailerPtr trailer = new HttpChunkTrailer;

        do {
            char firstChar = line[0];

            if (!lastHeader.empty() && (firstChar == ' ' || firstChar == '\t')) {
                //std::string headers;
                //trailer->getHeaders(lastHeader, &headers);

                //if (!headers.empty()) {
                //    int lastPos = current.size() - 1;
                //    String newString = current.get(lastPos) + line.trim();
                //    current.set(lastPos, newString);
                //}
                //else {
                // Content-Length, Transfer-Encoding, or Trailer
                //}
            }
            else {
                header.clear();
                splitHeader(line, &header);
                StringPiece name = header[0];

                if (!name.iequals(HttpHeaders::Names::CONTENT_LENGTH) &&
                        !name.iequals(HttpHeaders::Names::TRANSFER_ENCODING) &&
                        !name.iequals(HttpHeaders::Names::TRAILER)) {
                    trailer->addHeader(name.c_str(), header[1].c_str());
                }

                lastHeader = name;
            }

            line = readHeader(buffer);
        }
        while (!line.empty());

        return trailer;
    }

    return boost::dynamic_pointer_cast<HttpChunkTrailer>(HttpChunk::LAST_CHUNK);
}

StringPiece HttpMessageDecoder::readHeader(const ReplayingDecoderBufferPtr& buffer) {
    StringPiece str;
    StringPiece bytes;
    buffer->readableBytes(&bytes);

    int headerSize = this->headerSize;
    int strSize = 0;

    for (int i = 0, j = bytes.length() - 1; i < j; ++i, ++headerSize) {
        // Abort decoding if the header part is too large.
        if (headerSize >= maxHeaderSize) {
            // TODO: Respond with Bad Request and discard the traffic
            //    or close the connection.
            //       No need to notify the upstream handlers - just log.
            //       If decoding a response, just throw an exception.
            throw TooLongFrameException(
                StringUtil::strprintf("HTTP header is larger than %d bytes.", maxHeaderSize));
        }

        if (bytes[i] == HttpCodecUtil::CR && bytes[i+1] == HttpCodecUtil::LF) {
            buffer->offsetReaderIndex(i+2);
            return StringPiece(bytes.data(), i);
        }
    }

    return StringPiece();
}

int HttpMessageDecoder::getChunkSize(const StringPiece& hex) const {
    StringPiece h = hex.trim();

    for (int i = 0; i < h.length(); ++i) {
        char c = h.at(i);

        if (c == ';' || Character::isWhitespace(c) || Character::isISOControl(c)) {
            h = h.substr(0, i);
            break;
        }
    }

    return Integer::parse(h.c_str(), 16);
}

StringPiece HttpMessageDecoder::readLine(const ReplayingDecoderBufferPtr& buffer,
        int maxLineLength) {
    StringPiece bytes;
    buffer->readableBytes(&bytes);

    if (bytes.length() >= maxLineLength) {
        // TODO: Respond with Bad Request and discard the traffic
        //    or close the connection.
        //       No need to notify the upstream handlers - just log.
        //       If decoding a response, just throw an exception.
        throw TooLongFrameException(
            StringUtil::strprintf("An HTTP line is larger than %d bytes.", maxLineLength));
    }

    for (int i = 0, j = bytes.length() - 1; i < j; ++i) {
        if (bytes[i] == HttpCodecUtil::CR && bytes[i+1] == HttpCodecUtil::LF) {
            buffer->offsetReaderIndex(i+2);
            return StringPiece(bytes.data(), i);
        }
    }

    return StringPiece();
}

bool HttpMessageDecoder::splitInitialLine(StringPiece& sb, std::vector<StringPiece>* lines) {
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

void HttpMessageDecoder::splitHeader(StringPiece& sb, std::vector<StringPiece>* header) {
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

        if (ch == ':' || Character::isWhitespace(ch)) {
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

int HttpMessageDecoder::findNonWhitespace(const StringPiece& sb, int offset) {
    int result;

    for (result = offset; result < sb.length(); ++result) {
        if (!Character::isWhitespace(sb.at(result))) {
            break;
        }
    }

    return result;
}

int HttpMessageDecoder::findWhitespace(const StringPiece& sb, int offset) {
    int result;

    for (result = offset; result < sb.length(); ++result) {
        if (Character::isWhitespace(sb.at(result))) {
            break;
        }
    }

    return result;
}

int HttpMessageDecoder::findEndOfString(const StringPiece& sb) {
    int result;

    for (result = sb.length(); result > 0; --result) {
        if (!Character::isWhitespace(sb.at(result - 1))) {
            break;
        }
    }

    return result;
}

}
}
}
}
