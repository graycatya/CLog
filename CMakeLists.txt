# CMakeList.txt: CLog 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("CatCLogTest")


option (_DEBUGORRELEASE "debug is ON , RELEASE is OFF" OFF)

if(_DEBUGORRELEASE)
    SET(CMAKE_BUILD_TYPE "Debug")
    SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -gdwarf-2")
    SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
else()
    SET(CMAKE_BUILD_TYPE "Release")
endif(_DEBUGORRELEASE)

include_directories("${PROJECT_SOURCE_DIR}/CatCLog")
add_subdirectory(${PROJECT_SOURCE_DIR}/CatCLog  CatCLog.out)
set (EXTRA_LIBS0 ${EXTRA_LIBS0} CatCLog)

aux_source_directory(. DIR_SRCS)

link_libraries(pthread)

# 将源代码添加到此项目的可执行文件。
add_executable (CatCLogTest ${DIR_SRCS})
target_link_libraries(CatCLogTest ${EXTRA_LIBS0})

# TODO: 如有需要，请添加测试并安装目标。
