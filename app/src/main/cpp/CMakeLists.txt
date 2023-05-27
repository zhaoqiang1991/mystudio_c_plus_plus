cmake_minimum_required(VERSION 3.22.1)

#这个project引用的名字就是未来在java工程里面引用到的库名字
#比如 System.loadLibrary("native");
project("native")

add_library(
        native
        SHARED
        native-lib.cpp)

find_library(
        log-lib
        log)


target_link_libraries(
        native
        ${log-lib})