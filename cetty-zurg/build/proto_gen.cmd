cd ../../include
ProtobufServiceGeneratord.exe ./cetty/zurg/slave/slave.proto ./cetty/zurg/master/master.proto -I. --cpp_out=. --src_out=../cetty-zurg/src

pause