# SPDX-License-Identifier: Apache-2.0
#
# Copyright (c) Centro de Inovacao EDGE
#
# Shared boilerplate for interface unit tests (tests/unit/<category>/<peripheral>/).
# cmake_minimum_required()/find_package()/project() MUST stay literal in the
# test's own CMakeLists.txt -- CMake scans the top-level list file for those
# textually before running anything, so it can't see them through an
# include()+macro indirection. Call zbook_unit_test() after project() to
# wire up sources:
#
#   cmake_minimum_required(VERSION 3.20.0)
#   find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
#   project(test_zbook_spi)
#
#   include(${CMAKE_CURRENT_SOURCE_DIR}/../../unit.cmake)
#   zbook_unit_test()
macro(zbook_unit_test)
	target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c)
endmacro()
