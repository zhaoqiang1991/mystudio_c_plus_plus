cmake_minimum_required(VERSION 3.22.1)

#这个project引用的名字就是未来在java工程里面引用到的库名字
#比如 System.loadLibrary("native");
project("native")
#指定编译路径
file(GLOB SOURCES "src/main/cpp/*.cpp")

add_library(native SHARED ${SOURCES})

#指定头文件的搜索路径
include_directories(src/main/cpp/*.h)
find_library(
        log-lib
        log)

#将ANDROID_ABI(动态获取的)设置为环境路径
set(my_lib_path ${CMAKE_SOURCE_DIR}/src/main/cpp/libs/${ANDROID_ABI})
#需要传递的编译路径
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${my_lib_path} ")


target_link_libraries(
        native
        ${log-lib})