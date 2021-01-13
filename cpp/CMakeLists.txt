# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (pelco_control)

# 指定生成目标
add_executable(pelco pelco.cpp)
add_executable(demo demo.cpp pelcoSerial.cpp)
