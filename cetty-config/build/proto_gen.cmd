cd ../../bin
set path=%path%;%cd%

cd ../include
protoc ./cetty/config/config_options.proto -I. -I../../thirdparty/protobuf-2.4.1/src --cpp_out=.
ConfigGeneratord.exe  ./cetty/config/KeyValuePair.proto -I. -I../../thirdparty/protobuf-2.4.1/src --config_out=.

mv ./cetty/config/config_options.pb.cc ../cetty-config/src/cetty/config/
mv ./cetty/config/KeyValuePair.cnf.cpp ../cetty-config/src/cetty/config/
pause
