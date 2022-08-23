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

function(mifsa_get_git_commit _git_hash)
    set(_commit_id"unknown")
    find_package(Git QUIET)
    if(GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%H
            OUTPUT_VARIABLE _commit_id
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            WORKING_DIRECTORY
            ${PROJECT_SOURCE_DIR}
            )
    endif()
    set(${_git_hash} ${_commit_id} PARENT_SCOPE)
endfunction()

function(mifsa_install_etc _target _dir)
    if(NOT EXISTS ${_dir}/etc)
        message(${_dir}/etc)
        return()
    endif()
    if(TARGET ${_target})
        add_custom_command(
            TARGET
            ${_target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${_dir}/etc/ ${COMMON_ETC_OUTPUT_PATH}
            )
    else()
        add_custom_target(
            ${_target}
            ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${_dir}/etc/ ${COMMON_ETC_OUTPUT_PATH}
            )
    endif()
    get_filename_component(_PREFIX_RELATIVE_PATH ${CMAKE_INSTALL_PREFIX} REALPATH)
    if(${_PREFIX_RELATIVE_PATH} MATCHES "^/usr")
        #use root
        install(
            DIRECTORY
            ${_dir}/etc/
            DESTINATION
            /etc
            )
    else()
        install(
            DIRECTORY
            ${_dir}/etc/
            DESTINATION
            etc
            )
    endif()
    unset(_PREFIX_RELATIVE_PATH)
endfunction()

function(mifsa_install_share _target _dir)
    if(NOT EXISTS ${_dir}/share)
        return()
    endif()
    if(TARGET ${_target})
        add_custom_command(
            TARGET
            ${_target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${_dir}/share/ ${COMMON_SHARE_OUTPUT_PATH}
            )
    else()
        add_custom_target(
            ${_target}
            ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${_dir}/share/ ${COMMON_SHARE_OUTPUT_PATH}
            )
    endif()
    install(
        DIRECTORY
        ${_dir}/share/
        DESTINATION
        share/${CMAKE_PROJECT_NAME}
        )
    unset(_PREFIX_RELATIVE_PATH)
endfunction()
