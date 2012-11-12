// Copyright 2010, Shuo Chen. All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//

#include <cetty/util/SmallFile.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <boost/static_assert.hpp>
#include <boost/implicit_cast.hpp>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace util {

#if defined(_WIN32)

SmallFile::SmallFile(const StringPiece& filename)
    : fd(0), err(0), file(0) {
    buf[0] = '\0';
    file = std::fopen(filename.data(), "rb");
}

SmallFile::~SmallFile() {
    if (file) {
        std::fclose(file);
    }
}

// return errno
int SmallFile::readToString(int maxSize,
                            std::string* content,
                            int64_t* fileSize,
                            int64_t* modifyTime,
                            int64_t* createTime) {
    assert(content != NULL);
    if (fd >= 0) {
        content->clear();

        while (content->size() < (size_t)maxSize) {
            size_t toRead = std::min((size_t)maxSize - content->size(), sizeof(buf));
            size_t n = std::fread(buf, toRead, 1, file);

            if (n > 0) {
                content->append(buf, n);
            }
            else {
                break;
            }
        }
    }

    return 0;
}

int SmallFile::readToBuffer(int* size) {
    if (file) {
        std::size_t n = std::fread(buf, sizeof(buf) - 1, 1, file);

        if (size) {
            *size = static_cast<int>(n);
        }
    }

    return 0;
}

#else

SmallFile::SmallFile(const StringPiece& filename)
    : fd(::open(filename.c_str(), O_RDONLY)),
      err(0),
      file(0) {
    buf[0] = '\0';

    if (fd < 0) {
        LOG_DEBUG << "open filename.data() failed.";
        err = errno;
    }
}

SmallFile::~SmallFile() {
    if (fd >= 0) {
        ::close(fd); // FIXME: check EINTR
    }
}

int SmallFile::readToBuffer(int* size) {
    int err = err;

    if (fd >= 0) {
    	fsync(fd);
        ssize_t n = ::pread(fd, buf, sizeof(buf)-1, 0);

        if (n >= 0) {
            if (size) {
                *size = static_cast<int>(n);
            }

            buf[n] = '\0';
        }
        else {
            err = errno;
        }
    }

    return err;
}

// return errno
int SmallFile::readToString(int maxSize,
                            std::string* content,
                            int64_t* fileSize,
                            int64_t* modifyTime,
                            int64_t* createTime) {
    BOOST_STATIC_ASSERT(sizeof(off_t) == 8);
    assert(content != NULL);
    int err = err;

    if (fd >= 0) {
        content->clear();

        if (fileSize) {
            struct stat statbuf;

            if (::fstat(fd, &statbuf) == 0) {
                if (S_ISREG(statbuf.st_mode)) {
                    *fileSize = statbuf.st_size;
                    content->reserve(static_cast<int>(std::min(boost::implicit_cast<int64_t>(maxSize), *fileSize)));
                }
                else if (S_ISDIR(statbuf.st_mode)) {
                    err = EISDIR;
                }

                if (modifyTime) {
                    *modifyTime = statbuf.st_mtime;
                }

                if (createTime) {
                    *createTime = statbuf.st_ctime;
                }
            }
            else {
                err = errno;
            }
        }

        while (content->size() < (size_t)maxSize) {
            size_t toRead = std::min((size_t)maxSize - content->size(), sizeof(buf));
            ssize_t n = ::read(fd, buf, toRead);

            if (n > 0) {
                content->append(buf, n);
            }
            else {
                if (n < 0) {
                    err = errno;
                }

                break;
            }
        }
    }

    return err;
}

#endif

int SmallFile::readFile(const StringPiece& filename,
                        int maxSize,
                        std::string* content,
                        int64_t* fileSize /*= NULL*/,
                        int64_t* modifyTime /*= NULL*/,
                        int64_t* createTime /*= NULL*/) {
    SmallFile file(filename);
    return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}

}
}
