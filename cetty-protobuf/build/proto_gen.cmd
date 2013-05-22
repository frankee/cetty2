cd ../../bin
set path=%path%;%cd%

cd ../include
ProtobufServiceGeneratord.exe  ./cetty/protobuf/service/service_options.proto  ./cetty/protobuf/service/service.proto -I. -I../../thirdparty/protobuf-2.4.1/src --service_out=.

cd ../cetty-protobuf/example/echo
ProtobufServiceGeneratord.exe echo.proto -I. -I../../../include/ --service_out=.

cd ../../../include
mv ./cetty/protobuf/service/service.pb.cc ../cetty-protobuf/src/cetty/protobuf/service/
mv ./cetty/protobuf/service/service_options.pb.cc ../cetty-protobuf/src/cetty/protobuf/service/

pause