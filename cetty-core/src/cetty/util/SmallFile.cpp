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
#include <cetty/Platform.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace util {

#if defined(CETTY_OS_FAMILY_WINDOWS)

SmallFile::SmallFile(const StringPiece& filename)
    : fd_(0),
      err_(0),
      file_(0) {
    buf_[0] = '\0';
    file_ = std::fopen(filename.c_str(), "rb");
}

SmallFile::~SmallFile() {
    if (file_) {
        std::fclose(file_);
    }
}

// return errno
int SmallFile::readToString(int maxSize,
                            std::string* content,
                            int64_t* fileSize,
                            int64_t* modifyTime,
                            int64_t* createTime) {
    assert(content != NULL && maxSize > 0);

    if (fd_ >= 0) {
        content->clear();
        std::size_t max = static_cast<std::size_t>(maxSize);

        while (content->size() < max) {
            std::size_t toRead = std::min(max - content->size(), sizeof(buf_));
            std::size_t n = std::fread(buf_, toRead, 1, file_);

            if (n > 0) {
                content->append(buf_, n);
            }
            else {
                break;
            }
        }
    }

    return 0;
}

int SmallFile::readToBuffer(int* size) {
    if (file_) {
        std::size_t n = std::fread(buf_, sizeof(buf_) - 1, 1, file_);

        if (size) {
            *size = static_cast<int>(n);
        }
    }

    return 0;
}

#else

SmallFile::SmallFile(const StringPiece& filename)
    : fd_(::open(filename.c_str(), O_RDONLY)),
      err_(0),
      file_(0) {
    buf_[0] = '\0';

    if (fd_ < 0) {
        LOG_DEBUG << "open filename.data() failed.";
        err_ = errno;
    }
}

SmallFile::~SmallFile() {
    if (fd_ >= 0) {
        ::close(fd_); // FIXME: check EINTR
    }
}

int SmallFile::readToBuffer(int* size) {
    int err = err;

    if (fd_ >= 0) {
        fsync(fd_);
        ssize_t n = ::pread(fd_, buf_, sizeof(buf_)-1, 0);

        if (n >= 0) {
            if (size) {
                *size = static_cast<int>(n);
            }

            buf_[n] = '\0';
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

    if (fd_ >= 0) {
        content->clear();

        if (fileSize) {
            struct stat statbuf;

            if (::fstat(fd_, &statbuf) == 0) {
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
            size_t toRead = std::min((size_t)maxSize - content->size(), sizeof(buf_));
            ssize_t n = ::read(fd_, buf_, toRead);

            if (n > 0) {
                content->append(buf_, n);
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
