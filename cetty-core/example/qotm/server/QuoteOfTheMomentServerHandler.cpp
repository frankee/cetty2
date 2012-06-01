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

#include "QuoteOfTheMomentServerHandler.h"

// Quotes from Mohandas K. Gandhi:
static const char* QUOTES[] = {
    "Where there is love there is life.",
    "First they ignore you, then they laugh at you, then they fight you, then you win.",
    "Be the change you want to see in the world.",
    "The weak can never forgive. Forgiveness is the attribute of the strong.",
};

static const int QUOTES_CNT = sizeof(QUOTES) / sizeof(const char*);

const char* QuoteOfTheMomentServerHandler::nextQuote() {
    int quoteId;

    {
        boost::lock_guard<boost::mutex> lock(mutex);
        quoteId = random.nextInt(QUOTES_CNT);
    }

    return QUOTES[quoteId];
}

cetty::util::Random QuoteOfTheMomentServerHandler::random;
boost::mutex QuoteOfTheMomentServerHandler::mutex;
