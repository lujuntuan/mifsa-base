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

macro(mifsa_module_begin _type)
    string(TOLOWER ${_type} SUB_MODULE_TYPE)
    string(REGEX REPLACE ".*/(.*)" "\\1" SUB_MODULE_DIR_NAME ${CMAKE_CURRENT_LIST_DIR})
    if(${SUB_MODULE_TYPE} MATCHES "base")
        set(SUB_PROJECT_NAME "")
    elseif(${SUB_MODULE_TYPE} MATCHES "server")
        set(SUB_PROJECT_NAME mifsa_${MIFSA_MODULE_TYPE}_${SUB_MODULE_TYPE})
    elseif(${SUB_MODULE_TYPE} MATCHES "client")
        set(SUB_PROJECT_NAME mifsa_${MIFSA_MODULE_TYPE})
        if(NOT "${ARGV1}" STREQUAL "")
            set(SUB_PROJECT_NAME ${SUB_PROJECT_NAME}_${ARGV1})
        endif()
    elseif(${SUB_MODULE_TYPE} MATCHES "idl")
        if(NOT ${SUB_MODULE_DIR_NAME} MATCHES ${MIFSA_IDL_TYPE})
            return()
        endif()
        set(SUB_PROJECT_NAME mifsa_${MIFSA_MODULE_TYPE}_${SUB_MODULE_TYPE})
    elseif(${SUB_MODULE_TYPE} MATCHES "platform")
        set(SUB_PROJECT_NAME mifsa_${MIFSA_MODULE_TYPE}_${SUB_MODULE_TYPE}_${SUB_MODULE_DIR_NAME})
        if(NOT "${ARGV1}" STREQUAL "")
            set(SUB_PROJECT_NAME mifsa_${MIFSA_MODULE_TYPE}_${SUB_MODULE_TYPE}_${ARGV1})
        endif()
    else()
        message(FATAL_ERROR "mifsa_module_begin not support ${SUB_MODULE_TYPE}!")
    endif()
     if("${SUB_PROJECT_NAME}" STREQUAL "")
         set(SUB_PROJECT_NAME mifsa_base)
     else()
         if(MIFSA_MODULE_VERSION)
             project(${SUB_PROJECT_NAME} VERSION ${MIFSA_MODULE_VERSION} LANGUAGES C CXX)
         else()
             project(${SUB_PROJECT_NAME} LANGUAGES C CXX)
         endif()
     endif()
endmacro()

