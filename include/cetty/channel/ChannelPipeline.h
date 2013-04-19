#if !defined(CETTY_CHANNEL_CHANNELPIPELINE_H)
#define CETTY_CHANNEL_CHANNELPIPELINE_H

/*
 * Copyright 2012 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <map>
#include <vector>
#include <string>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <cetty/channel/ChannelPtr.h>
#include <cetty/channel/ChannelHandlerWrapper.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/buffer/ChannelBufferPtr.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace util {
class Exception;
}
}

namespace cetty {
namespace channel {

using namespace cetty::buffer;
using namespace cetty::util;

class ChannelPipelineException;

/**
 * A list of {@link ChannelHandler}s which handles or intercepts
 * inbound and outbount operations of a {@link Channel}.  {@link ChannelPipeline}
 * implements an advanced form of the
 * <a href="http://java.sun.com/blueprints/corej2eepatterns/Patterns/InterceptingFilter.html">Intercepting
 * Filter</a> pattern to give a user full control over how an event is handled
 * and how the {@link ChannelHandler}s in the pipeline interact with each other.
 *
 * <h3>Creation of a pipeline</h3>
 *
 * For each new channel, a new pipeline iscreated and attached to the
 * channel.  Once attached, the coupling between the channel and the pipeline
 * is permanent; the channel cannot attach another pipeline to it nor detach
 * the current pipeline from it. All of this is handled for you and you not need
 * to take care of this.
 * <p>
 *
 *
 * <h3>How an event flows in a pipeline</h3>
 *
 * The following diagram describes how I/O is processed by
 * {@link ChannelHandler}s in a {@link ChannelPipeline} typically.
 * A I/O-operation can be handled by either a {@link ChannelInboundHandler}
 * or a {@link ChannelOutboundHandler} and be forwarded to the closest
 * handler by calling either one of the methods defined in the
 * {@link ChannelInboundInvoker} interface for inbound I/O or by one
 * of the methods defined in the {@link ChannelOutboundInvoker} interface
 * for outbound I/O. {@link ChannelPipeline} extends both of them.
 *
 * <pre>
 *                                                  I/O Request
 *                                             via {@link Channel} or
 *                                         {@link ChannelHandlerContext}
 *                                                       |
 *  +----------------------------------------------------+-----------------+
 *  |                           ChannelPipeline          |                 |
 *  |                                                   \|/                |
 *  |    +----------------------+            +-----------+------------+    |
 *  |    | Inbound Handler  N   |            | Outbound Handler  1    |    |
 *  |    +----------+-----------+            +-----------+------------+    |
 *  |              /|\                                   |                 |
 *  |               |                                   \|/                |
 *  |    +----------+-----------+            +-----------+------------+    |
 *  |    | Inbound Handler N-1  |            |   Outbound Handler  2  |    |
 *  |    +----------+-----------+            +-----------+------------+    |
 *  |              /|\                                   .                 |
 *  |               .                                    .                 |
 *  | [{@link ChannelInboundInvoker}]   [{@link ChannelOutboundInvoker}()] |
 *  |        [ method call]                    [method call]               |
 *  |               .                                    .                 |
 *  |               .                                   \|/                |
 *  |    +----------+-----------+            +-----------+------------+    |
 *  |    | Inbound Handler  2   |            | Outbound Handler M-1   |    |
 *  |    +----------+-----------+            +-----------+------------+    |
 *  |              /|\                                   |                 |
 *  |               |                                   \|/                |
 *  |    +----------+-----------+            +-----------+------------+    |
 *  |    | Inbound Handler  1   |            | Outbound Handler  M    |    |
 *  |    +----------+-----------+            +-----------+------------+    |
 *  |              /|\                                   |                 |
 *  +---------------+------------------------------------+-----------------+
 *                  |                                   \|/
 *  +---------------+------------------------------------+-----------------+
 *  |               |                                    |                 |
 *  |       [ Socket.read() ]                     [ Socket.write() ]       |
 *  |                                                                      |
 *  |  Netty Internal I/O Threads (Transport Implementation)               |
 *  +----------------------------------------------------------------------+
 * </pre>
 * An inbound event is handled by the inbound handlers in the bottom-up
 * direction as shown on the left side of the diagram.  An inbound handler
 * usually handles the inbound data generated by the I/O thread on the bottom
 * of the diagram.  The inbound data is often read from a remote peer via the
 * actual input operation such as {@link InputStream#read(byte[])}.
 * If an inbound event goes beyond the top inbound handler, it is discarded
 * silently.
 * <p>
 * A outbound event is handled by the outbound handler in the top-down
 * direction as shown on the right side of the diagram.  A outbound handler
 * usually generates or transforms the outbound traffic such as write requests.
 * If a outbound event goes beyond the bottom outbound handler, it is
 * handled by an I/O thread associated with the {@link Channel}. The I/O thread
 * often performs the actual output operation such as {@link OutputStream#write(byte[])}.
 * <p>
 * For example, let us assume that we created the following pipeline:
 * <pre>
 * {@link ChannelPipeline} p = ...;
 * p.addLast("1", new InboundHandlerA());
 * p.addLast("2", new InboundHandlerB());
 * p.addLast("3", new OutboundHandlerA());
 * p.addLast("4", new OutboundHandlerB());
 * p.addLast("5", new InboundOutboundHandlerX());
 * </pre>
 * In the example above, the class whose name starts with {@code Inbound} means
 * it is an inbound handler.  The class whose name starts with
 * {@code Outbound} means it is a outbound handler.
 * <p>
 * In the given example configuration, the handler evaluation order is 1, 2, 3,
 * 4, 5 when an event goes inbound.  When an event goes outbound, the order
 * is 5, 4, 3, 2, 1.  On top of this principle, {@link ChannelPipeline} skips
 * the evaluation of certain handlers to shorten the stack depth:
 * <ul>
 * <li>3 and 4 don't implement {@link ChannelInboundHandler}, and therefore the
 *     actual evaluation order of an inbound event will be: 1, 2, and 5.</li>
 * <li>1, 2, and 5 don't implement {@link ChannelOutboundHandler}, and
 *     therefore the actual evaluation order of a outbound event will be:
 *     4 and 3.</li>
 * <li>If 5 implements both
 *     {@link ChannelInboundHandler} and {@link ChannelOutboundHandler}, the
 *     evaluation order of an inbound and a outbound event could be 125 and
 *     543 respectively.</li>
 * </ul>
 *
 * <h3>Building a pipeline</h3>
 * <p>
 * A user is supposed to have one or more {@link ChannelHandler}s in a
 * pipeline to receive I/O events (e.g. read) and to request I/O operations
 * (e.g. write and close).  For example, a typical server will have the following
 * handlers in each channel's pipeline, but your mileage may vary depending on
 * the complexity and characteristics of the protocol and business logic:
 *
 * <ol>
 * <li>Protocol Decoder - translates binary data (e.g. {@link ByteBuf})
 *                        into a Java object.</li>
 * <li>Protocol Encoder - translates a Java object into binary data.</li>
 * <li><tt>ExecutionHandler</tt> - applies a thread model.</li>
 * <li>Business Logic Handler - performs the actual business logic
 *                              (e.g. database access).</li>
 * </ol>
 *
 * and it could be represented as shown in the following example:
 *
 * <pre>
 * {@link ChannelPipeline} pipeline = ...;
 * pipeline.addLast("decoder", new MyProtocolDecoder());
 * pipeline.addLast("encoder", new MyProtocolEncoder());
 * pipeline.addLast("executor", new ExecutionHandler(...));
 * pipeline.addLast("handler", new MyBusinessLogicHandler());
 * </pre>
 *
 * <h3>Thread safety</h3>
 * <p>
 * A {@link ChannelHandler} can be added or removed at any time because a
 * {@link ChannelPipeline} is thread safe.  For example, you can insert an
 * encryption handler when sensitive information is about to be exchanged,
 * and remove it after the exchange.
 */
class ChannelPipeline : private boost::noncopyable {
public:
    ChannelPipeline(const ChannelPtr& channel);
    ~ChannelPipeline();

    /**
     * Returns the {@link Channel} that this pipeline is attached to.
     *
     * @return the channel.
     */
    ChannelPtr channel() const;

    /**
     * Returns the {@link EventLoopPtr} that this pipeline is attached to.
     *
     * @return the EventLoopPtr, which is the same as the Channel's.
     */
    const EventLoopPtr& eventLoop() const;

public:
    /**
     * Inserts a {@link ChannelHandler} at the first position of this pipeline.
     *
     * @param name     the name of the handler to insert first
     * @param handler  the handler to insert first
     *
     * @throws InvalidArgumentException
     *         if there's an entry with the same name already in the pipeline
     * @throws NullPointerException
     *         if the specified name or handler is <tt>NULL</tt>
     */
    bool addFirst(ChannelHandlerContext* context);

    template<typename T>
    bool addFirst(const std::string& name,
                  const typename ChannelHandlerWrapper<T>::HandlerPtr& handler) {
        if (name.empty() || !handler) {
            return false;
        }

        return addFirst(
                   new typename ChannelHandlerWrapper<T>::Handler::Context(name, handler));
    }

    template<typename T>
    bool addFirst(const std::string& name,
                  const EventLoopPtr& loop,
                  const typename ChannelHandlerWrapper<T>::HandlerPtr& handler) {
        if (name.empty() || !handler) {
            return false;
        }

        return addFirst(
                   new typename ChannelHandlerWrapper<T>::Handler::Context(name, loop, handler));
    }

    /**
     * Appends a {@link ChannelHandler} at the last position of this pipeline.
     *
     * @param name     the name of the handler to append
     * @param handler  the handler to append
     *
     * @throws InvalidArgumentException
     *         if there's an entry with the same name already in the pipeline
     * @throws NullPointerException
     *         if the specified name or handler is <tt>NULL</tt>
     */
    bool addLast(ChannelHandlerContext* context);

    template<typename T>
    bool addLast(const std::string& name,
                 const typename ChannelHandlerWrapper<T>::HandlerPtr& handler) {
        if (name.empty() || !handler) {
            return false;
        }

        return addLast(
                   new typename ChannelHandlerWrapper<T>::Handler::Context(name, handler));
    }

    template<typename T>
    bool addLast(const std::string& name,
                 const EventLoopPtr& loop,
                 const typename ChannelHandlerWrapper<T>::HandlerPtr& handler) {
        if (name.empty() || !handler) {
            return false;
        }

        return addLast(
                   new typename ChannelHandlerWrapper<T>::Handler::Context(name, loop, handler));
    }

    /**
     * Inserts a {@link ChannelHandler} before an existing handler of this
     * pipeline.
     *
     * @param baseName  the name of the existing handler
     * @param name      the name of the handler to insert before
     * @param handler   the handler to insert before
     *
     * @throws NoSuchElementException
     *         if there's no such entry with the specified <tt>baseName</tt>
     * @throws InvalidArgumentException
     *         if there's an entry with the same name already in the pipeline
     * @throws NullPointerException
     *         if the specified baseName, name, or handler is <tt>NULL</tt>
     */
    bool addBefore(const std::string& name, ChannelHandlerContext* context);

    template<typename T>
    bool addBefore(const std::string& baseName,
                   const std::string& name,
                   const typename ChannelHandlerWrapper<T>::HandlerPtr& handler) {
        if (name.empty() || !handler) {
            return false;
        }

        return addBefore(baseName,
                         new typename ChannelHandlerWrapper<T>::Handler::Context(name, handler));
    }

    template<typename T>
    bool addBefore(const std::string& baseName,
                   const std::string& name,
                   const EventLoopPtr& loop,
                   const typename ChannelHandlerWrapper<T>::HandlerPtr& handler) {
        if (name.empty() || !handler) {
            return false;
        }

        return addBefore(baseName,
                         new typename ChannelHandlerWrapper<T>::Handler::Context(name, loop, handler));
    }

    /**
     * Inserts a {@link ChannelHandler} after an existing handler of this
     * pipeline.
     *
     * @param baseName  the name of the existing handler
     * @param name      the name of the handler to insert after
     * @param handler   the handler to insert after
     *
     * @throws NoSuchElementException
     *         if there's no such entry with the specified <tt>baseName</tt>
     * @throws InvalidArgumentException
     *         if there's an entry with the same name already in the pipeline
     * @throws NullPointerException
     *         if the specified baseName, name, or handler is <tt>NULL</tt>
     */
    bool addAfter(const std::string& name, ChannelHandlerContext* context);

    template<typename T>
    bool addAfter(const std::string& baseName,
                  const std::string& name,
                  const typename ChannelHandlerWrapper<T>::HandlerPtr& handler) {
        return addAfter(baseName,
                        new typename ChannelHandlerWrapper<T>::Handler::Context(name, handler));
    }

    template<typename T>
    bool addAfter(const std::string& baseName,
                  const std::string& name,
                  const EventLoopPtr& loop,
                  const typename ChannelHandlerWrapper<T>::HandlerPtr& handler) {
        return addAfter(baseName,
                        new typename ChannelHandlerWrapper<T>::Handler::Context(name, loop, handler));
    }

    /**
     * Removes the {@link ChannelHandler} with the specified name from this
     * pipeline.
     *
     * @return the removed handler
     *
     * @throws NoSuchElementException
     *         if there's no such handler with the specified name in this pipeline
     * @throws NullPointerException
     *         if the specified name is <tt>NULL</tt>
     */
    void remove(const std::string& name);

    /**
     * Removes the specified {@link ChannelHandler} from this pipeline
     * and transfer the content of its {@link Buf} to the next
     * {@link ChannelHandler} in the {@link ChannelPipeline}.
     *
     * @param  handler          the {@link ChannelHandler} to remove
     *
     * @throws NoSuchElementException
     *         if there's no such handler in this pipeline
     * @throws NullPointerException
     *         if the specified handler is {@code null}
     */
    void removeAndForward(const std::string& name);

    /**
     * Removes the first {@link ChannelHandler} in this pipeline.
     *
     * All the remaining content in the {@link Buf) (if any) of the {@link ChannelHandler}
     * will be discarded.
     *
     * @return the removed handler
     *
     * @throws NoSuchElementException
     *         if this pipeline is empty
     */
    void removeFirst();

    /**
     * Removes the last {@link ChannelHandler} in this pipeline.
     *
     * All the remaining content in the {@link Buf) (if any) of the {@link ChannelHandler}
     * will be discarded.
     *
     * @return the removed handler
     *
     * @throws NoSuchElementException
     *         if this pipeline is empty
     */
    void removeLast();

    /**
     * Replaces the specified {@link ChannelHandler} with a new handler in
     * this pipeline.
     *
     * All the remaining content in the {@link Buf) (if any) of the {@link ChannelHandler}
     * will be discarded.
     *
     * @param  oldHandler   the {@link ChannelHandler} to be replaced
     * @param  newName      the name under which the replacement should be added
     * @param  newHandler   the {@link ChannelHandler} which is used as replacement
     *
     * @return itself
     *
     * @throws NoSuchElementException
     *         if the specified old handler does not exist in this pipeline
     * @throws IllegalArgumentException
     *         if a handler with the specified new name already exists in this
     *         pipeline, except for the handler to be replaced
     * @throws NullPointerException
     *         if the specified old handler, new name, or new handler is
     *         {@code null}
     */
    bool replace(const std::string& name, ChannelHandlerContext* context);

    template<typename T>
    bool replace(const std::string& baseName,
                 const std::string& name,
                 const typename ChannelHandlerWrapper<T>::HandlerPtr& handler) {
        return replace(baseName,
                       new typename ChannelHandlerWrapper<T>::Handler::Context(name, handler));
    }

    template<typename T>
    bool replace(const std::string& baseName,
                 const std::string& name,
                 const EventLoopPtr& loop,
                 const typename ChannelHandlerWrapper<T>::HandlerPtr& handler) {
        return replace(baseName,
                       new typename ChannelHandlerWrapper<T>::Handler::Context(name, loop, handler));
    }

    /**
     * Replaces the specified {@link ChannelHandler} with a new handler in
     * this pipeline and transfer the content of its {@link Buf} to the next
     * {@link ChannelHandler} in the {@link ChannelPipeline}.
     *
     * @param  oldHandler    the {@link ChannelHandler} to be replaced
     * @param  newName       the name under which the replacement should be added
     * @param  newHandler    the {@link ChannelHandler} which is used as replacement
     *
     * @return itself

     * @throws NoSuchElementException
     *         if the specified old handler does not exist in this pipeline
     * @throws IllegalArgumentException
     *         if a handler with the specified new name already exists in this
     *         pipeline, except for the handler to be replaced
     * @throws NullPointerException
     *         if the specified old handler, new name, or new handler is
     *         {@code null}
     */
    void replaceAndForward(const std::string& baseName,
        const std::string& newName,
        ChannelHandlerContext* context);

    /**
     * Returns the context of the first {@link ChannelHandler} in this pipeline.
     *
     * @return the context of the first handler.  {@code null} if this pipeline is empty.
     */
    ChannelHandlerContext* first() const;

    ChannelHandlerContext* head() const {
        return head_;
    }

    /**
     * Returns the context of the last {@link ChannelHandler} in this pipeline.
     *
     * @return the context of the last handler.  {@code null} if this pipeline is empty.
     */
    ChannelHandlerContext* last() const;

    ChannelHandlerContext* tail() const {
        return tail_;
    }

    /**
     * Returns the context object of the {@link ChannelHandler} with the
     * specified name in this pipeline.
     *
     * @return the context object of the handler with the specified name.
     *         {@code null} if there's no such handler in this pipeline.
     */
    ChannelHandlerContext* find(const std::string& name) const;

    void setHead(ChannelHandlerContext* ctx);

    template<typename T>
    void setHead(const std::string& name,
                 const typename ChannelHandlerWrapper<T>::HandlerPtr& handler) {
        return setHead(
                   new typename ChannelHandlerWrapper<T>::Handler::Context(name, handler));
    }

public:
    ChannelPipeline& fireChannelOpen();
    ChannelPipeline& fireChannelActive();
    ChannelPipeline& fireChannelInactive();

    ChannelPipeline& fireMessageUpdated();
    ChannelPipeline& fireChannelReadSuspended();

    ChannelPipeline& fireUserEventTriggered(const boost::any& evt);
    ChannelPipeline& fireExceptionCaught(const ChannelException& cause);

    void read();

    ChannelFuturePtr bind(const InetAddress& localAddress);
    ChannelFuturePtr connect(const InetAddress& remoteAddress);
    ChannelFuturePtr connect(const InetAddress& remoteAddress,
                             const InetAddress& localAddress);
    ChannelFuturePtr disconnect();
    ChannelFuturePtr close();
    ChannelFuturePtr flush();

    const ChannelFuturePtr& bind(const InetAddress& localAddress,
                                 const ChannelFuturePtr& future);
    const ChannelFuturePtr& connect(const InetAddress& remoteAddress,
                                    const ChannelFuturePtr& future);
    const ChannelFuturePtr& connect(const InetAddress& remoteAddress,
                                    const InetAddress& localAddress,
                                    const ChannelFuturePtr& future);
    const ChannelFuturePtr& disconnect(const ChannelFuturePtr& future);
    const ChannelFuturePtr& close(const ChannelFuturePtr& future);
    const ChannelFuturePtr& flush(const ChannelFuturePtr& future);

#if (DATA_PROCESS_BOCK || 1)

    /**
     * Return the bound {@link MessageBuf} of the first {@link ChannelInboundMessageHandler} in the
     * {@link ChannelPipeline}. If no {@link ChannelInboundMessageHandler} exists in the {@link ChannelPipeline}
     * it will throw a {@link UnsupportedOperationException}.
     * <p/>
     * This method can only be called from within the event-loop, otherwise it will throw an
     * {@link IllegalStateException}.
     */
    template<class T, int MessageT>
    ChannelMessageContainer<T, MessageT>* inboundMessageContainer() {
        if (head_) {
            return head_->nextInboundMessageContainer<ChannelMessageContainer<T, MessageT> >(head_);
        }

        return NULL;
    }

