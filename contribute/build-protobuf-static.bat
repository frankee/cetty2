echo set building enviroment
set VS_PATH=c:\Program Files\Microsoft Visual Studio 9.0
call "%VS_PATH%\VC\vcvarsall.bat"

echo clean
devenv protobuf-2.4.1\vsprojects\protobuf.sln /clean "Debug|Win32"
devenv protobuf-2.4.1\vsprojects\protobuf.sln /clean "Release|Win32"

echo compile
devenv protobuf-2.4.1\vsprojects\protobuf.sln /rebuild "Debug|Win32"
devenv protobuf-2.4.1\vsprojects\protobuf.sln /rebuild "Release|Win32"

xcopy protobuf-2.4.1\vsprojects\Debug\libprotobuf.lib ..\lib\libprotobufd.lib
xcopy protobuf-2.4.1\vsprojects\Debug\libprotobuf-lite.lib ..\lib\libprotobuf-lited.lib
xcopy protobuf-2.4.1\vsprojects\Release\libprotobuf.lib ..\lib\libprotobuf.lib
xcopy protobuf-2.4.1\vsprojects\Release\libprotobuf-lite.lib ..\lib\libprotobuf-lite.lib
xcopy protobuf-2.4.1\vsprojects\Release\protoc.exe ..\bin\protoc.exe

call protobuf-2.4.1\vsprojects\extract_includes.bat
xcopy include\google ..\include\google /s
erase include
