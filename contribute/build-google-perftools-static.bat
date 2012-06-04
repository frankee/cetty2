echo set building enviroment
set VS_PATH=c:\Program Files\Microsoft Visual Studio 9.0
call "%VS_PATH%\VC\vcvarsall.bat"

echo clean
devenv google-perftools-1.7\libtcmalloc_minimal_static.sln /clean "Debug|Win32"
devenv google-perftools-1.7\libtcmalloc_minimal_static.sln /clean "Release|Win32"

echo compile
devenv google-perftools-1.7\libtcmalloc_minimal_static.sln /rebuild "Debug|Win32"
devenv google-perftools-1.7\libtcmalloc_minimal_static.sln /rebuild "Release|Win32"

xcopy google-perftools-1.7\debug\libtcmalloc_minimal-Debug.lib ..\lib\libtcmalloc_minimal-Debug.lib
xcopy google-perftools-1.7\release\libtcmalloc_minimal.lib ..\lib\libtcmalloc_minimal.lib