# CMakeList.txt: CLog 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("CatCLog")

option (_DEBUGORRELEASE "debug is ON , RELEASE is OFF" OFF)

if(_DEBUGORRELEASE)
    SET(CMAKE_BUILD_TYPE "Debug")
    SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -gdwarf-2")
    SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
else()
    SET(CMAKE_BUILD_TYPE "Release")
endif(_DEBUGORRELEASE)

link_libraries(pthread)

aux_source_directory(. DIR_SRCS)

# 指定生成 TransferSock 链接库
#add_library(CatCLog ${DIR_LIB_SRCS})
add_library(CatCLog SHARED ${DIR_SRCS} )

# TODO: 如有需要，请添加测试并安装目标。
