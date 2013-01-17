cd ../../bin
set path=%path%;%cd%


cd ../cetty-gearman/example/echo/protobuf
ProtobufServiceGeneratord.exe echo.proto -I. -I../../../../include/ --service_out=.

pause