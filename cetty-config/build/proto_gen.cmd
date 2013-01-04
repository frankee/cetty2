cd ../../include
protoc ./cetty/config/config_options.proto -I. -I../../thirdparty/protobuf-2.4.1/src --cpp_out=.
protoc --plugin=protoc-gen-config=../bin/ConfigGeneratord.exe  ./cetty/config/KeyValuePair.proto -I. -I../../thirdparty/protobuf-2.4.1/src --config_out=.
mv ./cetty/config/KeyValuePair.cnf.cpp ../cetty-config/src/cetty/config/
pause
