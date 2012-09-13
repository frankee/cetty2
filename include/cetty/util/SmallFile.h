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
#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <cetty/util/StringPiece.h>

namespace cetty {
namespace util {

class SmallFile : boost::noncopyable {
public:
    SmallFile(const StringPiece& filename);
    ~SmallFile();

    // return errno
    int readToString(int maxSize,
                     std::string* content,
                     boost::int64_t* fileSize,
                     boost::int64_t* modifyTime,
                     boost::int64_t* createTime);

    // return errno
    int readToBuffer(int* size);

    const char* buffer() const { return buf; }

    // read the file content, returns errno if error happens.
    static int readFile(const StringPiece& filename,
                        int maxSize,
                        std::string* content,
                        boost::int64_t* fileSize = NULL,
                        boost::int64_t* modifyTime = NULL,
                        boost::int64_t* createTime = NULL);

private:
    static const int MAX_BUFFER_SIZE = 65536;

private:
    int fd;
    int err;
    FILE* file;

    char buf[MAX_BUFFER_SIZE];
};

}
}

#endif //#if !defined(CETTY_UTIL_SMALLFILE_H)

// Local Variables:
// mode: c++
// End:
