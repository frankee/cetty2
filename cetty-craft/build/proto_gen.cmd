cd ../../include
ProtobufServiceGeneratord.exe ./cetty/craft/craft.proto ./cetty/craft/access/AccessControl.proto -I. --cpp_out=. --src_out=../cetty-craft/src

cd ../cetty-craft/example/echo
ProtobufServiceGeneratord.exe ./echo.proto -I. --cpp_out=.

pause