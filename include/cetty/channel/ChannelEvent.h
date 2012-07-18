#if !defined(CETTY_CHANNEL_CHANNELEVENT_H)
#define CETTY_CHANNEL_CHANNELEVENT_H

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

#include <cetty/channel/ChannelFwd.h>
#include <cetty/channel/ChannelFutureFwd.h>

namespace cetty {
namespace channel {

/**
 * An I/O event or I/O request associated with a {@link Channel}.
 * <p>
 * A {@link ChannelEvent} is handled by a series of {@link ChannelHandler}s in
 * a {@link ChannelPipeline}.
 *
 * <h3>Upstream events and downstream events, and their interpretation</h3>
 * <p>
 * Every event is either an upstream event or a downstream event.
 * If an event flows forward from the first handler to the last handler in a
 * {@link ChannelPipeline}, we call it an upstream event and say <strong>"an
 * event goes upstream."</strong>  If an event flows backward from the last
 * handler to the first handler in a {@link ChannelPipeline}, we call it a
 * downstream event and say <strong>"an event goes downstream."</strong>
 * (Please refer to the diagram in {@link ChannelPipeline} for more explanation.)
 * <p>
 * When your server receives a message from a client, the event associated with
 * the received message is an upstream event.  When your server sends a message
 * or reply to the client, the event associated with the write request is a
 * downstream event.  The same rule applies for the client side.  If your client
 * sent a request to the server, it means your client triggered a downstream
 * event.  If your client received a response from the server, it means
 * your client will be notified with an upstream event.  Upstream events are
 * often the result of inbound operations such as read from socket,
 * and downstream events are the request for outbound operations such as
 * write bytes to socket, connect to remote SocketAddress, and close socket.
 *
 * <h4>Upstream events</h4>
 *
 * <table border="1" cellspacing="0" cellpadding="6">
 * <tr>
 * <th>Event name</th></th><th>Event type and condition</th><th>Meaning</th>
 * </tr>
 * <tr>
 * <td><tt>"messageReceived"</tt></td>
 * <td>{@link MessageEvent}</td>
 * <td>a message object (e.g. {@link ChannelBuffer}) was received from a remote peer</td>
 * </tr>
 * <tr>
 * <td><tt>"exceptionCaught"</tt></td>
 * <td>{@link ExceptionEvent}</td>
 * <td>an exception was raised by an I/O thread or a {@link ChannelHandler}</td>
 * </tr>
 * <tr>
 * <td><tt>"channelOpen"</tt></td>
 * <td>{@link ChannelStateEvent}<br/>(state = {@link ChannelState#OPEN OPEN}, value = <tt>true</tt>)</td>
 * <td>a {@link Channel} is open, but not bound nor connected</td>
 * </tr>
 * <tr>
 * <td><tt>"channelClosed"</tt></td>
 * <td>{@link ChannelStateEvent}<br/>(state = {@link ChannelState#OPEN OPEN}, value = <tt>false</tt>)</td>
 * <td>a {@link Channel} was closed and all its related resources were released</td>
 * </tr>
 * <tr>
 * <td><tt>"channelBound"</tt></td>
 * <td>{@link ChannelStateEvent}<br/>(state = {@link ChannelState#BOUND BOUND}, value = {@link SocketAddress})</td>
 * <td>a {@link Channel} is open and bound to a local address, but not connected</td>
 * </tr>
 * <tr>
 * <td><tt>"channelUnbound"</tt></td>
 * <td>{@link ChannelStateEvent}<br/>(state = {@link ChannelState#BOUND BOUND}, value = <tt>NULL</tt>)</td>
 * <td>a {@link Channel} was unbound from the current local address</td>
 * </tr>
 * <tr>
 * <td><tt>"channelConnected"</tt></td>
 * <td>{@link ChannelStateEvent}<br/>(state = {@link ChannelState#CONNECTED CONNECTED}, value = {@link SocketAddress})</td>
 * <td>a {@link Channel} is open, bound to a local address, and connected to a remote address</td>
 * </tr>
 * <tr>
 * <td><tt>"writeCompleted"</tt></td>
 * <td>{@link WriteCompletionEvent}</td>
 * <td>something has been written to a remote peer</td>
 * </tr>
 * <tr>
 * <td><tt>"channelDisconnected"</tt></td>
 * <td>{@link ChannelStateEvent}<br/>(state = {@link ChannelState#CONNECTED CONNECTED}, value = <tt>NULL</tt>)</td>
 * <td>a {@link Channel} was disconnected from its remote peer</td>
 * </tr>
 * <tr>
 * <td><tt>"channelInterestChanged"</tt></td>
 * <td>{@link ChannelStateEvent}<br/>(state = {@link ChannelState#INTEREST_OPS INTEREST_OPS}, no value)</td>
 * <td>a {@link Channel}'s {@link Channel#getInterestOps() interestOps} was changed</td>
 * </tr>
 * </table>
 * <p>
 * These two additional event types are used only for a parent channel which
 * can have a child channel (e.g. {@link ServerSocketChannel}).
 * <p>
 * <table border="1" cellspacing="0" cellpadding="6">
 * <tr>
 * <th>Event name</th><th>Event type and condition</th><th>Meaning</th>
 * </tr>
 * <tr>
 * <td><tt>"childChannelOpen"</tt></td>
 * <td>{@link ChildChannelStateEvent}<br/>(<tt>childChannel.isOpen() = true</tt>)</td>
 * <td>a child {@link Channel} was open (e.g. a server channel accepted a connection.)</td>
 * </tr>
 * <tr>
 * <td><tt>"childChannelClosed"</tt></td>
 * <td>{@link ChildChannelStateEvent}<br/>(<tt>childChannel.isOpen() = false</tt>)</td>
 * <td>a child {@link Channel} was closed (e.g. the accepted connection was closed.)</td>
 * </tr>
 * </table>
 *
 * <h4>Downstream events</h4>
 *
 * <table border="1" cellspacing="0" cellpadding="6">
 * <tr>
 * <th>Event name</th><th>Event type and condition</th><th>Meaning</th>
 * </tr>
 * <tr>
 * <td><tt>"write"</tt></td>
 * <td>{@link MessageEvent}</td><td>Send a message to the {@link Channel}.</td>
 * </tr>
 * <tr>
 * <td><tt>"bind"</tt></td>
 * <td>{@link ChannelStateEvent}<br/>(state = {@link ChannelState#BOUND BOUND}, value = {@link SocketAddress})</td>
 * <td>Bind the {@link Channel} to the specified local address.</td>
 * </tr>
 * <tr>
 * <td><tt>"unbind"</tt></td>
 * <td>{@link ChannelStateEvent}<br/>(state = {@link ChannelState#BOUND BOUND}, value = <tt>NULL</tt>)</td>
 * <td>Unbind the {@link Channel} from the current local address.</td>
 * </tr>
 * <tr>
 * <td><tt>"connect"</tt></td>
 * <td>{@link ChannelStateEvent}<br/>(state = {@link ChannelState#CONNECTED CONNECTED}, value = {@link SocketAddress})</td>
 * <td>Connect the {@link Channel} to the specified remote address.</td>
 * </tr>
 * <tr>
 * <td><tt>"disconnect"</tt></td>
 * <td>{@link ChannelStateEvent}<br/>(state = {@link ChannelState#CONNECTED CONNECTED}, value = <tt>NULL</tt>)</td>
 * <td>Disconnect the {@link Channel} from the current remote address.</td>
 * </tr>
 * <tr>
 * <td><tt>"close"</tt></td>
 * <td>{@link ChannelStateEvent}<br/>(state = {@link ChannelState#OPEN OPEN}, value = <tt>false</tt>)</td>
 * <td>Close the {@link Channel}.</td>
 * </tr>
 * </table>
 * <p>
 * Other event types and conditions which were not addressed here will be
 * ignored and discarded.  Please note that there's no <tt>"open"</tt> in the
 * table.  It is because a {@link Channel} is always open when it is created
 * by a {@link ChannelFactory}.
 *
 * <h3>Additional resources worth reading</h3>
 * <p>
 * Please refer to the {@link ChannelHandler} and {@link ChannelPipeline}
 * documentation to find out how an event flows in a pipeline and how to handle
 * the event in your application.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 *
 * @dot
 * strict digraph {
 * node            [fontname=Helvetica]
 * ChannelEvent    [shape=box]
 * ChannelFuture   [shape=box URL="\ref ChannelFuture"]
 * ChannelEvent -> ChannelFuture [arrowtail=diamond]
 * }
 * @enddot
 */

class ChannelEvent {
public:
    virtual ~ChannelEvent() {}

    /**
     * Returns the {@link Channel} which is associated with this event.
     */
    virtual const ChannelPtr& getChannel() const = 0;

    /**
     * Returns the {@link ChannelFuture const ChannelFuturePtr&} which is associated with this event.
     * If this event is an upstream event, this method will always return a
     * {@link SucceededChannelFuture} because the event has occurred already.
     * If this event is a downstream event (i.e. I/O request), the returned
     * future will be notified when the I/O request succeeds or fails.
     */
    virtual const ChannelFuturePtr& getFuture() const = 0;

    /**
     * Constructs a string describing this Event.
     *
     * @return The specified string
     */
    virtual std::string toString() const = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELEVENT_H)

// Local Variables:
// mode: c++
// End:
