cd ../../include
protoc.exe  ./cetty/protobuf/service/service_options.proto  ./cetty/protobuf/service/service.proto -I. -I../../thirdparty/protobuf-2.4.1/src --cpp_out=.

mv ./cetty/protobuf/service/service.pb.cc ../cetty-protobuf/src/cetty/protobuf/service/
mv ./cetty/protobuf/service/service_options.pb.cc ../cetty-protobuf/src/cetty/protobuf/service/

pause