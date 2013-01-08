cd ../../include
d:/alp2/cetty/bin/ProtobufServiceGeneratord.exe  ./cetty/craft/craft_options.proto  ./cetty/craft/craft.proto  ./cetty/craft/access/AccessControl.proto -I. --service_out=.

cd ../cetty-craft/example/echo
d:/alp2/cetty/bin/ProtobufServiceGeneratord.exe ./echo.proto -I. -I../../../include  --service_out=.

pause