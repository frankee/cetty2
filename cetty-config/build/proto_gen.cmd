cd ../../include
protoc --plugin=protoc-gen-config=../bin/protoc-gen-config.exe  ./cetty/config/KeyValuePairConfig.proto -I. -I../../thirdparty/protobuf-2.4.1/src --config_out=.
mv ./cetty/config/KeyValuePairConfig.cnf.cpp ../cetty-config/src/cetty/config/
pause
