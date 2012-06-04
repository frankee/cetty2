echo set building enviroment
set VS_PATH=c:\Program Files\Microsoft Visual Studio 9.0
call "%VS_PATH%\VC\vcvarsall.bat"

echo clean
devenv gtest-1.6.0\msvc\gtest-md.sln /clean "Debug|Win32"
devenv gtest-1.6.0\msvc\gtest-md.sln /clean "Release|Win32"

echo compile
devenv gtest-1.6.0\msvc\gtest-md.sln /rebuild "Debug|Win32"
devenv gtest-1.6.0\msvc\gtest-md.sln /rebuild "Release|Win32"

xcopy gtest-1.6.0\msvc\gtest-md\Debug\gtestd.lib ..\lib\
xcopy gtest-1.6.0\msvc\gtest-md\Debug\gtest_main-mdd.lib ..\lib\
xcopy gtest-1.6.0\msvc\gtest-md\Release\gtest.lib ..\lib\
xcopy gtest-1.6.0\msvc\gtest-md\Release\gtest_main-md.lib ..\lib\