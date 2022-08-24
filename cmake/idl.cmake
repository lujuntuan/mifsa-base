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

if(${MIFSA_IDL_TYPE} MATCHES "auto")
    set(MIFSA_IDL_DETECTED_STR " (For auto detected )")
    if(ROS_ENABLE)
        set(MIFSA_IDL_TYPE ros)
    elseif(VSOMEIP_ENABLE)
        set(MIFSA_IDL_TYPE vsomeip)
    elseif(FDBUS_ENABLE)
        set(MIFSA_IDL_TYPE fdbus)
    else()
        set(MIFSA_IDL_TYPE unknown)
    endif()
endif()

string(TOLOWER ${MIFSA_IDL_TYPE} MIFSA_IDL_TYPE)
set(MIFSA_IDL_SUPPORT ON)
if(${MIFSA_IDL_TYPE} MATCHES "ros")
    set(MIFSA_IDL_DEF "-DMIFSA_SUPPORT_ROS")
    message("** Use ros idl${MIFSA_IDL_DETECTED_STR}")
elseif(${MIFSA_IDL_TYPE} MATCHES "vsomeip")
    set(MIFSA_IDL_DEF "-DMIFSA_SUPPORT_VSOMEIP")
    message("** Use vsomeip idl${MIFSA_IDL_DETECTED_STR}")
elseif(${MIFSA_IDL_TYPE} MATCHES "fdbus")
    set(MIFSA_IDL_DEF "-DMIFSA_SUPPORT_FDBUS")
    message("** Use fdbus idl${MIFSA_IDL_DETECTED_STR}")
    if(MSVC)
        foreach(var
                CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
                )
            if(${var} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${var} "${${var}}")
            endif()
        endforeach()
    endif()
else()
    set(MIFSA_IDL_SUPPORT OFF)
    message(WARNING "No idl support!")
endif()

function(ros_idl_add_library _target)
    find_package(${_target} QUIET)
    if(NOT ${_target}_FOUND)
        find_package(rosidl_default_generators REQUIRED)
        file(GLOB_RECURSE
            ROS_IDL_SRC
            RELATIVE
            ${CMAKE_CURRENT_LIST_DIR}
            msg/*
            srv/*
            )
        rosidl_generate_interfaces(
            ${_target}
            ${ROS_IDL_SRC}
            )
        ament_package()
    endif()
    rosidl_get_typesupport_target(
        ROS_IDL_TYPE
        ${_target}
        rosidl_typesupport_cpp
    )
    set(MIFSA_ROS_IDL_TYPE ${ROS_IDL_TYPE} PARENT_SCOPE)
endfunction()

function(vsomeip_idl_add_library _target)
    if(MSVC)
        set(VSOMEIP_IDL_LIBRARY_TYPE STATIC)
    else()
        set(VSOMEIP_IDL_LIBRARY_TYPE SHARED)
    endif()
    commonapi_core_generate_src(
        ${PROJECT_BINARY_DIR}/include
        "v1/mifsa_${MIFSA_MODULE_TYPE}_idl/Common"
        COMMONAPI_CORE_IDL_SRC
        ${CMAKE_CURRENT_LIST_DIR}/*.fidl
        )
    commonapi_someip_generate_src(
        ${PROJECT_BINARY_DIR}/include
        "v1/mifsa_${MIFSA_MODULE_TYPE}_idl/Common"
        COMMONAPI_SOMEIP_IDL_SRC
        ${CMAKE_CURRENT_LIST_DIR}/*.fdepl
        )
    add_library(
        ${_target}
        ${VSOMEIP_IDL_LIBRARY_TYPE}
        ${COMMONAPI_CORE_IDL_SRC}
        ${COMMONAPI_SOMEIP_IDL_SRC}
        )
endfunction()

function(fdbus_idl_add_library _target)
    if(MSVC)
        set(FDBUS_IDL_LIBRARY_TYPE STATIC)
    else()
        set(FDBUS_IDL_LIBRARY_TYPE SHARED)
    endif()
    proto_generate_src(
        ${PROJECT_BINARY_DIR}/include/mifsa/${MIFSA_MODULE_TYPE}/idl/fdbus
        PROTO_IDL_SRC
        ${CMAKE_CURRENT_LIST_DIR}/*.proto
        )
    add_library(
        ${_target}
        ${FDBUS_IDL_LIBRARY_TYPE}
        ${PROTO_IDL_SRC}
        )
    if(MSVC)
        target_link_libraries(
            ${PROJECT_NAME}
            PRIVATE
            ws2_32
            )
    endif()
endfunction()

function(link_idl_library _target)
    set(ROS_IDL_NAME mifsa_${MIFSA_MODULE_TYPE}_idl)
    if(${MIFSA_IDL_TYPE} MATCHES "ros")
        find_package(${ROS_IDL_NAME} QUIET)
        rosidl_get_typesupport_target(
            ROS_IDL_TYPE
            ${ROS_IDL_NAME}
            rosidl_typesupport_cpp
        )
        target_link_libraries(
            ${_target}
            PRIVATE
            rclcpp::rclcpp
            ${ROS_IDL_TYPE}
        )
    elseif(${MIFSA_IDL_TYPE} MATCHES "vsomeip")
        target_link_libraries(
            ${_target}
            PRIVATE
            ${ROS_IDL_NAME}
         )
    elseif(${MIFSA_IDL_TYPE} MATCHES "fdbus")
        target_link_libraries(
            ${_target}
            PRIVATE
            ${ROS_IDL_NAME}
        )
        if(FDBUS_NEW_INTERFACE)
            target_compile_definitions(
                ${SUB_PROJECT_NAME}
                PRIVATE
                "-DFDBUS_NEW_INTERFACE"
            )
    endif()
    endif()
endfunction()
