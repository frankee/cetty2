echo set building enviroment
set VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio 10.0
call "%VS_PATH%\VC\vcvarsall.bat"

echo clean
devenv protobuf-2.4.1\vsprojects\protobuf.sln /clean "Debug|Win32"
devenv protobuf-2.4.1\vsprojects\protobuf.sln /clean "Release|Win32"

echo compile
devenv protobuf-2.4.1\vsprojects\protobuf.sln /rebuild "Debug|Win32"
devenv protobuf-2.4.1\vsprojects\protobuf.sln /rebuild "Release|Win32"

xcopy protobuf-2.4.1\vsprojects\Debug\libprotobuf.lib .\_lib\libprotobufd.lib
xcopy protobuf-2.4.1\vsprojects\Debug\libprotobuf-lite.lib .\_lib\libprotobuf-lited.lib
xcopy protobuf-2.4.1\vsprojects\Release\libprotobuf.lib .\_lib\libprotobuf.lib
xcopy protobuf-2.4.1\vsprojects\Release\libprotobuf-lite.lib .\_lib\libprotobuf-lite.lib
xcopy protobuf-2.4.1\vsprojects\Release\protoc.exe .\_bin\protoc.exe

call protobuf-2.4.1\vsprojects\extract_includes.bat
xcopy include\google .\_include\google /s
erase include
