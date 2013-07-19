cd ../../bin
set path=%path%;%cd%

cd ../include
ProtobufServiceGeneratord.exe ./cetty/zurg/slave/slave.proto ./cetty/zurg/slave/slave_service.proto -I. --service_out=.
ConfigGeneratord.exe  ./cetty/zurg/slave/SlaveServiceConfig.proto -I. --config_out=.

mv ./cetty/zurg/slave/slave.pb.cc ../cetty-zurg/src/cetty/zurg/slave/
mv ./cetty/zurg/slave/slave_service.pb.cc ../cetty-zurg/src/cetty/zurg/slave/
mv ./cetty/zurg/slave/SlaveServiceConfig.cnf.cpp ../cetty-zurg/src/cetty/zurg/slave/

pause