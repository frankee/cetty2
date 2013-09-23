cd ../../bin
set path=%path%;%cd%

cd ../include
ConfigGeneratord.exe  ./cetty/shuttle/ShuttleConfig.proto -I. -I../../thirdparty/_include --config_out=.
mv ./cetty/shuttle/ShuttleConfig.cnf.cpp ../cetty-shuttle/src/cetty/shuttle/
pause
