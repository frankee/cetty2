#if !defined(CETTY_UTIL_STRINGUTIL_H)
#define CETTY_UTIL_STRINGUTIL_H

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

#include <string>
#include <vector>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>

#include <cetty/Types.h>

namespace cetty {
namespace util {

class StringPiece;

class StringUtil {
public:
    static const std::string NEWLINE;

    static inline bool isISOControl(int ch) {
        return iscntrl(ch) != 0;
    }

    static inline bool isWhitespace(int ch) {
        return isspace(ch) != 0;
    }

    static inline bool isDigit(int ch) {
        return ('0' <= ch && ch <= '9');
    }

    static inline bool isHex(int ch) {
        return (ch >= '0' && ch <= '9')
               || (ch >= 'a' && ch <= 'f')
               || (ch >= 'A' && ch <= 'F');
    }

    static inline bool isAlnum(char c) {
        return ('a' <= c && c <= 'z') ||
               ('A' <= c && c <= 'Z') ||
               ('0' <= c && c <= '9');
    }

    static int hashCode(const std::string& str);

    /**
     * Append a formatted string at the end of a string.
     * @param dest the destination string.
     * @param format the printf-like format string.  The conversion character `%' can be used with
     * such flag characters as `s', `d', `o', `u', `x', `X', `c', `e', `E', `f', `g', `G', and `%'.
     * @param ... used according to the format string.
     */
    static void printf(std::string* dest, const char* format, ...);

    /**
     * Generate a formatted string.
     * @param format the printf-like format string.  The conversion character `%' can be used with
     * such flag characters as `s', `d', `o', `u', `x', `X', `c', `e', `E', `f', `g', `G', and `%'.
     * @param ... used according to the format string.
     * @return the result string.
     */
    static std::string printf(const char* format, ...);

    /**
     * Split a string with a delimiter.
     * @param str the string.
     * @param delim the delimiter.
     * @param elems a vector object into which the result elements are pushed.
     * @return the number of result elements.
     */
    static size_t split(const std::string& str,
                        char delim,
                        std::vector<std::string>* elems);

    /**
     * Split a string with delimiters.
     * @param str the string.
     * @param delims the delimiters.
     * @param elems a vector object into which the result elements are pushed.
     * @return the number of result elements.
     */
    static size_t split(const std::string& str,
                        const std::string& delims,
                        std::vector<std::string>* elems);

    /**
     * Convert the letters of a string into upper case.
     * @param str the string to convert.
     * @return the string itself.
     */
    static std::string* toUpper(std::string* str);

    /**
     * Convert the letters of a string into lower case.
     * @param str the string to convert.
     * @return the string itself.
     */
    static std::string* toLower(std::string* str);

    static inline bool iequals(const std::string& astr, const std::string& bstr) {
        return iequals(astr.c_str(), bstr.c_str());
    }

    static inline bool iequals(const char* astr, const char* bstr) {
        BOOST_ASSERT(astr && bstr);

        while (*astr != '\0') {
            if (*astr != *bstr
                    && *astr + 0x20 != *bstr
                    && *astr != *bstr + 0x20) {
                return false;
            }

            ++astr;
            ++bstr;
        }

        return *bstr == '\0';
    }

    static inline int icompare(const std::string& astr, const std::string& bstr) {
        return icompare(astr.c_str(), bstr.c_str());
    }

    /**
     * Compare two strings by case insensitive evaluation.
     */
    static inline int icompare(const char* astr, const char* bstr) {
        BOOST_ASSERT(astr && bstr);

        while (*astr != '\0') {
            if (*bstr == '\0') { return 1; }

            int ac = *(unsigned char*)astr;

            if (ac >= 'A' && ac <= 'Z') { ac += 'a' - 'A'; }

            int bc = *(unsigned char*)bstr;

            if (bc >= 'A' && bc <= 'Z') { bc += 'a' - 'A'; }

            if (ac != bc) { return ac - bc; }

            astr++;
            bstr++;
        }

        return (*bstr == '\0') ? 0 : -1;
    }

    // HasPrefixString()
    //    Check if a string begins with a given prefix.
    static inline bool hasPrefixString(const std::string& str,
                                       const std::string& prefix) {
        return str.size() >= prefix.size() &&
               str.compare(0, prefix.size(), prefix) == 0;
    }

    // StripPrefixString()
    //    Given a string and a putative prefix, returns the string minus the
    //    prefix string if the prefix matches, otherwise the original
    //    string.
    static inline std::string stripPrefixString(const std::string& str,
            const std::string& prefix) {
        if (hasPrefixString(str, prefix)) {
            return str.substr(prefix.size());
        }
        else {
            return str;
        }
    }

    // HasSuffixString()
    //    Return true if str ends in suffix.
    static inline bool hasSuffixString(const std::string& str,
                                       const std::string& suffix) {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    // StripSuffixString()
    //    Given a string and a putative suffix, returns the string minus the
    //    suffix string if the suffix matches, otherwise the original
    //    string.
    static inline std::string stripSuffixString(const std::string& str,
            const std::string& suffix) {
        if (hasSuffixString(str, suffix)) {
            return str.substr(0, str.size() - suffix.size());
        }
        else {
            return str;
        }
    }

    // ----------------------------------------------------------------------
    // StripString
    //    Replaces any occurrence of the character 'remove' (or the characters
    //    in 'remove') with the character 'replacewith'.
    //    Good for keeping html characters or protocol characters (\t) out
    //    of places where they might cause a problem.
    // ----------------------------------------------------------------------
    static inline std::string* stripString(std::string* s, const char* remove, char replacewith) {
        const char* strStart = s->c_str();
        const char* str = strStart;

        for (str = strpbrk(str, remove);
                str != NULL;
                str = strpbrk(str + 1, remove)) {
            (*s)[str - strStart] = replacewith;
        }

        return s;
    }

    /**
     * Strip a String of it's ISO control characters.
     *
     * @param value
     *          The String that should be stripped.
     * @return {@code String}
     *          A new String instance with its hexadecimal control characters replaced
     *          by a space. Or the unmodified String if it does not contain any ISO
     *          control characters.
     */
    static std::string stripControlCharacters(const std::string& value);
    static std::string* stripControlCharacters(std::string* str);

    /**
    * Cut space characters at head or tail of a string.
    * @param str the string to convert.
    * @return the string itself.
    */
    static std::string* trim(std::string* str);

    // ----------------------------------------------------------------------
    // StringReplace()
    //    Give me a string and two patterns "old" and "new", and I replace
    //    the first instance of "old" in the string with "new", if it
    //    exists.  RETURN a new string, regardless of whether the replacement
    //    happened or not.
    // ----------------------------------------------------------------------
    static void replace(const std::string& s,
                        const std::string& oldsub,
                        const std::string& newsub,
                        bool replaceAll,
                        std::string* res) {
        if (oldsub.empty()) {
            res->append(s);  // if empty, append the given string.
            return;
        }

        std::string::size_type startPos = 0;
        std::string::size_type pos;

        do {
            pos = s.find(oldsub, startPos);

            if (pos == std::string::npos) {
                break;
            }

            res->append(s, startPos, pos - startPos);
            res->append(newsub);
            startPos = pos + oldsub.size();  // start searching again after the "old"
        }
        while (replaceAll);

        res->append(s, startPos, s.length() - startPos);
    }

    static inline std::string replace(const std::string& s,
                                      const std::string& oldsub,
                                      const std::string& newsub,
                                      bool replace_all) {
        std::string ret;
        replace(s, oldsub, newsub, replace_all, &ret);
        return ret;
    }

    // ----------------------------------------------------------------------
    // JoinStrings()
    //    These methods concatenate a vector of strings into a C++ std::string, using
    //    the C-string "delim" as a separator between components. There are two
    //    flavors of the function, one flavor returns the concatenated string,
    //    another takes a pointer to the target string. In the latter case the
    //    target string is cleared and overwritten.
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    // JoinStrings()
    //    This merges a vector of string components with delim inserted
    //    as separaters between components.
    //
    // ----------------------------------------------------------------------
    static std::string* join(const std::vector<std::string>& components,
                             const char* delim,
                             std::string* result);

    static std::string join(const std::vector<std::string>& components,
                            const char* delim);

    /**
     * Convert a decimal string to an integer.
     * @param str the decimal string.
     * @return the integer.  If the string does not contain numeric expression, 0 is returned.
     */
    static int32_t strto32(const char* str);

    /**
     * Convert a decimal string to an integer.
     * @param str the decimal string.
     * @return the integer.  If the string does not contain numeric expression, 0 is returned.
     */
    static int32_t strto32(const std::string& str);

    /**
     * Convert a decimal string to an integer.
     * @param str the decimal string.
     * @return the integer.  If the string does not contain numeric expression, 0 is returned.
     */
    static int32_t strto32(const StringPiece& str);

    static int32_t strto32(const char* str, int base);

    static int32_t strto32(const std::string& str, int base);

    static int32_t strto32(const StringPiece& str, int base);

    /**
     * Convert a decimal string to an integer.
     * @param str the decimal string.
     * @return the integer.  If the string does not contain numeric expression, 0 is returned.
     */
    static int64_t strto64(const char* str);

    /**
     * Convert a decimal string to an integer.
     * @param str the decimal string.
     * @return the integer.  If the string does not contain numeric expression, 0 is returned.
     */
    static int64_t strto64(const std::string& str);

    /**
     * Convert a decimal string to an integer.
     * @param str the decimal string.
     * @return the integer.  If the string does not contain numeric expression, 0 is returned.
     */
    static int64_t strto64(const StringPiece& str);

    static int64_t strto64(const char* str, int base);

    static int64_t strto64(const std::string& str, int base);

    static int64_t strto64(const StringPiece& str, int base);

    /**
     *
     *
     */
    static inline double strtof(const std::string& str) {
        return strtof(str.c_str());
    }

    /**
     * Convert a decimal string to a real number.
     * @param str the decimal string.
     * @return the real number.  If the string does not contain numeric expression, 0.0 is returned.
     */
    static double strtof(const char* str);

    static std::string numtostr(int number);
    static std::string* numtostr(int number, std::string* str);

    static std::string numtostr(unsigned int number);
    static std::string* numtostr(unsigned int number, std::string* str);

    static std::string numtostr(long number);
    static std::string* numtostr(long number, std::string* str);

    static std::string numtostr(unsigned long number);
    static std::string* numtostr(unsigned long number, std::string* str);

    static std::string numtostr(long long number);
    static std::string* numtostr(long long number, std::string* str);

    static std::string numtostr(unsigned long long number);
    static std::string* numtostr(unsigned long long number, std::string* str);

    //    Description: converts a double or float to a string which, if
    //    passed to NoLocaleStrtod(), will produce the exact same original double
    //    (except in case of NaN; all NaNs are considered the same value).
    //    We try to keep the string short but it's not guaranteed to be as
    //    short as possible.
    //
    //    DoubleToBuffer() and FloatToBuffer() write the text to the given
    //    buffer and return it.  The buffer must be at least
    //    kDoubleToBufferSize bytes for doubles and kFloatToBufferSize
    //    bytes for floats.  kFastToBufferSize is also guaranteed to be large
    //    enough to hold either.
    //
    //    Return value: string
    // ----------------------------------------------------------------------
    static std::string numtostr(float number);
    static std::string* numtostr(float number, std::string* str);

    static std::string numtostr(double number);
    static std::string* numtostr(double number, std::string* str);

    static inline std::string hextostr(int hex) {
        if (hex >= 0) {
            return hextostr(static_cast<uint32_t>(hex));
        }
    }
    static std::string* hextostr(int hex, std::string* str) {
        if (hex >= 0) {
            return hextostr(static_cast<uint32_t>(hex), str);
        }
    }

    static std::string hextostr(uint32_t hex);
    static std::string* hextostr(uint32_t hex, std::string* str);

    static std::string hextostr(uint64_t hex);
    static std::string* hextostr(uint64_t hex, std::string* str);

    /**
     * Convert a UTF-8 string into a UCS wstring.
     * @param src the source object.
     * @param dest the destination object.
     */
    static void utftoucs(const std::string& src, std::wstring* dest);

    /**
     * Convert a UTF-8 string into a UCS-4 array.
     * @param src the source object.
     * @param dest the destination object.
     */
    static void utftoucs(const std::string& src, std::vector<uint32_t>* dest);

    /**
     * Convert a UCS-4 array into a UTF-8 string.
     * @param src the source object.
     * @param dest the destination object.
     */
    static void ucstoutf(const std::vector<uint32_t>& src, std::string* dest);

    /**
     * Convert a UCS wstring into a UTF-8 string.
     * @param src the source object.
     * @param dest the destination object.
     */
    static void ucstoutf(const std::wstring& src, std::string* dest);

private:
    StringUtil() {}
    ~StringUtil() {}
};

}
}

#endif //#if !defined(CETTY_UTIL_STRINGUTIL_H)

// Local Variables:
// mode: c++
// End:
