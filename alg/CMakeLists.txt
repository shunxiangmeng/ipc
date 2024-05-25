cmake_minimum_required(VERSION 3.10.0)

#project(alg)

set(Tartget alg)

message(STATUS "******************************************************************")
message(STATUS "CMAKE_SOURCE_DIR->" ${CMAKE_SOURCE_DIR})
message(STATUS "PROJECT_SOURCE_DIR->" ${PROJECT_SOURCE_DIR})
message(STATUS "******************************************************************")

set(CMAKE_CXX_STANDARD 14)

#编译详情
#set(CMAKE_VERBOSE_MAKEFILE ON)

#交叉编译工具链信息
#include(${PROJECT_SOURCE_DIR}/Toolchains.cmake)

if(WIN32)
else()
    # 消除GCC 7.x 的一个 abi 编译 note
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")
endif()

#依赖库信息
set(APP_DEPEND_LIBS)
#include(${PROJECT_SOURCE_DIR}/Linklib.cmake)

#头文件路径
set(APP_INCLUDE_DIRS)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/ulucu)
include_directories(${PROJECT_SOURCE_DIR}/../common)
include_directories(${PROJECT_SOURCE_DIR}/../common/thirdparty)

#源文件
set(APP_SOURCE_FILES)

file(GLOB_RECURSE APP_SOURCE_FILES src/*.cpp)

#指定头文件路径
include_directories(${APP_INCLUDE_DIRS})
add_executable(${Tartget} ${APP_SOURCE_FILES})
#添加依赖库
target_link_libraries(${Tartget} ${APP_DEPEND_LIBS})

#输出目录
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/Bin/${PLATFORM}/)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../Build/${Platform})
