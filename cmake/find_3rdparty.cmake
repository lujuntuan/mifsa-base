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

find_path(MIFSA_3RDPARTY_DIR
    NAMES
    lib
    PATHS
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/../
    PATH_SUFFIXES
    3rd
    3RD
    3rdparty
    3RDPARTY
    dependence
    DEPENDENCE
    NO_CMAKE_FIND_ROOT_PATH
    NO_DEFAULT_PATH
    )

if(MIFSA_3RDPARTY_DIR)
    set(MIFSA_3RDPARTY_DIR_ENABLE ON)
    message(STATUS "3rdparty_dir found")
    message(STATUS "MIFSA_3RDPARTY_DIR=${MIFSA_3RDPARTY_DIR}")
else()
    set(MIFSA_3RDPARTY_DIR_ENABLE OFF)
    message(STATUS "3rdparty_dir not found")
endif()
macro(search_3rdparty_begin arg)
    set(MIFSA_MIFSA_3RD_INC_SEARCH_PATH
        ${CMAKE_INSTALL_PREFIX}}/include
        ${CMAKE_INSTALL_PREFIX}/${arg}/include
        )
    set(MIFSA_MIFSA_3RD_LIB_SEARCH_PATH
        ${CMAKE_INSTALL_PREFIX}}/lib
        ${CMAKE_INSTALL_PREFIX}/${arg}/lib
        ${CMAKE_INSTALL_PREFIX}}/lib/${CMAKE_BUILD_TYPE}
        ${CMAKE_INSTALL_PREFIX}/${arg}/lib/${CMAKE_BUILD_TYPE}
        )
    set(MIFSA_MIFSA_3RD_EXE_SEARCH_PATH
        ${CMAKE_INSTALL_PREFIX}}/bin
        ${CMAKE_INSTALL_PREFIX}/${arg}/bin
        ${CMAKE_INSTALL_PREFIX}}/bin/${CMAKE_BUILD_TYPE}
        ${CMAKE_INSTALL_PREFIX}/${arg}/bin/${CMAKE_BUILD_TYPE}
        )
    if(WIN32)
        list(APPEND
            MIFSA_MIFSA_3RD_INC_SEARCH_PATH
            $ENV{programfiles}/${arg}/include
            $ENV{programfiles\(x86\)}/${arg}/include
            )
        list(APPEND
            MIFSA_MIFSA_3RD_LIB_SEARCH_PATH
            $ENV{programfiles}/${arg}/lib
            $ENV{programfiles\(x86\)}/${arg}/lib
            $ENV{programfiles}/${arg}/lib/${CMAKE_BUILD_TYPE}
            $ENV{programfiles\(x86\)}/${arg}/lib/${CMAKE_BUILD_TYPE}
            )
        list(APPEND
            MIFSA_MIFSA_3RD_EXE_SEARCH_PATH
            $ENV{programfiles}/${arg}/bin
            $ENV{programfiles\(x86\)}/${arg}/bin
            $ENV{programfiles}/${arg}/bin/${CMAKE_BUILD_TYPE}
            $ENV{programfiles\(x86\)}/${arg}/bin/${CMAKE_BUILD_TYPE}
            )
    endif()
    if(MIFSA_3RDPARTY_DIR_ENABLE)
        list(APPEND
            MIFSA_MIFSA_3RD_INC_SEARCH_PATH
            ${MIFSA_3RDPARTY_DIR}/include
            ${MIFSA_3RDPARTY_DIR}/${arg}/include
            )
        list(APPEND
            MIFSA_MIFSA_3RD_LIB_SEARCH_PATH
            ${MIFSA_3RDPARTY_DIR}/lib
            ${MIFSA_3RDPARTY_DIR}/${arg}/lib
            ${MIFSA_3RDPARTY_DIR}/lib/${CMAKE_BUILD_TYPE}
            ${MIFSA_3RDPARTY_DIR}/${arg}/lib/${CMAKE_BUILD_TYPE}
            )
        list(APPEND
            MIFSA_MIFSA_3RD_EXE_SEARCH_PATH
            ${MIFSA_3RDPARTY_DIR}/bin
            ${MIFSA_3RDPARTY_DIR}/${arg}/bin
            ${MIFSA_3RDPARTY_DIR}/bin/${CMAKE_BUILD_TYPE}
            ${MIFSA_3RDPARTY_DIR}/${arg}/bin/${CMAKE_BUILD_TYPE}
            )
    else()
    endif()
endmacro(search_3rdparty_begin arg)
macro(search_3rdparty_end)
    unset(MIFSA_MIFSA_3RD_INC_SEARCH_PATH)
    unset(MIFSA_MIFSA_3RD_LIB_SEARCH_PATH)
    unset(MIFSA_MIFSA_3RD_EXE_SEARCH_PATH)
endmacro(search_3rdparty_end)
