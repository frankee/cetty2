#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCODECUTIL_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPCODECUTIL_H

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

#include <string>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class HttpMessage;

/**
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class HttpCodecUtil {
public:
    //space ' '
    static const char SP = 32;

    //tab ' '
    static const char HT = 9;

    /**
     * Carriage return
     */
    static const char CR = 13;

    /**
     * Equals '='
     */
    static const char EQUALS = 61;

    /**
     * Line feed character
     */
    static const char LF = 10;

    /**
     * carriage return line feed
     */
    static const char CRLF[2];

    /**
    * Colon ':'
    */
    static const char COLON = 58;

    /**
    * Semicolon ';'
    */
    static const char SEMICOLON = 59;

    /**
    * comma ','
    */
    static const char COMMA = 44;

    static const char DOUBLE_QUOTE = '"';

public:
    static bool validateHeaderName(const std::string& name);
    static bool validateHeaderValue(const std::string& value);
    static bool isTransferEncodingChunked(const HttpMessage& m);
    static bool isContentLengthSet(const HttpMessage& m);

    //static void splitElements(const std::string& s, std::vector<std::string>& elements, bool ignoreEmpty = true);
    /// Splits the given string into separate elements. Elements are expected
    /// to be separated by commas.
    ///
    /// For example, the string 
    ///   text/plain; q=0.5, text/html, text/x-dvi; q=0.8
    /// is split into the elements
    ///   text/plain; q=0.5
    ///   text/html
    ///   text/x-dvi; q=0.8
    ///
    /// Commas enclosed in double quotes do not split elements.
    ///
    /// If ignoreEmpty is true, empty elements are not returned.

    //static void splitParameters(const std::string& s, std::string& value, NameValueCollection& parameters);
    /// Splits the given string into a value and a collection of parameters.
    /// Parameters are expected to be separated by semicolons.
    ///
    /// Enclosing quotes of parameter values are removed.
    ///
    /// For example, the string
    ///   multipart/mixed; boundary="MIME_boundary_01234567"
    /// is split into the value
    ///   multipart/mixed
    /// and the parameter
    ///   boundary -> MIME_boundary_01234567

    //static void splitParameters(const std::string::const_iterator& begin, const std::string::const_iterator& end, NameValueCollection& parameters);
    /// Splits the given string into a collection of parameters.
    /// Parameters are expected to be separated by semicolons.
    ///
    /// Enclosing quotes of parameter values are removed.

    //static void quote(const std::string& value, std::string& result, bool allowSpace = false);
    /// Checks if the value must be quoted. If so, the value is
    /// appended to result, enclosed in double-quotes.
    /// Otherwise, the value is appended to result as-is.

private:
    HttpCodecUtil() {}
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCODECUTIL_H)

// Local Variables:
// mode: c++
// End:
