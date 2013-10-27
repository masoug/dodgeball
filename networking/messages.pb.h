// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: messages.proto

#ifndef PROTOBUF_messages_2eproto__INCLUDED
#define PROTOBUF_messages_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace NetProtocol {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_messages_2eproto();
void protobuf_AssignDesc_messages_2eproto();
void protobuf_ShutdownFile_messages_2eproto();

class Vector3;
class RequestPlayer;
class PlayerConfirmation;
class Error;
class NetPacket;

enum NetPacket_Type {
  NetPacket_Type_VECTOR3 = 1,
  NetPacket_Type_REQPLAYER = 2,
  NetPacket_Type_PLAYERCONFIRM = 3,
  NetPacket_Type_ERROR = 4
};
bool NetPacket_Type_IsValid(int value);
const NetPacket_Type NetPacket_Type_Type_MIN = NetPacket_Type_VECTOR3;
const NetPacket_Type NetPacket_Type_Type_MAX = NetPacket_Type_ERROR;
const int NetPacket_Type_Type_ARRAYSIZE = NetPacket_Type_Type_MAX + 1;

const ::google::protobuf::EnumDescriptor* NetPacket_Type_descriptor();
inline const ::std::string& NetPacket_Type_Name(NetPacket_Type value) {
  return ::google::protobuf::internal::NameOfEnum(
    NetPacket_Type_descriptor(), value);
}
inline bool NetPacket_Type_Parse(
    const ::std::string& name, NetPacket_Type* value) {
  return ::google::protobuf::internal::ParseNamedEnum<NetPacket_Type>(
    NetPacket_Type_descriptor(), name, value);
}
// ===================================================================

class Vector3 : public ::google::protobuf::Message {
 public:
  Vector3();
  virtual ~Vector3();

  Vector3(const Vector3& from);

  inline Vector3& operator=(const Vector3& from) {
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
  static const Vector3& default_instance();

  void Swap(Vector3* other);

  // implements Message ----------------------------------------------

  Vector3* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Vector3& from);
  void MergeFrom(const Vector3& from);
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

  // required float x = 1;
  inline bool has_x() const;
  inline void clear_x();
  static const int kXFieldNumber = 1;
  inline float x() const;
  inline void set_x(float value);

  // required float y = 2;
  inline bool has_y() const;
  inline void clear_y();
  static const int kYFieldNumber = 2;
  inline float y() const;
  inline void set_y(float value);

  // required float z = 3;
  inline bool has_z() const;
  inline void clear_z();
  static const int kZFieldNumber = 3;
  inline float z() const;
  inline void set_z(float value);

  // @@protoc_insertion_point(class_scope:NetProtocol.Vector3)
 private:
  inline void set_has_x();
  inline void clear_has_x();
  inline void set_has_y();
  inline void clear_has_y();
  inline void set_has_z();
  inline void clear_has_z();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  float x_;
  float y_;
  float z_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];

  friend void  protobuf_AddDesc_messages_2eproto();
  friend void protobuf_AssignDesc_messages_2eproto();
  friend void protobuf_ShutdownFile_messages_2eproto();

  void InitAsDefaultInstance();
  static Vector3* default_instance_;
};
// -------------------------------------------------------------------

class RequestPlayer : public ::google::protobuf::Message {
 public:
  RequestPlayer();
  virtual ~RequestPlayer();

  RequestPlayer(const RequestPlayer& from);

  inline RequestPlayer& operator=(const RequestPlayer& from) {
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
  static const RequestPlayer& default_instance();

  void Swap(RequestPlayer* other);

  // implements Message ----------------------------------------------

  RequestPlayer* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RequestPlayer& from);
  void MergeFrom(const RequestPlayer& from);
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

  // required string name = 1;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 1;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  inline ::std::string* release_name();
  inline void set_allocated_name(::std::string* name);

  // @@protoc_insertion_point(class_scope:NetProtocol.RequestPlayer)
 private:
  inline void set_has_name();
  inline void clear_has_name();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* name_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_messages_2eproto();
  friend void protobuf_AssignDesc_messages_2eproto();
  friend void protobuf_ShutdownFile_messages_2eproto();

  void InitAsDefaultInstance();
  static RequestPlayer* default_instance_;
};
// -------------------------------------------------------------------

