#include <cetty/protobuf/test/NullChannelHandlerContext.h>

NullChannelContext* NullChannelContext::nullChannelContext=NULL;

const ChannelPtr& NullChannelContext::getChannel()const { return ChannelPtr(); }
const ChannelPipelinePtr& NullChannelContext::getPipeline()const {return ChannelPipelinePtr();}
const std::string& NullChannelContext::getName()const {return std::string();}
const ChannelHandlerPtr& NullChannelContext::getHandler()const {return ChannelHandlerPtr();}
const ChannelUpstreamHandlerPtr& NullChannelContext::getUpstreamHandler()const {return ChannelUpstreamHandlerPtr();}
const ChannelDownstreamHandlerPtr&  NullChannelContext::getDownstreamHandler()const {return ChannelDownstreamHandlerPtr();}

bool  NullChannelContext::canHandleUpstream()const {return true;}
bool  NullChannelContext::canHandleDownstream()const {return true;}
void  NullChannelContext::sendUpstream(const ChannelEvent& e) {return;}
void  NullChannelContext::sendUpstream(const MessageEvent& e) {return;}
void  NullChannelContext::sendUpstream(const ChannelStateEvent& e) {return;}
void  NullChannelContext::sendUpstream(const ChildChannelStateEvent& e) {return;}
void  NullChannelContext::sendUpstream(const WriteCompletionEvent& e) {return;}
void  NullChannelContext::sendUpstream(const ExceptionEvent& e) {return;}
void  NullChannelContext::sendDownstream(const ChannelEvent& e) {return;}
void  NullChannelContext::sendDownstream(const MessageEvent& e) {return;}
void  NullChannelContext::sendDownstream(const ChannelStateEvent& e) {return;}
void* NullChannelContext::getAttachment() {return NULL;}
const void* NullChannelContext::getAttachment()const {return NULL;}
void  NullChannelContext::setAttachment(void* attachment) {return;}
