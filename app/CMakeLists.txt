cmake_minimum_required(VERSION 3.10.2)

#这个project引用的名字就是未来在java工程里面引用到的库名字
#比如 System.loadLibrary("native");
project("tigerPlayer")
# 引入指定目录下的CMakeLists.txt
add_subdirectory(src/main/cpp/librtmp)
#指定头文件的搜索路径 ,这个必须 放在add_library之前，就是配置的信息一定要放在最前面
include_directories(src/main/cpp/include)
#指定编译路径
file(GLOB SOURCE src/main/cpp/*.cpp)
#file(GLOB SOURCE "src/main/cpp/nativeClient.cpp")

#配置so库输出路径，但是这个配置必须放在add_library之前，否则不生效,其中libs，以及libs下面的所有目录都不需要手动创建，只要在
#CMakelist配置好就会自动生成
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI})

#将ANDROID_ABI(动态获取的)设置为环境路径
set(my_lib_path ${CMAKE_SOURCE_DIR}/src/main/cpp/libs/${ANDROID_ABI})
#需要传递的编译路径
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${my_lib_path}")
message("CMAKE_SOURCE_DIR= "${CMAKE_SOURCE_DIR})
message("CMAKE_CXX_FLAGS= "${CMAKE_CXX_FLAGS})

#定义宏开关
#add_compile_definitions(ENABLE_JNI_ONLOAD)
add_definitions(-DENABLE_LIVE_JNI_ONLOAD)

add_library(tigerPlayer SHARED ${SOURCE})#src/main/cpp/LiveClient.cpp


find_library(
        log-lib
        log)

#[[target_link_libraries(
        tigerPlayer
        avfilter
        avformat
        avcodec
        avutil
        swresample
        swscale
        z
        OpenSLES
        android
        ${log-lib})]]


target_link_libraries(
        tigerPlayer
        avfilter avformat    avcodec  avutil  swresample swscale
        # ffmpeg
        z
        OpenSLES
        android
        rtmp
        x264
        faac
        ${log-lib} )