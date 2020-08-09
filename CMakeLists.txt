cmake_minimum_required(VERSION 3.0)

project(demo)

# 查找某一个目录下面的所有源文件
# 并将其保存到 DIR_SRCS 变量当中
aux_source_directory(. DIR_SRCS)
 
add_executable(Demo ${DIR_SRCS})

 