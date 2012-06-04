echo set building enviroment
set VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio 10.0
call "%VS_PATH%\VC\vcvarsall.bat"

bjam.exe --with-system --with-date_time --with-thread --with-regex --with-program_options --with-filesystem --toolset=msvc-10.0 --link=static --threading=multi --runtimelink=shared stage