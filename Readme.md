# CMake Modules #

应用项目中统一的cmake模块，包含第三库的find模块，以及常用的模块，在项目的CMakeList.txt只要添加如下的行：

```cmake
include (Dependence)
include (MakeFlags)
include (Functions)
```