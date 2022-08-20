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
project(examples)

link_libraries(
    mifsa_base
    )
add_executable(
    mifsa_base_example_application
    example_application.cpp
    )
add_executable(
    mifsa_base_example_queue
    example_queue.cpp
    )
add_executable(
    mifsa_base_example_semaphone
    example_semaphone.cpp
    )
add_executable(
    mifsa_base_example_thread
    example_thread.cpp
    )
add_executable(
    mifsa_base_example_timer
    example_timer.cpp
    )
add_executable(
    mifsa_base_example_pool
    example_pool.cpp
    )
add_executable(
    mifsa_base_example_variant
    example_variant.cpp
    )
install(
    TARGETS
    mifsa_base_example_application
    mifsa_base_example_queue
    mifsa_base_example_semaphone
    mifsa_base_example_thread
    mifsa_base_example_timer
    mifsa_base_example_pool
    mifsa_base_example_variant
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
