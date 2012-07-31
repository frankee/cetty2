#include <NullChannelHandlerContext.h>

NullChannelContext* NullChannelContext::nullChannelContext=NULL;

const ChannelPtr& NullChannelContext::getChannel()const { return ChannelPtr(); }
const ChannelPipelinePtr& NullChannelContext::getPipeline()const {return ChannelPipelinePtr();}
const std::string& NullChannelContext::getName()const {return std::string();}
const ChannelHandlerPtr& NullChannelContext::getHandler()const {return ChannelHandlerPtr();}
const ChannelInboundHandlerPtr& NullChannelContext::getInboundHandler()const {return ChannelInboundHandlerPtr();}
const ChannelOutboundHandlerPtr&  NullChannelContext::getOutboundHandler()const {return ChannelOutboundHandlerPtr();}

bool  NullChannelContext::canHandleInboundMessage()const {return true;}
bool  NullChannelContext::canHandleOutboundMessage()const {return true;}
void  NullChannelContext::sendUpstream(const UserEvent& e) {return;}
void  NullChannelContext::sendUpstream(const MessageEvent& e) {return;}
void  NullChannelContext::sendUpstream(const ChannelStateEvent& e) {return;}
void  NullChannelContext::sendUpstream(const ChildChannelStateEvent& e) {return;}
void  NullChannelContext::sendUpstream(const WriteCompletionEvent& e) {return;}
void  NullChannelContext::sendUpstream(const ExceptionEvent& e) {return;}
void  NullChannelContext::sendDownstream(const UserEvent& e) {return;}
void  NullChannelContext::sendDownstream(const MessageEvent& e) {return;}
void  NullChannelContext::sendDownstream(const ChannelStateEvent& e) {return;}
void* NullChannelContext::getAttachment() {return NULL;}
const void* NullChannelContext::getAttachment()const {return NULL;}
void  NullChannelContext::setAttachment(void* attachment) {return;}