class PlayerConfirmation : public ::google::protobuf::Message {
 public:
  PlayerConfirmation();
  virtual ~PlayerConfirmation();

  PlayerConfirmation(const PlayerConfirmation& from);

  inline PlayerConfirmation& operator=(const PlayerConfirmation& from) {
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
  static const PlayerConfirmation& default_instance();

  void Swap(PlayerConfirmation* other);

  // implements Message ----------------------------------------------

  PlayerConfirmation* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const PlayerConfirmation& from);
  void MergeFrom(const PlayerConfirmation& from);
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

  // optional uint32 id = 2;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 2;
  inline ::google::protobuf::uint32 id() const;
  inline void set_id(::google::protobuf::uint32 value);

  // optional string name = 3;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 3;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  inline ::std::string* release_name();
  inline void set_allocated_name(::std::string* name);

  // optional uint32 type = 4;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 4;
  inline ::google::protobuf::uint32 type() const;
  inline void set_type(::google::protobuf::uint32 value);

  // optional .NetProtocol.Vector3 initPos = 5;
  inline bool has_initpos() const;
  inline void clear_initpos();
  static const int kInitPosFieldNumber = 5;
  inline const ::NetProtocol::Vector3& initpos() const;
  inline ::NetProtocol::Vector3* mutable_initpos();
  inline ::NetProtocol::Vector3* release_initpos();
  inline void set_allocated_initpos(::NetProtocol::Vector3* initpos);

  // @@protoc_insertion_point(class_scope:NetProtocol.PlayerConfirmation)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_name();
  inline void clear_has_name();
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_initpos();
  inline void clear_has_initpos();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* name_;
  ::google::protobuf::uint32 id_;
  ::google::protobuf::uint32 type_;
  ::NetProtocol::Vector3* initpos_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];

  friend void  protobuf_AddDesc_messages_2eproto();
  friend void protobuf_AssignDesc_messages_2eproto();
  friend void protobuf_ShutdownFile_messages_2eproto();

  void InitAsDefaultInstance();
  static PlayerConfirmation* default_instance_;
};
// -------------------------------------------------------------------

class Error : public ::google::protobuf::Message {
 public:
  Error();
  virtual ~Error();

  Error(const Error& from);

  inline Error& operator=(const Error& from) {
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
  static const Error& default_instance();

  void Swap(Error* other);

  // implements Message ----------------------------------------------

  Error* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Error& from);
  void MergeFrom(const Error& from);
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

  // required uint32 errorCode = 1;
  inline bool has_errorcode() const;
  inline void clear_errorcode();
  static const int kErrorCodeFieldNumber = 1;
  inline ::google::protobuf::uint32 errorcode() const;
  inline void set_errorcode(::google::protobuf::uint32 value);

  // required string errorMsg = 2;
  inline bool has_errormsg() const;
  inline void clear_errormsg();
  static const int kErrorMsgFieldNumber = 2;
  inline const ::std::string& errormsg() const;
  inline void set_errormsg(const ::std::string& value);
  inline void set_errormsg(const char* value);
  inline void set_errormsg(const char* value, size_t size);
  inline ::std::string* mutable_errormsg();
  inline ::std::string* release_errormsg();
  inline void set_allocated_errormsg(::std::string* errormsg);

  // @@protoc_insertion_point(class_scope:NetProtocol.Error)
 private:
  inline void set_has_errorcode();
  inline void clear_has_errorcode();
  inline void set_has_errormsg();
  inline void clear_has_errormsg();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* errormsg_;
  ::google::protobuf::uint32 errorcode_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_messages_2eproto();
  friend void protobuf_AssignDesc_messages_2eproto();
  friend void protobuf_ShutdownFile_messages_2eproto();

  void InitAsDefaultInstance();
  static Error* default_instance_;
};
// -------------------------------------------------------------------

class NetPacket : public ::google::protobuf::Message {
 public:
  NetPacket();
  virtual ~NetPacket();

  NetPacket(const NetPacket& from);

  inline NetPacket& operator=(const NetPacket& from) {
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
  static const NetPacket& default_instance();

  void Swap(NetPacket* other);

  // implements Message ----------------------------------------------

  NetPacket* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const NetPacket& from);
  void MergeFrom(const NetPacket& from);
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

  typedef NetPacket_Type Type;
  static const Type VECTOR3 = NetPacket_Type_VECTOR3;
  static const Type REQPLAYER = NetPacket_Type_REQPLAYER;
  static const Type PLAYERCONFIRM = NetPacket_Type_PLAYERCONFIRM;
  static const Type ERROR = NetPacket_Type_ERROR;
  static inline bool Type_IsValid(int value) {
    return NetPacket_Type_IsValid(value);
  }
  static const Type Type_MIN =
    NetPacket_Type_Type_MIN;
  static const Type Type_MAX =
    NetPacket_Type_Type_MAX;
  static const int Type_ARRAYSIZE =
    NetPacket_Type_Type_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Type_descriptor() {
    return NetPacket_Type_descriptor();
  }
  static inline const ::std::string& Type_Name(Type value) {
    return NetPacket_Type_Name(value);
  }
  static inline bool Type_Parse(const ::std::string& name,
      Type* value) {
    return NetPacket_Type_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // required .NetProtocol.NetPacket.Type type = 1;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 1;
  inline ::NetProtocol::NetPacket_Type type() const;
  inline void set_type(::NetProtocol::NetPacket_Type value);

  // optional .NetProtocol.Vector3 vector3 = 2;
  inline bool has_vector3() const;
  inline void clear_vector3();
  static const int kVector3FieldNumber = 2;
  inline const ::NetProtocol::Vector3& vector3() const;
  inline ::NetProtocol::Vector3* mutable_vector3();
  inline ::NetProtocol::Vector3* release_vector3();
  inline void set_allocated_vector3(::NetProtocol::Vector3* vector3);

  // optional .NetProtocol.RequestPlayer request_player = 3;
  inline bool has_request_player() const;
  inline void clear_request_player();
  static const int kRequestPlayerFieldNumber = 3;
  inline const ::NetProtocol::RequestPlayer& request_player() const;
  inline ::NetProtocol::RequestPlayer* mutable_request_player();
  inline ::NetProtocol::RequestPlayer* release_request_player();
  inline void set_allocated_request_player(::NetProtocol::RequestPlayer* request_player);

  // optional .NetProtocol.PlayerConfirmation player_confirmation = 4;
  inline bool has_player_confirmation() const;
  inline void clear_player_confirmation();
  static const int kPlayerConfirmationFieldNumber = 4;
  inline const ::NetProtocol::PlayerConfirmation& player_confirmation() const;
  inline ::NetProtocol::PlayerConfirmation* mutable_player_confirmation();
  inline ::NetProtocol::PlayerConfirmation* release_player_confirmation();
  inline void set_allocated_player_confirmation(::NetProtocol::PlayerConfirmation* player_confirmation);

  // optional .NetProtocol.Error error = 5;
  inline bool has_error() const;
  inline void clear_error();
  static const int kErrorFieldNumber = 5;
  inline const ::NetProtocol::Error& error() const;
  inline ::NetProtocol::Error* mutable_error();
  inline ::NetProtocol::Error* release_error();
  inline void set_allocated_error(::NetProtocol::Error* error);

  // @@protoc_insertion_point(class_scope:NetProtocol.NetPacket)
 private:
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_vector3();
  inline void clear_has_vector3();
  inline void set_has_request_player();
  inline void clear_has_request_player();
  inline void set_has_player_confirmation();
  inline void clear_has_player_confirmation();
  inline void set_has_error();
  inline void clear_has_error();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::NetProtocol::Vector3* vector3_;
  ::NetProtocol::RequestPlayer* request_player_;
  ::NetProtocol::PlayerConfirmation* player_confirmation_;
  ::NetProtocol::Error* error_;
  int type_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];

  friend void  protobuf_AddDesc_messages_2eproto();
  friend void protobuf_AssignDesc_messages_2eproto();
  friend void protobuf_ShutdownFile_messages_2eproto();

  void InitAsDefaultInstance();
  static NetPacket* default_instance_;
};
// ===================================================================


// ===================================================================

// Vector3

// required float x = 1;
inline bool Vector3::has_x() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Vector3::set_has_x() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Vector3::clear_has_x() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Vector3::clear_x() {
  x_ = 0;
  clear_has_x();
}
inline float Vector3::x() const {
  return x_;
}
inline void Vector3::set_x(float value) {
  set_has_x();
  x_ = value;
}

// required float y = 2;
inline bool Vector3::has_y() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Vector3::set_has_y() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Vector3::clear_has_y() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Vector3::clear_y() {
  y_ = 0;
  clear_has_y();
}
inline float Vector3::y() const {
  return y_;
}
inline void Vector3::set_y(float value) {
  set_has_y();
  y_ = value;
}

// required float z = 3;
inline bool Vector3::has_z() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Vector3::set_has_z() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Vector3::clear_has_z() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Vector3::clear_z() {
  z_ = 0;
  clear_has_z();
}
inline float Vector3::z() const {
  return z_;
}
inline void Vector3::set_z(float value) {
  set_has_z();
  z_ = value;
}

// -------------------------------------------------------------------

// RequestPlayer

// required string name = 1;
inline bool RequestPlayer::has_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RequestPlayer::set_has_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RequestPlayer::clear_has_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RequestPlayer::clear_name() {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& RequestPlayer::name() const {
  return *name_;
}
inline void RequestPlayer::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void RequestPlayer::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void RequestPlayer::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RequestPlayer::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  return name_;
}
inline ::std::string* RequestPlayer::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void RequestPlayer::set_allocated_name(::std::string* name) {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    delete name_;
  }
  if (name) {
    set_has_name();
    name_ = name;
  } else {
    clear_has_name();
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// -------------------------------------------------------------------

// PlayerConfirmation

// optional uint32 id = 2;
inline bool PlayerConfirmation::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void PlayerConfirmation::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void PlayerConfirmation::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void PlayerConfirmation::clear_id() {
  id_ = 0u;
  clear_has_id();
}
inline ::google::protobuf::uint32 PlayerConfirmation::id() const {
  return id_;
}
inline void PlayerConfirmation::set_id(::google::protobuf::uint32 value) {
  set_has_id();
  id_ = value;
}

// optional string name = 3;
inline bool PlayerConfirmation::has_name() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void PlayerConfirmation::set_has_name() {
  _has_bits_[0] |= 0x00000002u;
}
inline void PlayerConfirmation::clear_has_name() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void PlayerConfirmation::clear_name() {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& PlayerConfirmation::name() const {
  return *name_;
}
inline void PlayerConfirmation::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void PlayerConfirmation::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void PlayerConfirmation::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* PlayerConfirmation::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  return name_;
}
inline ::std::string* PlayerConfirmation::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void PlayerConfirmation::set_allocated_name(::std::string* name) {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    delete name_;
  }
  if (name) {
    set_has_name();
    name_ = name;
  } else {
    clear_has_name();
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional uint32 type = 4;
inline bool PlayerConfirmation::has_type() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void PlayerConfirmation::set_has_type() {
  _has_bits_[0] |= 0x00000004u;
}
inline void PlayerConfirmation::clear_has_type() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void PlayerConfirmation::clear_type() {
  type_ = 0u;
  clear_has_type();
}
inline ::google::protobuf::uint32 PlayerConfirmation::type() const {
  return type_;
}
inline void PlayerConfirmation::set_type(::google::protobuf::uint32 value) {
  set_has_type();
  type_ = value;
}

// optional .NetProtocol.Vector3 initPos = 5;
inline bool PlayerConfirmation::has_initpos() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void PlayerConfirmation::set_has_initpos() {
  _has_bits_[0] |= 0x00000008u;
}
inline void PlayerConfirmation::clear_has_initpos() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void PlayerConfirmation::clear_initpos() {
  if (initpos_ != NULL) initpos_->::NetProtocol::Vector3::Clear();
  clear_has_initpos();
}
inline const ::NetProtocol::Vector3& PlayerConfirmation::initpos() const {
  return initpos_ != NULL ? *initpos_ : *default_instance_->initpos_;
}
inline ::NetProtocol::Vector3* PlayerConfirmation::mutable_initpos() {
  set_has_initpos();
  if (initpos_ == NULL) initpos_ = new ::NetProtocol::Vector3;
  return initpos_;
}
inline ::NetProtocol::Vector3* PlayerConfirmation::release_initpos() {
  clear_has_initpos();
  ::NetProtocol::Vector3* temp = initpos_;
  initpos_ = NULL;
  return temp;
}
inline void PlayerConfirmation::set_allocated_initpos(::NetProtocol::Vector3* initpos) {
  delete initpos_;
  initpos_ = initpos;
  if (initpos) {
    set_has_initpos();
  } else {
    clear_has_initpos();
  }
}

// -------------------------------------------------------------------

// Error

// required uint32 errorCode = 1;
inline bool Error::has_errorcode() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Error::set_has_errorcode() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Error::clear_has_errorcode() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Error::clear_errorcode() {
  errorcode_ = 0u;
  clear_has_errorcode();
}
inline ::google::protobuf::uint32 Error::errorcode() const {
  return errorcode_;
}
inline void Error::set_errorcode(::google::protobuf::uint32 value) {
  set_has_errorcode();
  errorcode_ = value;
}

// required string errorMsg = 2;
inline bool Error::has_errormsg() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Error::set_has_errormsg() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Error::clear_has_errormsg() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Error::clear_errormsg() {
  if (errormsg_ != &::google::protobuf::internal::kEmptyString) {
    errormsg_->clear();
  }
  clear_has_errormsg();
}
inline const ::std::string& Error::errormsg() const {
  return *errormsg_;
}
inline void Error::set_errormsg(const ::std::string& value) {
  set_has_errormsg();
  if (errormsg_ == &::google::protobuf::internal::kEmptyString) {
    errormsg_ = new ::std::string;
  }
  errormsg_->assign(value);
}
inline void Error::set_errormsg(const char* value) {
  set_has_errormsg();
  if (errormsg_ == &::google::protobuf::internal::kEmptyString) {
    errormsg_ = new ::std::string;
  }
  errormsg_->assign(value);
}
inline void Error::set_errormsg(const char* value, size_t size) {
  set_has_errormsg();
  if (errormsg_ == &::google::protobuf::internal::kEmptyString) {
    errormsg_ = new ::std::string;
  }
  errormsg_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Error::mutable_errormsg() {
  set_has_errormsg();
  if (errormsg_ == &::google::protobuf::internal::kEmptyString) {
    errormsg_ = new ::std::string;
  }
  return errormsg_;
}
inline ::std::string* Error::release_errormsg() {
  clear_has_errormsg();
  if (errormsg_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = errormsg_;
    errormsg_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void Error::set_allocated_errormsg(::std::string* errormsg) {
  if (errormsg_ != &::google::protobuf::internal::kEmptyString) {
    delete errormsg_;
  }
  if (errormsg) {
    set_has_errormsg();
    errormsg_ = errormsg;
  } else {
    clear_has_errormsg();
    errormsg_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// -------------------------------------------------------------------

// NetPacket

// required .NetProtocol.NetPacket.Type type = 1;
inline bool NetPacket::has_type() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void NetPacket::set_has_type() {
  _has_bits_[0] |= 0x00000001u;
}
inline void NetPacket::clear_has_type() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void NetPacket::clear_type() {
  type_ = 1;
  clear_has_type();
}
inline ::NetProtocol::NetPacket_Type NetPacket::type() const {
  return static_cast< ::NetProtocol::NetPacket_Type >(type_);
}
inline void NetPacket::set_type(::NetProtocol::NetPacket_Type value) {
  assert(::NetProtocol::NetPacket_Type_IsValid(value));
  set_has_type();
  type_ = value;
}

// optional .NetProtocol.Vector3 vector3 = 2;
inline bool NetPacket::has_vector3() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void NetPacket::set_has_vector3() {
  _has_bits_[0] |= 0x00000002u;
}
inline void NetPacket::clear_has_vector3() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void NetPacket::clear_vector3() {
  if (vector3_ != NULL) vector3_->::NetProtocol::Vector3::Clear();
  clear_has_vector3();
}
inline const ::NetProtocol::Vector3& NetPacket::vector3() const {
  return vector3_ != NULL ? *vector3_ : *default_instance_->vector3_;
}
inline ::NetProtocol::Vector3* NetPacket::mutable_vector3() {
  set_has_vector3();
  if (vector3_ == NULL) vector3_ = new ::NetProtocol::Vector3;
  return vector3_;
}
inline ::NetProtocol::Vector3* NetPacket::release_vector3() {
  clear_has_vector3();
  ::NetProtocol::Vector3* temp = vector3_;
  vector3_ = NULL;
  return temp;
}
inline void NetPacket::set_allocated_vector3(::NetProtocol::Vector3* vector3) {
  delete vector3_;
  vector3_ = vector3;
  if (vector3) {
    set_has_vector3();
  } else {
    clear_has_vector3();
  }
}

// optional .NetProtocol.RequestPlayer request_player = 3;
inline bool NetPacket::has_request_player() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void NetPacket::set_has_request_player() {
  _has_bits_[0] |= 0x00000004u;
}
inline void NetPacket::clear_has_request_player() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void NetPacket::clear_request_player() {
  if (request_player_ != NULL) request_player_->::NetProtocol::RequestPlayer::Clear();
  clear_has_request_player();
}
inline const ::NetProtocol::RequestPlayer& NetPacket::request_player() const {
  return request_player_ != NULL ? *request_player_ : *default_instance_->request_player_;
}
inline ::NetProtocol::RequestPlayer* NetPacket::mutable_request_player() {
  set_has_request_player();
  if (request_player_ == NULL) request_player_ = new ::NetProtocol::RequestPlayer;
  return request_player_;
}
inline ::NetProtocol::RequestPlayer* NetPacket::release_request_player() {
  clear_has_request_player();
  ::NetProtocol::RequestPlayer* temp = request_player_;
  request_player_ = NULL;
  return temp;
}
inline void NetPacket::set_allocated_request_player(::NetProtocol::RequestPlayer* request_player) {
  delete request_player_;
  request_player_ = request_player;
  if (request_player) {
    set_has_request_player();
  } else {
    clear_has_request_player();
  }
}

// optional .NetProtocol.PlayerConfirmation player_confirmation = 4;
inline bool NetPacket::has_player_confirmation() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void NetPacket::set_has_player_confirmation() {
  _has_bits_[0] |= 0x00000008u;
}
inline void NetPacket::clear_has_player_confirmation() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void NetPacket::clear_player_confirmation() {
  if (player_confirmation_ != NULL) player_confirmation_->::NetProtocol::PlayerConfirmation::Clear();
  clear_has_player_confirmation();
}
inline const ::NetProtocol::PlayerConfirmation& NetPacket::player_confirmation() const {
  return player_confirmation_ != NULL ? *player_confirmation_ : *default_instance_->player_confirmation_;
}
inline ::NetProtocol::PlayerConfirmation* NetPacket::mutable_player_confirmation() {
  set_has_player_confirmation();
  if (player_confirmation_ == NULL) player_confirmation_ = new ::NetProtocol::PlayerConfirmation;
  return player_confirmation_;
}
inline ::NetProtocol::PlayerConfirmation* NetPacket::release_player_confirmation() {
  clear_has_player_confirmation();
  ::NetProtocol::PlayerConfirmation* temp = player_confirmation_;
  player_confirmation_ = NULL;
  return temp;
}
inline void NetPacket::set_allocated_player_confirmation(::NetProtocol::PlayerConfirmation* player_confirmation) {
  delete player_confirmation_;
  player_confirmation_ = player_confirmation;
  if (player_confirmation) {
    set_has_player_confirmation();
  } else {
    clear_has_player_confirmation();
  }
}

// optional .NetProtocol.Error error = 5;
inline bool NetPacket::has_error() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void NetPacket::set_has_error() {
  _has_bits_[0] |= 0x00000010u;
}
inline void NetPacket::clear_has_error() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void NetPacket::clear_error() {
  if (error_ != NULL) error_->::NetProtocol::Error::Clear();
  clear_has_error();
}
inline const ::NetProtocol::Error& NetPacket::error() const {
  return error_ != NULL ? *error_ : *default_instance_->error_;
}
inline ::NetProtocol::Error* NetPacket::mutable_error() {
  set_has_error();
  if (error_ == NULL) error_ = new ::NetProtocol::Error;
  return error_;
}
inline ::NetProtocol::Error* NetPacket::release_error() {
  clear_has_error();
  ::NetProtocol::Error* temp = error_;
  error_ = NULL;
  return temp;
}
inline void NetPacket::set_allocated_error(::NetProtocol::Error* error) {
  delete error_;
  error_ = error;
  if (error) {
    set_has_error();
  } else {
    clear_has_error();
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace NetProtocol

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::NetProtocol::NetPacket_Type>() {
  return ::NetProtocol::NetPacket_Type_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_messages_2eproto__INCLUDED
