# SPDX-License-Identifier: Apache-2.0
#
# Copyright (c) Centro de Inovacao EDGE

zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)

file(GLOB_RECURSE ZBOOK_INTERFACE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
target_sources(app PRIVATE ${ZBOOK_INTERFACE_SOURCES})
