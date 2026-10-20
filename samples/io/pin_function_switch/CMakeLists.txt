# SPDX-License-Identifier: Apache-2.0
#
# Copyright (c) Centro de Inovacao EDGE

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sample_zbook_pin_function_switch)

# Mocks stand in for the zbook_io_* backends: remove them once the real ones are built.
target_sources(app PRIVATE src/main.c ../mocks/zbook_io_mocks.c)
