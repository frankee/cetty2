cd ../../include
ProtobufServiceGeneratord.exe ./cetty/protobuf/service/service.proto ./cetty/protobuf/service/common.proto -I. -I../../thirdparty/protobuf-2.4.1/src --cpp_out=. --src=../cetty-protobuf/src

pause