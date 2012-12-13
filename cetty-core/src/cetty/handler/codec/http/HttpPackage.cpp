
/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/handler/codec/http/HttpPackage.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class HttpPackageEmptyVisitor : public boost::static_visitor<bool> {
public:
    bool operator()(const HttpRequestPtr& value) const { return !!value; }
    bool operator()(const HttpResponsePtr& value) const { return !!value; }
    bool operator()(const HttpChunkPtr& value) const { return !!value; }
    bool operator()(const HttpChunkTrailerPtr& value) const { return !!value; }

    template <typename T>
    bool operator()(const T& t) const { return false; }
};

HttpPackage::operator bool() const {
    static HttpPackageEmptyVisitor visitor;
    return variant.apply_visitor(visitor);
}

class HttpPackageHeadersVisitor : public boost::static_visitor<HttpHeaders*> {
public:
    HttpHeaders* operator()(const HttpRequestPtr& value) {
        return &(value->headers());
    }
    HttpHeaders* operator()(const HttpResponsePtr& value) {
        return &(value->headers());
    }
    HttpHeaders* operator()(const HttpChunkTrailerPtr& value) {
        return &(value->headers());
    }

    template <typename T>
    HttpHeaders* operator()(const T& t) { return NULL; }
};

HttpHeaders* HttpPackage::headers() {
    static HttpPackageHeadersVisitor visitor;
    return variant.apply_visitor(visitor);
}

class HttpPackageContentLengthVisitor : public boost::static_visitor<int> {
public:
    int operator()(const HttpRequestPtr& value, int defaultValue) const {
        return value->contentLength(defaultValue);
    }
    int operator()(const HttpResponsePtr& value, int defaultValue) const {
        return value->contentLength(defaultValue);
    }

    template <typename T>
    int operator()(const T& t, int defaultValue) const { return defaultValue; }
};

int HttpPackage::contentLength(int defaultValue) const {
    static HttpPackageContentLengthVisitor visitor;
    boost::variant<int> value(defaultValue);
    return boost::apply_visitor(visitor,
                                variant,
                                value);
}

class HttpPackage100ContinueExpectedVisitor : public boost::static_visitor<bool> {
public:
    bool operator()(const HttpRequestPtr& value) const {
        return value->is100ContinueExpected();
    }

    template <typename T>
    bool operator()(const T& t) const { return false; }
};

bool HttpPackage::is100ContinueExpected() const {
    static HttpPackage100ContinueExpectedVisitor visitor;
    return variant.apply_visitor(visitor);
}

class HttpPackageTransferEncodingVisitor
        : public boost::static_visitor<const HttpTransferEncoding&> {
public:
    const HttpTransferEncoding& operator()(const HttpRequestPtr& value) const {
        return value->transferEncoding();
    }
    const HttpTransferEncoding& operator()(const HttpResponsePtr& value) const {
        return value->transferEncoding();
    }

    template <typename T>
    const HttpTransferEncoding& operator()(const T& t) const {
        return HttpTransferEncoding::SINGLE;
    }
};

const HttpTransferEncoding& HttpPackage::transferEncoding() const {
    static HttpPackageTransferEncodingVisitor visitor;
    return variant.apply_visitor(visitor);
}

class HttpPackageSetTransferEncodingVisitor
        : public boost::static_visitor<void> {
public:
    void operator()(const HttpRequestPtr& value,
                    const HttpTransferEncoding& te) const {
        value->setTransferEncoding(te);
    }
    void operator()(const HttpResponsePtr& value,
                    const HttpTransferEncoding& te) const {
        value->setTransferEncoding(te);
    }

    template <typename T>
    void operator()(const T& t, const HttpTransferEncoding& te) const {}
};

void HttpPackage::setTransferEncoding(const HttpTransferEncoding& te) {
    static HttpPackageSetTransferEncodingVisitor visitor;
    boost::variant<HttpTransferEncoding> transferEncoding(te);
    boost::apply_visitor(visitor,
                         variant,
                         transferEncoding);
}

class HttpPackageSetContentVisitor : public boost::static_visitor<> {
public:
    void operator()(const HttpRequestPtr& value,
                    const ChannelBufferPtr& content) {
        return value->setContent(content);
    }
    void operator()(const HttpResponsePtr& value,
                    const ChannelBufferPtr& content) {
        return value->setContent(content);
    }
    void operator()(const HttpChunkPtr& value,
                    const ChannelBufferPtr& content) {
        return value->setContent(content);
    }
    void operator()(const HttpChunkTrailerPtr& value,
                    const ChannelBufferPtr& content) {
        return value->setContent(content);
    }

    template <typename T>
    void operator()(const T& t, const ChannelBufferPtr& content) {}
};

void HttpPackage::setContent(const ChannelBufferPtr& content) {
    static HttpPackageSetContentVisitor visitor;
    boost::variant<ChannelBufferPtr> buffer(content);
    boost::apply_visitor(visitor,
                         variant,
                         buffer);
}

}
}
}
}
