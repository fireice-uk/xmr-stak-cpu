project(xmr-stak-cpu)

cmake_minimum_required(VERSION 2.8.10)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.1)
        message(FATAL_ERROR "GCC version must be at least 5.1!")
    endif()
endif()

find_library(MHTD NAMES microhttpd)
if("${MHTD}" STREQUAL "MHTD-NOTFOUND")
    message(STATUS "libmicrohttpd not found.")
else()
    message(STATUS "libmicrohttpd found.")
    add_definitions(-DWITH_LIBMICROHTTPD)
    set(USE_HTTPD 1)
endif()

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CONFIGURATION_TYPES "RELEASE;STATIC")
if("${CMAKE_BUILD_TYPE}" STREQUAL "") 
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_C_FLAGS "-DNDEBUG -march=westmere -O3 -m64 -s")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")

set(CMAKE_EXE_LINKER_FLAGS_RELSEASE "")
set(CMAKE_EXE_LINKER_FLAGS_STATIC "-static-libgcc -static-libstdc++")

set(EXECUTABLE_OUTPUT_PATH "bin")

set(SOURCES
    crypto/c_blake256.c
    crypto/c_groestl.c
    crypto/c_jh.c
    crypto/c_keccak.c
    crypto/c_skein.c
    crypto/soft_aes.c
    crypto/cryptonight_common.cpp
    cli-miner.cpp
    console.cpp
    executor.cpp
    jconf.cpp
    jpsock.cpp
    minethd.cpp)

if(USE_HTTPD)
    set(SOURCES ${SOURCES} httpd.cpp)
endif(USE_HTTPD)

add_executable(xmr-stak-cpu ${SOURCES})
target_link_libraries(xmr-stak-cpu pthread)

if(USE_HTTPD)
    target_link_libraries(xmr-stak-cpu microhttpd)
endif(USE_HTTPD)
