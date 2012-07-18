#if !defined(CETTY_CHANNEL_CHANNELEXCEPTION_H)
#define CETTY_CHANNEL_CHANNELEXCEPTION_H

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

#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

/**
 * A {@link RuntimeException} which is thrown when an I/O operation fails.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.hidden
 */

CETTY_DECLARE_EXCEPTION(ChannelException, RuntimeException)

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELEXCEPTION_H)

// Local Variables:
// mode: c++
// End:
