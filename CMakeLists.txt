# SPDX-License-Identifier: MIT
# Copyright (c) 2022 Falk Werner

cmake_minimum_required (VERSION 3.10)
project(hashmap VERSION 2.0.0)


option(WITHOUT_TESTS   "disable unit tests"   OFF)


set(CMAKE_C_STANDARD 99)


add_library(hmap STATIC src/hmap/hmap.c)
target_include_directories(hmap PUBLIC include)

file(WRITE "${PROJECT_BINARY_DIR}/hmap.pc"
"prefix=\"${CMAKE_INSTALL_PREFIX}\"
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib${LIB_SUFFIX}
includedir=\${prefix}/include
Name: hmap
Description: General purpose hash map
Version: ${PROJECT_VERSION}
Libs: -L\${libdir} -lhmap
Cflags: -I\${includedir}"
)


install(TARGETS hmap LIBRARY DESTINATION lib)
install(DIRECTORY include DESTINATION include)
install(FILES "${PROJECT_BINARY_DIR}/hmap.pc" DESTINATION lib${LIB_SUFFIX}/pkgconfig)


if(NOT WITHOUT_TESTS)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTEST REQUIRED gtest_main)

add_executable(alltests test-src/test_hmap.cpp)
target_include_directories(alltests PUBLIC ${GTEST_INCLUDE_DIRS})
target_link_libraries(alltests PUBLIC hmap ${GTEST_LIBRARIES})


endif(NOT WITHOUT_TESTS)
