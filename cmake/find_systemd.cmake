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

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_search_module(PC_SYSTEMD QUIET libsystemd)
endif()

search_3rdparty_begin(systemd)
find_path(SYSTEMD_INCLUDE_DIR
    NAMES
    systemd/sd-daemon.h
    PATHS
    ${PC_SYSTEMD_INCLUDE_DIRS}
    ${3RD_INC_SEARCH_PATH}
    )

find_library(SYSTEMD_LIBRARY
    NAMES
    systemd
    PATHS
    ${PC_SYSTEMD_LIBRARY_DIRS}
    ${3RD_LIB_SEARCH_PATH}
    )
search_3rdparty_end()
mark_as_advanced(SYSTEMD_INCLUDE_DIR SYSTEMD_LIBRARY)

if(SYSTEMD_INCLUDE_DIR AND SYSTEMD_LIBRARY)
    set(SYSTEMD_ENABLE ON)
    message(STATUS "systemd found")
    message(STATUS "SYSTEMD_INCLUDE_DIR=${SYSTEMD_INCLUDE_DIR}")
    message(STATUS "SYSTEMD_LIBRARY=${SYSTEMD_LIBRARY}")
    add_library(systemd SHARED IMPORTED)
    get_filename_component(SYSTEMD_LIBRARY_NAME ${SYSTEMD_LIBRARY} NAME)
    set_target_properties(
        systemd
        PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES
        ${SYSTEMD_INCLUDE_DIR}
        IMPORTED_LOCATION
        ${SYSTEMD_LIBRARY}
        IMPORTED_SONAME
        ${SYSTEMD_LIBRARY_NAME}
    )
else()
    set(SYSTEMD_ENABLE OFF)
    message(STATUS "systemd not found")
endif()
