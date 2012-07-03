// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: echo.proto

#ifndef PROTOBUF_echo_2eproto__INCLUDED
#define PROTOBUF_echo_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
#include <cetty/util/BarePointer.h>
#include <cetty/protobuf/service/ProtobufService.h>
#include <cetty/protobuf/service/ProtobufServiceFuture.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/ProtobufClientServiceAdaptor.h>
// @@protoc_insertion_point(includes)

namespace echo {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_echo_2eproto();
void protobuf_AssignDesc_echo_2eproto();
void protobuf_ShutdownFile_echo_2eproto();

class EchoRequest;
class EchoResponse;

// ===================================================================

class EchoRequest : public ::google::protobuf::Message {
 public:
  EchoRequest();
  virtual ~EchoRequest();
  
  EchoRequest(const EchoRequest& from);
  
  inline EchoRequest& operator=(const EchoRequest& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const EchoRequest& default_instance();
  
  void Swap(EchoRequest* other);
  
  // implements Message ----------------------------------------------
  
  EchoRequest* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const EchoRequest& from);
  void MergeFrom(const EchoRequest& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required string payload = 1;
  inline bool has_payload() const;
  inline void clear_payload();
  static const int kPayloadFieldNumber = 1;
  inline const ::std::string& payload() const;
  inline void set_payload(const ::std::string& value);
  inline void set_payload(const char* value);
  inline void set_payload(const char* value, size_t size);
  inline ::std::string* mutable_payload();
  inline ::std::string* release_payload();
  
  // @@protoc_insertion_point(class_scope:echo.EchoRequest)
 private:
  inline void set_has_payload();
  inline void clear_has_payload();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* payload_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_echo_2eproto();
  friend void protobuf_AssignDesc_echo_2eproto();
  friend void protobuf_ShutdownFile_echo_2eproto();
  
  void InitAsDefaultInstance();
  static EchoRequest* default_instance_;
};
// -------------------------------------------------------------------

class EchoResponse : public ::google::protobuf::Message {
 public:
  EchoResponse();
  virtual ~EchoResponse();
  
  EchoResponse(const EchoResponse& from);
  
  inline EchoResponse& operator=(const EchoResponse& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const EchoResponse& default_instance();
  
  void Swap(EchoResponse* other);
  
  // implements Message ----------------------------------------------
  
  EchoResponse* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const EchoResponse& from);
  void MergeFrom(const EchoResponse& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required string payload = 2;
  inline bool has_payload() const;
  inline void clear_payload();
  static const int kPayloadFieldNumber = 2;
  inline const ::std::string& payload() const;
  inline void set_payload(const ::std::string& value);
  inline void set_payload(const char* value);
  inline void set_payload(const char* value, size_t size);
  inline ::std::string* mutable_payload();
  inline ::std::string* release_payload();
  
  // @@protoc_insertion_point(class_scope:echo.EchoResponse)
 private:
  inline void set_has_payload();
  inline void clear_has_payload();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* payload_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_echo_2eproto();
  friend void protobuf_AssignDesc_echo_2eproto();
  friend void protobuf_ShutdownFile_echo_2eproto();
  
  void InitAsDefaultInstance();
  static EchoResponse* default_instance_;
};
// ===================================================================

class EchoService_Stub;

typedef ::cetty::util::BarePointer< ::echo::EchoRequest const> ConstEchoRequestPtr;
typedef ::cetty::util::BarePointer< ::echo::EchoResponse> EchoResponsePtr;

class EchoService : public ::cetty::protobuf::service::ProtobufService {
 protected:
  // This class should be treated as an abstract interface.
  inline EchoService() {};
 public:
  virtual ~EchoService();
  
  typedef EchoService_Stub Stub;
  
  static const ::google::protobuf::ServiceDescriptor* descriptor();
  
  virtual void Echo(const ConstEchoRequestPtr& request,
                       const EchoResponsePtr& response,
                       const DoneCallback& done);
  
  // implements Service ----------------------------------------------
  
  const ::google::protobuf::ServiceDescriptor* GetDescriptor();
  void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                  const ::cetty::protobuf::service::ConstMessagePtr& request,
                  const ::cetty::protobuf::service::MessagePtr& response,
                  const DoneCallback& done);
  
  const ::google::protobuf::Message* GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const;
  const ::google::protobuf::Message* GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(EchoService);
};

class EchoService_Stub : public EchoService {
 public:
  EchoService_Stub(const ::cetty::protobuf::service::ProtobufClientServicePtr& service);
  ~EchoService_Stub();
  
  inline const ::cetty::protobuf::service::ProtobufClientServicePtr& channel() {
      return channel_.getService();
  }
  
  // implements EchoService ------------------------------------------
  
  //using EchoService::Echo;
  typedef boost::intrusive_ptr< ::cetty::service::ServiceFuture<EchoResponsePtr> > EchoResponseFuturePtr;
  virtual void Echo(const ConstEchoRequestPtr& request,
                       const EchoResponseFuturePtr& future);

  //template<typename RequestT,typename ResponseT>
  //void Echo(const ::cetty::util::BarePointer<RequestT>& request,
//	  const boost::intrusive_ptr< ::cetty::service::ServiceFuture<ResponseT> >& future);

 private:
  cetty::protobuf::service::ProtobufClientServiceAdaptor channel_;
  bool owns_channel_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(EchoService_Stub);
};


// ===================================================================


// ===================================================================

// EchoRequest

// required string payload = 1;
inline bool EchoRequest::has_payload() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void EchoRequest::set_has_payload() {
  _has_bits_[0] |= 0x00000001u;
}
inline void EchoRequest::clear_has_payload() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void EchoRequest::clear_payload() {
  if (payload_ != &::google::protobuf::internal::kEmptyString) {
    payload_->clear();
  }
  clear_has_payload();
}
inline const ::std::string& EchoRequest::payload() const {
  return *payload_;
}
inline void EchoRequest::set_payload(const ::std::string& value) {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::kEmptyString) {
    payload_ = new ::std::string;
  }
  payload_->assign(value);
}
inline void EchoRequest::set_payload(const char* value) {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::kEmptyString) {
    payload_ = new ::std::string;
  }
  payload_->assign(value);
}
inline void EchoRequest::set_payload(const char* value, size_t size) {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::kEmptyString) {
    payload_ = new ::std::string;
  }
  payload_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* EchoRequest::mutable_payload() {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::kEmptyString) {
    payload_ = new ::std::string;
  }
  return payload_;
}
inline ::std::string* EchoRequest::release_payload() {
  clear_has_payload();
  if (payload_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = payload_;
    payload_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// -------------------------------------------------------------------

// EchoResponse

// required string payload = 2;
inline bool EchoResponse::has_payload() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void EchoResponse::set_has_payload() {
  _has_bits_[0] |= 0x00000001u;
}
inline void EchoResponse::clear_has_payload() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void EchoResponse::clear_payload() {
  if (payload_ != &::google::protobuf::internal::kEmptyString) {
    payload_->clear();
  }
  clear_has_payload();
}
inline const ::std::string& EchoResponse::payload() const {
  return *payload_;
}
inline void EchoResponse::set_payload(const ::std::string& value) {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::kEmptyString) {
    payload_ = new ::std::string;
  }
  payload_->assign(value);
}
inline void EchoResponse::set_payload(const char* value) {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::kEmptyString) {
    payload_ = new ::std::string;
  }
  payload_->assign(value);
}
inline void EchoResponse::set_payload(const char* value, size_t size) {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::kEmptyString) {
    payload_ = new ::std::string;
  }
  payload_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* EchoResponse::mutable_payload() {
  set_has_payload();
  if (payload_ == &::google::protobuf::internal::kEmptyString) {
    payload_ = new ::std::string;
  }
  return payload_;
}
inline ::std::string* EchoResponse::release_payload() {
  clear_has_payload();
  if (payload_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = payload_;
    payload_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace echo

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_echo_2eproto__INCLUDED


// Local Variables:
// mode: c++
// End:

