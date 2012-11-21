cd ../../include
ProtobufServiceGeneratord.exe ./cetty/protobuf/service/service.proto -I. -I../../thirdparty/protobuf-2.4.1/src --cpp_out=. --src=../cetty-protobuf/src

protoc.exe ./cetty/protobuf/service/service.proto -I. -I../../thirdparty/protobuf-2.4.1/src --java_out=.

pause