macro(mifsa_module_end)
    if(${SUB_MODULE_TYPE} MATCHES "base")
        if(EXISTS ${PROJECT_SOURCE_DIR}/config.h.in)
            configure_file(
                ${PROJECT_SOURCE_DIR}/config.h.in
                ${COMMON_INCLUDE_OUTPUT_PATH}/mifsa/${MIFSA_MODULE_TYPE}/config.h
                )
            install(
                FILES
                ${COMMON_INCLUDE_OUTPUT_PATH}/mifsa/${MIFSA_MODULE_TYPE}/config.h
                DESTINATION
                ${CMAKE_INSTALL_INCLUDEDIR}/mifsa/${MIFSA_MODULE_TYPE}
                )
        endif()
        if(EXISTS ${PROJECT_SOURCE_DIR}/config.cmake.in)
            configure_file(
                ${PROJECT_SOURCE_DIR}/config.cmake.in
                ${COMMON_CMAKE_OUTPUT_PATH}/${PROJECT_NAME}/${PROJECT_NAME}-config.cmake
                @ONLY
                )
            install(
                DIRECTORY
                ${COMMON_CMAKE_OUTPUT_PATH}/${SUB_PROJECT_NAME}/
                DESTINATION
                ${CMAKE_INSTALL_LIBDIR}/cmake/${SUB_PROJECT_NAME}
                )
        endif()
        if(EXISTS ${PROJECT_SOURCE_DIR}/cmake)
            install(
                DIRECTORY
                ${PROJECT_SOURCE_DIR}/cmake/
                DESTINATION
                ${CMAKE_INSTALL_LIBDIR}/cmake/${SUB_PROJECT_NAME}
                )
        endif()
        set_target_properties(
            ${SUB_PROJECT_NAME}
            PROPERTIES
            VERSION
            ${PROJECT_VERSION}
            )
        target_compile_definitions(
            ${SUB_PROJECT_NAME}
            PRIVATE
            ${MIFSA_LIBRARY_DEF}
            )
        install(
            TARGETS
            ${SUB_PROJECT_NAME}
            EXPORT
            ${SUB_PROJECT_NAME}-targets
            RUNTIME
            DESTINATION
            ${CMAKE_INSTALL_BINDIR}
            ARCHIVE
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
            LIBRARY
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
            )
        install(
            EXPORT
            ${SUB_PROJECT_NAME}-targets
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/${SUB_PROJECT_NAME}
        )
    elseif(${SUB_MODULE_TYPE} MATCHES "client")
        if(EXISTS ${PROJECT_SOURCE_DIR}/../include)
            install(
                DIRECTORY
                ${PROJECT_SOURCE_DIR}/../include/
                DESTINATION
                ${CMAKE_INSTALL_INCLUDEDIR}
                )
        endif()
        if(EXISTS ${PROJECT_SOURCE_DIR}/../config.h.in)
            configure_file(
                ${PROJECT_SOURCE_DIR}/../config.h.in
                ${COMMON_INCLUDE_OUTPUT_PATH}/mifsa/${MIFSA_MODULE_TYPE}/config.h
                )
            install(
                FILES
                ${COMMON_INCLUDE_OUTPUT_PATH}/mifsa/${MIFSA_MODULE_TYPE}/config.h
                DESTINATION
                ${CMAKE_INSTALL_INCLUDEDIR}/mifsa/${MIFSA_MODULE_TYPE}
                )
        endif()
        if(EXISTS ${PROJECT_SOURCE_DIR}/../config.cmake.in)
            configure_file(
                ${PROJECT_SOURCE_DIR}/../config.cmake.in
                ${COMMON_CMAKE_OUTPUT_PATH}/${PROJECT_NAME}/${PROJECT_NAME}-config.cmake
                @ONLY
                )
            install(
                DIRECTORY
                ${COMMON_CMAKE_OUTPUT_PATH}/${SUB_PROJECT_NAME}/
                DESTINATION
                ${CMAKE_INSTALL_LIBDIR}/cmake/${SUB_PROJECT_NAME}
                )
        endif()
        if(EXISTS ${PROJECT_SOURCE_DIR}/../src)
            file(GLOB_RECURSE
                MIFSA_COMMON_SOURCES
                ${PROJECT_SOURCE_DIR}/../src/*.*
                )
            target_include_directories(
                ${SUB_PROJECT_NAME}
                PRIVATE
                ${PROJECT_SOURCE_DIR}/../src
                )
            target_sources(
                ${SUB_PROJECT_NAME}
                PRIVATE
                ${MIFSA_COMMON_SOURCES}
                )
        endif()
        target_link_libraries(
            ${SUB_PROJECT_NAME}
            PUBLIC
            mifsa_base
         )
         link_idl_library(${SUB_PROJECT_NAME})
        target_include_directories(
            ${SUB_PROJECT_NAME}
            PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            )
        set_target_properties(
            ${SUB_PROJECT_NAME}
            PROPERTIES
            VERSION
            ${PROJECT_VERSION}
            )
        target_compile_definitions(
            ${SUB_PROJECT_NAME}
            PRIVATE
            ${MIFSA_LIBRARY_DEF}
            "-DMIFSA_CLIENT_TYPE"
            PUBLIC
            ${MIFSA_IDL_DEF}
            )
        install(
            TARGETS
            ${SUB_PROJECT_NAME}
            EXPORT
            ${SUB_PROJECT_NAME}-config
            RUNTIME
            DESTINATION
            ${CMAKE_INSTALL_BINDIR}
            ARCHIVE
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
            LIBRARY
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
            )
        install(
            EXPORT
            ${SUB_PROJECT_NAME}-config
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/${SUB_PROJECT_NAME}
        )
    elseif(${SUB_MODULE_TYPE} MATCHES "server")
        if(EXISTS ${PROJECT_SOURCE_DIR}/../config.h.in)
            configure_file(
                ${PROJECT_SOURCE_DIR}/../config.h.in
                ${COMMON_INCLUDE_OUTPUT_PATH}/mifsa/${MIFSA_MODULE_TYPE}/config.h
                )
        endif()
        if(EXISTS ${PROJECT_SOURCE_DIR}/../src)
            file(GLOB_RECURSE
                MIFSA_COMMON_SOURCES
                ${PROJECT_SOURCE_DIR}/../src/*.*
                )
            target_include_directories(
                ${SUB_PROJECT_NAME}
                PRIVATE
                ${PROJECT_SOURCE_DIR}/../src
                )
            target_sources(
                ${SUB_PROJECT_NAME}
                PRIVATE
                ${MIFSA_COMMON_SOURCES}
                )
        endif()
        target_link_libraries(
            ${SUB_PROJECT_NAME}
            PUBLIC
            mifsa_base
         )
        if(MIFSA_SYSTEMD AND EXISTS ${PROJECT_SOURCE_DIR}/${SUB_PROJECT_NAME}.service.in)
            target_compile_definitions(
                ${SUB_PROJECT_NAME}
                PUBLIC
                "-DMIFSA_SUPPORT_SYSTEMD"
                )
            target_link_libraries(
                ${SUB_PROJECT_NAME}
                PRIVATE
                systemd
             )
            if(${MIFSA_IDL_TYPE} MATCHES "fdbus")
                set(SUB_SYSTEMD_TARGET "fdbus_name_server.service")
            endif()
            configure_file(
                ${PROJECT_SOURCE_DIR}/${SUB_PROJECT_NAME}.service.in
                ${COMMON_SYSTEMD_OUTPUT_PATH}/${SUB_PROJECT_NAME}.service
                @ONLY
             )
            install(
                FILES
                ${COMMON_SYSTEMD_OUTPUT_PATH}/${SUB_PROJECT_NAME}.service
                DESTINATION
                ${CMAKE_INSTALL_LIBDIR}/systemd/system
             )
        endif()
        link_idl_library(${SUB_PROJECT_NAME})
        target_include_directories(
            ${SUB_PROJECT_NAME}
            PRIVATE
            ${PROJECT_SOURCE_DIR}/../include
            )
        target_compile_definitions(
            ${SUB_PROJECT_NAME}
            PRIVATE
            ${MIFSA_LIBRARY_APPDEF}
            "-DMIFSA_SERVER_TYPE"
            PUBLIC
            ${MIFSA_IDL_DEF}
            )
        install(
            TARGETS
            ${SUB_PROJECT_NAME}
            RUNTIME
            DESTINATION
            ${CMAKE_INSTALL_BINDIR}
            ARCHIVE
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
            LIBRARY
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
            )
    elseif(${SUB_MODULE_TYPE} MATCHES "platform")
        target_link_libraries(
            ${SUB_PROJECT_NAME}
            PUBLIC
            mifsa_base
            )
        target_include_directories(
            ${SUB_PROJECT_NAME}
            PRIVATE
            ${PROJECT_SOURCE_DIR}/../../include
            )
        install(
            TARGETS
            ${SUB_PROJECT_NAME}
            RUNTIME
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/mifsa-plugins
            ARCHIVE
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/mifsa-plugins
            LIBRARY
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/mifsa-plugins
            )
        set_target_properties(
            ${SUB_PROJECT_NAME}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mifsa-plugins
        )
        set_target_properties(
            ${SUB_PROJECT_NAME}
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY
            ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/mifsa-plugins
        )
        set_target_properties(
            ${SUB_PROJECT_NAME}
            PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/mifsa-plugins
        )
        set_target_properties(
            ${SUB_PROJECT_NAME}
            PROPERTIES
            PREFIX
            ""
        )
        target_compile_definitions(
            ${SUB_PROJECT_NAME}
            PRIVATE
            ${MIFSA_LIBRARY_DEF}
            "-DMIFSA_PLATFORM_TYPE"
            )
    elseif(${SUB_MODULE_TYPE} MATCHES "idl")
        mifsa_install_etc(${SUB_PROJECT_NAME} ${PROJECT_SOURCE_DIR}/..)
        mifsa_install_share(${SUB_PROJECT_NAME} ${PROJECT_SOURCE_DIR}/..)
        if(${MIFSA_IDL_TYPE} MATCHES "ros")
            set(SUB_PROJECT_NAME ${MIFSA_ROS_IDL_TYPE})
        elseif(${MIFSA_IDL_TYPE} MATCHES "vsomeip")
            target_link_libraries(
                ${SUB_PROJECT_NAME}
                PUBLIC
                CommonAPI
                CommonAPI-SomeIP
                )
            target_include_directories(
                ${SUB_PROJECT_NAME}
                PUBLIC
                $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                )
            install(
                TARGETS
                ${SUB_PROJECT_NAME}
                EXPORT
                ${SUB_PROJECT_NAME}-config
                RUNTIME
                DESTINATION
                ${CMAKE_INSTALL_BINDIR}
                ARCHIVE
                DESTINATION
                ${CMAKE_INSTALL_LIBDIR}
                LIBRARY
                DESTINATION
                ${CMAKE_INSTALL_LIBDIR}
                )
            install(
                EXPORT
                ${SUB_PROJECT_NAME}-config
                DESTINATION
                ${CMAKE_INSTALL_LIBDIR}/cmake/${SUB_PROJECT_NAME}
            )
            install(
                DIRECTORY
                ${PROJECT_BINARY_DIR}/include/
                DESTINATION
                ${CMAKE_INSTALL_INCLUDEDIR}
                PATTERN "*.c" EXCLUDE
                PATTERN "*.cc" EXCLUDE
                PATTERN "*.cpp" EXCLUDE
                )
        elseif(${MIFSA_IDL_TYPE} MATCHES "fdbus")
            target_link_libraries(
                ${SUB_PROJECT_NAME}
                PUBLIC
                ${FDBUS_LIBRARY}
                ${PROTOBUF_LIBRARY}
                )
            target_include_directories(
                ${SUB_PROJECT_NAME}
                PUBLIC
                $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                ${FDBUS_INCLUDE_DIR}
                ${PROTOBUF_INCLUDE_DIR}
                )
            install(
                TARGETS
                ${SUB_PROJECT_NAME}
                EXPORT
                ${SUB_PROJECT_NAME}-config
                RUNTIME
                DESTINATION
                ${CMAKE_INSTALL_BINDIR}
                ARCHIVE
                DESTINATION
                ${CMAKE_INSTALL_LIBDIR}
                LIBRARY
                DESTINATION
                ${CMAKE_INSTALL_LIBDIR}
                )
            install(
                EXPORT
                ${SUB_PROJECT_NAME}-config
                DESTINATION
                ${CMAKE_INSTALL_LIBDIR}/cmake/${SUB_PROJECT_NAME}
            )
            install(
                DIRECTORY
                ${PROJECT_BINARY_DIR}/include/
                DESTINATION
                ${CMAKE_INSTALL_INCLUDEDIR}
                PATTERN "*.c" EXCLUDE
                PATTERN "*.cc" EXCLUDE
                PATTERN "*.cpp" EXCLUDE
                )
        endif()
        target_compile_definitions(
            ${SUB_PROJECT_NAME}
            PRIVATE
            "-DMIFSA_IDL_TYPE"
            )
    endif()
    ##
    if(EXISTS ${PROJECT_SOURCE_DIR}/src)
        target_include_directories(
            ${SUB_PROJECT_NAME}
            PRIVATE
            ${PROJECT_SOURCE_DIR}/src
            )
    else()
        target_include_directories(
            ${SUB_PROJECT_NAME}
            PRIVATE
            ${PROJECT_SOURCE_DIR}>
            )
    endif()
    target_include_directories(
        ${SUB_PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${COMMON_INCLUDE_OUTPUT_PATH}>
        )
    if(EXISTS ${PROJECT_SOURCE_DIR}/include)
        target_include_directories(
            ${SUB_PROJECT_NAME}
            PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            )
        install(
            DIRECTORY
            ${PROJECT_SOURCE_DIR}/include/
            DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
            )
    endif()
    if(EXISTS ${PROJECT_SOURCE_DIR}/${SUB_PROJECT_NAME}.pc.in)
        configure_file(
            ${SUB_PROJECT_NAME}.pc.in
            ${COMMON_PKG_OUTPUT_PATH}/${SUB_PROJECT_NAME}.pc
            @ONLY
            )
        install(
            FILES
            ${COMMON_PKG_OUTPUT_PATH}/${SUB_PROJECT_NAME}.pc
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/pkgconfig
            )
    endif()
    mifsa_install_etc(${SUB_PROJECT_NAME} ${PROJECT_SOURCE_DIR})
    mifsa_install_share(${SUB_PROJECT_NAME} ${PROJECT_SOURCE_DIR})
    unset(SUB_PROJECT_NAME)
    unset(SUB_MODULE_DIR_NAME)
    unset(SUB_MODULE_TYPE)
    unset(SUB_MODULE_NAME)
endmacro()
