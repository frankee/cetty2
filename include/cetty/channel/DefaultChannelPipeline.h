#if !defined(CETTY_CHANNEL_DEFAULTCHANNELPIPELINE_H)
#define CETTY_CHANNEL_DEFAULTCHANNELPIPELINE_H

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

#include <map>
#include <string>
#include <boost/thread/recursive_mutex.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelSink.h>
#include <cetty/channel/ChannelPipeline.h>

namespace cetty {
namespace util {
class Exception;
}
}

namespace cetty {
namespace logging {
class InternalLogger;
}
}

namespace cetty {
namespace channel {

using namespace cetty::util;
using namespace cetty::logging;

class LifeCycleAwareChannelHandler;
class Channel;
class ChannelEvent;
class ChannelPipelineException;
class DefaultChannelHandlerContext;

/**
 * The default {@link ChannelPipeline} implementation.  It is recommended
 * to use {@link Channels#pipeline()} to create a new {@link ChannelPipeline}
 * instance rather than calling the constructor directly.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class DefaultChannelPipeline : public ChannelPipeline {
public:
    /**
     * Creates a new empty pipeline.
     */
    DefaultChannelPipeline();

    virtual ~DefaultChannelPipeline() {}

    virtual const ChannelPtr& getChannel() const;
    virtual const ChannelSinkPtr& getSink() const;

    virtual void attach(const ChannelPtr& channel, const ChannelSinkPtr& sink);
    virtual void detach();

    virtual bool isAttached() const;

    virtual void addFirst(const std::string& name, const ChannelHandlerPtr& handler);
    virtual void addLast(const std::string& name, const ChannelHandlerPtr& handler);
    virtual void addBefore(const std::string& baseName, const std::string& name, const ChannelHandlerPtr& handler);
    virtual void addAfter(const std::string& baseName, const std::string& name, const ChannelHandlerPtr& handler);

    virtual void remove(const ChannelHandlerPtr& handler);
    virtual ChannelHandlerPtr remove(const std::string& name);
    virtual ChannelHandlerPtr removeFirst();
    virtual ChannelHandlerPtr removeLast();

    virtual void replace(const ChannelHandlerPtr& oldHandler, const std::string& newName, const ChannelHandlerPtr& newHandler);
    virtual ChannelHandlerPtr replace(const std::string& oldName, const std::string& newName, const ChannelHandlerPtr& newHandler);
    virtual ChannelHandlerPtr getFirst() const;
    virtual ChannelHandlerPtr getLast() const;
    virtual ChannelHandlerPtr get(const std::string& name) const;

    virtual ChannelHandlerContext* getContext(const std::string& name) const;
    virtual ChannelHandlerContext* getContext(const ChannelHandlerPtr& handler) const;

    virtual ChannelHandlers getChannelHandles() const;

    /**
     * Returns the {@link std::string} representation of this pipeline.
     */
    virtual std::string toString() const;

    virtual void sendUpstream(const ChannelEvent& e);
    virtual void sendUpstream(const MessageEvent& e);
    virtual void sendUpstream(const ExceptionEvent& e);
    virtual void sendUpstream(const WriteCompletionEvent& e);
    virtual void sendUpstream(const ChannelStateEvent& e);
    virtual void sendUpstream(const ChildChannelStateEvent& e);

    virtual void sendDownstream(const ChannelEvent& e);
    virtual void sendDownstream(const MessageEvent& e);
    virtual void sendDownstream(const ChannelStateEvent& e);

    virtual boost::any getAttachment(const std::string& name) const;
    virtual void setAttachment(const std::string& name, const boost::any& attachment);

    virtual void notifyHandlerException(const ChannelEvent& evt, const Exception& e);

public:
    static InternalLogger* getLogger() { return logger; }

protected:
    void callBeforeAdd(ChannelHandlerContext* ctx);
    void callAfterAdd(ChannelHandlerContext* ctx);
    void callBeforeRemove(ChannelHandlerContext* ctx);
    void callAfterRemove(ChannelHandlerContext* ctx);

private:
    void init(const std::string& name, const ChannelHandlerPtr& handler);

    void checkDuplicateName(const std::string& name);

    DefaultChannelHandlerContext* getContextNoLock(const std::string& name) const;
    DefaultChannelHandlerContext* getContextNoLock(const ChannelHandlerPtr& handler) const;

    DefaultChannelHandlerContext* getContextOrDie(const std::string& name);
    DefaultChannelHandlerContext* getContextOrDie(const ChannelHandlerPtr& handler);

    ChannelHandlerPtr remove(DefaultChannelHandlerContext* ctx);
    ChannelHandlerPtr replace(DefaultChannelHandlerContext* ctx, const std::string& newName, const ChannelHandlerPtr& newHandler);

    // upstream & downstream list operators.
    void updateUpstreamList();
    void updateDownstreamList();

private:
    static InternalLogger* logger;
    static ChannelSinkPtr discardingSink;

private:
    typedef std::map<std::string, DefaultChannelHandlerContext*> ContextMap;
    typedef std::map<std::string, boost::any> AttachmentMap;

    ContextMap name2ctx;

    ChannelPtr channel;
    ChannelSinkPtr sink;

    mutable boost::recursive_mutex mutex;

    DefaultChannelHandlerContext* head;
    DefaultChannelHandlerContext* tail;

    DefaultChannelHandlerContext* upstreamHead; //< upstream single list.
    DefaultChannelHandlerContext* downstreamHead; //< downstream single list.
    
    AttachmentMap attachments;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_DEFAULTCHANNELPIPELINE_H)

// Local Variables:
// mode: c++
// End:
