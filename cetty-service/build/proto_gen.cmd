cd ../../bin
set path=%path%;%cd%

cd ../include
ConfigGeneratord.exe  ./cetty/service/builder/ServerBuilderConfig.proto -I. --config_out=.
mv ./cetty/service/builder/ServerBuilderConfig.cnf.cpp ../cetty-service/src/cetty/service/builder/
pause
