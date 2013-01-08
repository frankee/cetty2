cd ../../include
d:/alp2/cetty/bin/ConfigGeneratord.exe  ./cetty/service/builder/ServerBuilderConfig.proto -I. --config_out=.
mv ./cetty/service/builder/ServerBuilderConfig.cnf.cpp ../cetty-service/src/cetty/service/builder/
pause
