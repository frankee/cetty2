#if !defined(CETTY_UTIL_SMALLFILE_H)
#define CETTY_UTIL_SMALLFILE_H

// take from muduo FileUtil.h
// Copyright 2010, Shuo Chen. All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#include <cstdio>
#include <string>
#include <cetty/Types.h>
#include <boost/noncopyable.hpp>
#include <cetty/util/StringPiece.h>

namespace cetty {
namespace util {

class SmallFile : boost::noncopyable {
public:
    SmallFile(const StringPiece& filename);
    ~SmallFile();

    int readToString(std::string* content,
                     int64_t* fileSize,
                     int64_t* modifyTime,
                     int64_t* createTime);

    // return errno
    int readToString(int maxSize,
                     std::string* content,
                     int64_t* fileSize,
                     int64_t* modifyTime,
                     int64_t* createTime);

    // return errno
    int readToBuffer(int* size);

    const char* buffer() const;

    // read the file content, returns errno if error happens.
    static int readFile(const StringPiece& filename,
                        std::string* content,
                        int64_t* fileSize = NULL,
                        int64_t* modifyTime = NULL,
                        int64_t* createTime = NULL);

    static int readFile(const StringPiece& filename,
                        int maxSize,
                        std::string* content,
                        int64_t* fileSize = NULL,
                        int64_t* modifyTime = NULL,
                        int64_t* createTime = NULL);

private:
    static const int MAX_BUFFER_SIZE = 65536;

private:
    int fd_;
    int err_;
    FILE* file_;

    char buf_[MAX_BUFFER_SIZE];
};

inline
const char* SmallFile::buffer() const {
    return buf_;
}

inline
int SmallFile::readToString(std::string* content,
                            int64_t* fileSize,
                            int64_t* modifyTime,
                            int64_t* createTime) {
    return readToString(MAX_BUFFER_SIZE,
                        content,
                        fileSize,
                        modifyTime,
                        createTime);
}

inline
int SmallFile::readFile(const StringPiece& filename,
                        std::string* content,
                        int64_t* fileSize,
                        int64_t* modifyTime,
                        int64_t* createTime) {
    return readFile(filename,
                    MAX_BUFFER_SIZE,
                    content,
                    fileSize,
                    modifyTime,
                    createTime);
}

}
}

#endif //#if !defined(CETTY_UTIL_SMALLFILE_H)

// Local Variables:
// mode: c++
// End:
