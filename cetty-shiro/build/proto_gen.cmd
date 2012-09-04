cd ../../include
ProtobufServiceGeneratord.exe ./cetty/shiro/web/authentication.proto ./cetty/shiro/web/authority.proto -I. --cpp_out=. --src=../cetty-shiro/src

pause