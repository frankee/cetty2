#if !defined(CETTY_TYPES_H)
#define CETTY_TYPES_H

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

#include <boost/cstdint.hpp>
#include <cetty/Platform.h>

namespace cetty {

using boost::int8_t;
using boost::uint8_t;

using boost::int16_t;
using boost::uint16_t;

using boost::int32_t;
using boost::uint32_t;

using boost::int64_t;
using boost::uint64_t;

using boost::intmax_t;
using boost::uintmax_t;

// long long macros to be used because gcc and vc++ use different suffixes,
// and different size specifiers in format strings
#undef GOOGLE_LONGLONG
#undef GOOGLE_ULONGLONG
#undef GOOGLE_LL_FORMAT

#ifdef _MSC_VER
#define GOOGLE_LONGLONG(x) x##I64
#define GOOGLE_ULONGLONG(x) x##UI64
#define GOOGLE_LL_FORMAT "I64"  // As in printf("%I64d", ...)
#else
#define GOOGLE_LONGLONG(x) x##LL
#define GOOGLE_ULONGLONG(x) x##ULL
#define GOOGLE_LL_FORMAT "ll"  // As in "%lld". Note that "q" is poor form also.
#endif

static const int32_t MAX_INT32 = 0x7FFFFFFF;
static const int32_t MIN_INT32 = -MAX_INT32 - 1;
static const uint32_t MAX_UINT32 = 0xFFFFFFFFu;

static const int64_t MAX_INT64 = GOOGLE_LONGLONG(0x7FFFFFFFFFFFFFFF);
static const int64_t MIN_INT64 = -MAX_INT64 - 1;
static const uint64_t MAX_UINT64 = GOOGLE_ULONGLONG(0xFFFFFFFFFFFFFFFF);


#define REFERENCED(x) void(x)

}

#endif //#if !defined(CETTY_TYPES_H)

// Local Variables:
// mode: c++
// End:
