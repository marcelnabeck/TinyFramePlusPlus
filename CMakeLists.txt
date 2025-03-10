cmake_minimum_required(VERSION 3.7)
project(TinyFramePlusPlus)

# This is a dummy CMakeLists file for CLion, which still doesn't understand Makefiles.
# Do not try to build the library using this file, it's here only for static analysis.

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

set(CMAKE_CXX_STANDARD 14)

set(SOURCES_TINYFRAME
        TinyFrame.hpp
        TinyFrame_CheckSum.cpp
        TinyFrame_CheckSum.hpp
        TinyFrame_Types.hpp
        )

set(SOURCES_UTILS
        demo/utils.cpp
        demo/utils.hpp
        )

set(SOURCES_DEMO
        demo/demo.cpp
        demo/demo.hpp
        )

        
add_executable(Tests ${SOURCES_TINYFRAME} tests/TF_SimpleTest.cpp)
target_link_options(Tests PRIVATE "LINKER:-Map=Tests.map")

add_executable(demo.simple ${SOURCES_TINYFRAME} ${SOURCES_UTILS} demo/simple/test.cpp)
add_executable(demo.simple_custom_cksum ${SOURCES_TINYFRAME} ${SOURCES_UTILS} demo/simple_custom_cksum/test.cpp)
add_executable(demo.simple_long_payload ${SOURCES_TINYFRAME} ${SOURCES_UTILS} demo/simple_long_payload/test.cpp)
add_executable(demo.simple_multipart ${SOURCES_TINYFRAME} ${SOURCES_UTILS} demo/simple_multipart/test.cpp)

if (UNIX)
add_executable(demo.socket_demo.master ${SOURCES_TINYFRAME} ${SOURCES_UTILS} ${SOURCES_DEMO} demo/socket_demo/master.cpp)
add_executable(demo.socket_demo.slave ${SOURCES_TINYFRAME} ${SOURCES_UTILS} ${SOURCES_DEMO} demo/socket_demo/slave.cpp)
endif (UNIX)