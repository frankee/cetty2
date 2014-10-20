#include <stdlib.h>
#include <cetty/util/SecureRandom.h>

namespace cetty {
namespace util {

#if CETTY_OS_FAMILY_WINDOWS

SecureRandom::SecureRandom() {}
SecureRandom::~SecureRandom() {}

int64_t SecureRandom::nextInt64() {
    uint32_t a, b;

    a = rand();
    b = rand();
//     if (rand_s(&a)) {
//         abort();
//     }
// 
//     if (rand_s(&b)) {
//         abort();
//     }

    return (static_cast<int64_t>(a) << 32) | b;
}

#elif (CETTY_OS == CETTY_OS_MAC_OS_X || CETTY_OS == CETTY_OS_LINUX)

SecureRandom::SecureRandom()
    : stream_("/dev/urandom", std::ios::binary | std::ios::in) {
    if (!stream_.is_open()) {
        //std::cerr << "can't open " << fn << " " << strerror(errno) << std::endl;
        abort();
    }
}

SecureRandom::~SecureRandom() {
}

int64_t SecureRandom::nextInt64() {
    int64_t r;
    stream_.read(reinterpret_cast<char*>(&r), sizeof(r));

    if (stream_.fail()) {
        abort();
    }

    return r;
}

#else

SecureRandom() {
    srandomdev();
}

int64_t nextInt64() {
    long a, b;
    a = random();
    b = random();
    return (static_cast<int64_t>(a) << 32) | b;
}

#endif

}
}