        /**
     * Return the bound {@link ByteBuf} of the first {@link ChannelInboundByteHandler} in the
     * {@link ChannelPipeline}. If no {@link ChannelInboundByteHandler} exists in the {@link ChannelPipeline}
     * it will throw a {@link UnsupportedOperationException}.
     * <p/>
     * This method can only be called from within the event-loop, otherwise it will throw an
     * {@link IllegalStateException}.
     */
    template<class T>
    T* inboundMessageContainer() {
        if (head_) {
            return head_->nextInboundMessageContainer<T>(head_);
        }

        return NULL;
    }

    /**
     * Return the bound {@link MessageBuf} of the first {@link ChannelOutboundMessageHandler} in the
     * {@link ChannelPipeline}. If no {@link ChannelOutboundMessageHandler} exists in the {@link ChannelPipeline}
     * it will throw a {@link UnsupportedOperationException}.
     * <p/>
     * This method can only be called from within the event-loop, otherwise it will throw an
     * {@link IllegalStateException}.
     */
    template<class T, int MessageT>
    ChannelMessageContainer<T, MessageT>* outboundMessageContainer() {
        if (tail_) {
            return tail_->nextOutboundMessageContainer<ChannelMessageContainer<T, MessageT> >(tail_);
        }

        return NULL;
    }

    /**
     * Return the bound {@link MessageBuf} of the first {@link ChannelOutboundMessageHandler} in the
     * {@link ChannelPipeline}. If no {@link ChannelOutboundMessageHandler} exists in the {@link ChannelPipeline}
     * it will throw a {@link UnsupportedOperationException}.
     * <p/>
     * This method can only be called from within the event-loop, otherwise it will throw an
     * {@link IllegalStateException}.
     */
    template<class T>
    T* outboundMessageContainer() {
        if (tail_) {
            return tail_->nextOutboundMessageContainer<T>(tail_);
        }

        return NULL;
    }

    template<typename T, int MessageT>
    bool addInboundMessage(const T& message) {
        if (!!message) {
            ChannelMessageContainer<T, MessageT>* container =
                inboundMessageContainer<T, MessageT>();

            if (container) {
                container->addMessage(message);
                return true;
            }
            else {
                LOG_WARN << "has no inboundMessageContainer in pipeline, "
                         "you will lost the message.";
            }
        }
        else {
            LOG_WARN << "you add an empty (invalid) message to pipeline inbound";
        }

        return false;
    }

    template<typename T, int MessageT>
    bool addOutboundMessage(const T& message) {
        if (!!message) {
            ChannelMessageContainer<T, MessageT>* container =
                outboundMessageContainer<T, MessageT>();

            if (container) {
                container->addMessage(message);
                return true;
            }
            else {
                LOG_WARN << "has no outboundMessageContainer in pipeline, "
                         "you will lost the message.";
            }
        }
        else {
            LOG_WARN << "you add an empty (invalid) message to pipeline outbound";
        }

        return false;
    }

    template<typename T>
    bool addInboundChannelMessage(const T& message) {
        return addInboundMessage<T, MESSAGE_BLOCK>(message);
    }

    template<typename T>
    bool addOutboundChannelMessage(const T& message) {
        return addOutboundMessage<T, MESSAGE_BLOCK>(message);
    }

    bool addInboundChannelBuffer(const ChannelBufferPtr& buffer) {
        return addInboundMessage<ChannelBufferPtr, MESSAGE_STREAM>(buffer);
    }

    bool addOutboundChannelBuffer(const ChannelBufferPtr& buffer) {
        return addOutboundMessage<ChannelBufferPtr, MESSAGE_STREAM>(buffer);
    }

