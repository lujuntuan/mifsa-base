#*********************************************************************************
#  *Copyright(C): Juntuan.Lu, 2020-2030, All rights reserved.
#  *Author:  Juntuan.Lu
#  *Version: 1.0
#  *Date:  2022/04/01
#  *Email: 931852884@qq.com
#  *Description:
#  *Others:
#  *Function List:
#  *History:
#**********************************************************************************

cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(MIFSA_MODULE_TYPE base)
file(STRINGS version.txt MIFSA_MODULE_VERSION)
project(mifsa_${MIFSA_MODULE_TYPE} VERSION ${MIFSA_MODULE_VERSION} LANGUAGES C CXX)
#set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build to Release and Debug")
#set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Where install")

set(MIFSA_INTERNAL_PROJECT ON)
include(cmake/base.cmake)

mifsa_module_begin(base)

file(GLOB_RECURSE
    MIFSA_BASE_SOURCES
    ${PROJECT_SOURCE_DIR}/include/*.*
    ${PROJECT_SOURCE_DIR}/src/utils/*.*
    ${PROJECT_SOURCE_DIR}/src/base/*.*
    ${PROJECT_SOURCE_DIR}/src/hpplib/*.*
    )
add_library(
    ${PROJECT_NAME}
    ${MIFSA_BASE_SOURCES}
    )
find_package(Threads REQUIRED)
target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    )
find_package(automotive-dlt QUIET)
if(automotive-dlt_FOUND)
    option(MIFSA_DLT"MIFSA use dlt-daemon" ON)
    if(MIFSA_DLT)
        message("** Use dlt")
        target_link_libraries(
            ${PROJECT_NAME}
            PRIVATE
            Genivi::dlt
            )
        target_compile_definitions(
            ${PROJECT_NAME}
            PRIVATE
            "-DMIFSA_SUPPORT_DLT"
            )
    endif()
endif()

mifsa_module_end()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/examples AND MIFSA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/tests AND MIFSA_BUILD_TESTS)
    add_subdirectory(tests)
endif()
