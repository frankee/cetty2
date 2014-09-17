cd ../../bin
set path=%path%;%cd%

cd ../include
ProtobufServiceGeneratord.exe ./cetty/craft/craft_options.proto  ./cetty/craft/craft.proto  ./cetty/craft/access/AccessControl.proto -I. --service_out=.

cd ../cetty-craft/example/echo
ProtobufServiceGeneratord.exe echo.proto -I. -I../../../include/ --service_out=.

cd ../../../include
move cetty\craft\craft.pb.cc ..\cetty-craft\src\cetty\craft\
move cetty\craft\craft_options.pb.cc ..\cetty-craft\src\cetty\craft\
move cetty\craft\access\AccessControl.pb.cc  ..\cetty-craft\src\cetty\craft\access\

pause