    /**
     * Request to write a message via this ChannelOutboundInvoker and notify the {@link ChannelFuture}
     * once the operation completes, either because the operation was successful or because of an error.
     *
     * If you want to write a {@link FileRegion} use {@link #sendFile(FileRegion)}.
     * <p>
     * Be aware that the write could be only partially successful as the message may need to get encoded before write it
     * to the remote peer. In such cases the {@link ChannelFuture} will be failed with a
     * {@link IncompleteFlushException}. In such cases you may want to call {@link #flush(ChannelPromise)} or
     * {@link #flush()} to flush the rest of the data or just close the connection via {@link #close(ChannelPromise)}
     * or {@link #close()} if it is not possible to recover.
     *
     * The given {@link ChannelPromise} will be notified.
     * <p>
     * This will result in having the message added to the outbound buffer of the next {@link ChannelOutboundHandler}
     * and the {@link ChannelOperationHandler#flush(ChannelHandlerContext, ChannelPromise)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    template<typename T, int MessageT>
    ChannelFuturePtr write(const T& message) {
        ChannelFuturePtr f = newFuture();
        return write<T, MessageT>(message, f);
    }

    /**
     * Request to write a message via this ChannelOutboundInvoker and notify the {@link ChannelFuture}
     * once the operation completes, either because the operation was successful or because of an error.
     *
     * If you want to write a {@link FileRegion} use {@link #sendFile(FileRegion)}.
     * <p>
     * Be aware that the write could be only partially successful as the message may need to get encoded before write it
     * to the remote peer. In such cases the {@link ChannelFuture} will be failed with a
     * {@link IncompleteFlushException}. In such cases you may want to call {@link #flush(ChannelPromise)} or
     * {@link #flush()} to flush the rest of the data or just close the connection via {@link #close(ChannelPromise)}
     * or {@link #close()} if it is not possible to recover.
     *
     * The given {@link ChannelPromise} will be notified.
     * <p>
     * This will result in having the message added to the outbound buffer of the next {@link ChannelOutboundHandler}
     * and the {@link ChannelOperationHandler#flush(ChannelHandlerContext, ChannelPromise)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    template<typename T, int MessageT>
    const ChannelFuturePtr& write(const T& message, const ChannelFuturePtr& future) {
        if (addOutboundMessage<T, MessageT>(message)) {
            tail_->flush(*tail_, future);
        }
        else {
            future->setFailure(ChannelException("has no outbound message container for that type."));
        }

        return future;
    }

    template<typename T>
    ChannelFuturePtr writeMessage(const T& message) {
        return write<T, MESSAGE_BLOCK>(message);
    }

    template<typename T>
    const ChannelFuturePtr& writeMessage(const T& message,
                                         const ChannelFuturePtr& future) {
        return write<T, MESSAGE_BLOCK>(message, future);
    }

    ChannelFuturePtr writeBuffer(const ChannelBufferPtr& buffer) {
        return write<ChannelBufferPtr, MESSAGE_STREAM>(buffer);
    }

    const ChannelFuturePtr& writeBuffer(const ChannelBufferPtr& buffer,
                                        const ChannelFuturePtr& future) {
        return write<ChannelBufferPtr, MESSAGE_STREAM>(buffer, future);
    }

#endif

public:
    void notifyHandlerException(const Exception& e);

    ChannelFuturePtr newFuture();
    const ChannelFuturePtr& voidFuture() const;

    /**
     * Returns the {@link std::string} representation of this pipeline.
     */
    std::string toString() const;

protected:
    bool callBeforeAdd(ChannelHandlerContext* ctx);
    bool callAfterAdd(ChannelHandlerContext* ctx);
    bool callBeforeRemove(ChannelHandlerContext* ctx);
    bool callAfterRemove(ChannelHandlerContext* ctx);

private:
    bool duplicated(const std::string& name) {
        return contexts_.find(name) != contexts_.end();
    }

    void firePipelineChanged() {

    }

private:
    typedef std::map<std::string, ChannelHandlerContext*> ContextMap;

private:
    bool firedChannelActive_;
    bool fireMessageUpdatedOnActivation_;

    ContextMap contexts_;

    ChannelWeakPtr channel_;
    EventLoopPtr eventLoop_;
    ChannelFuturePtr voidFuture_;

    ChannelHandlerContext* head_;
    ChannelHandlerContext* tail_;
};

inline
ChannelPtr ChannelPipeline::channel() const {
    return channel_.lock();
}

inline
const EventLoopPtr& ChannelPipeline::eventLoop() const {
    return eventLoop_;
}

inline
const ChannelFuturePtr& ChannelPipeline::voidFuture() const {
    return voidFuture_;
}

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELPIPELINE_H)

// Local Variables:
// mode: c++
// End:
