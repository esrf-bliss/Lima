############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2017
# European Synchrotron Radiation Facility
# BP 220, Grenoble 38043
# FRANCE
#
# Contact: lima@esrf.fr
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################

cmake_minimum_required(VERSION 3.1)

project(bitshuffle)

find_package(HDF5 REQUIRED)

set(BS_VERSION_MAJOR "0")
set(BS_VERSION_MINOR "3")
set(BS_VERSION_POINT "4")

set(BITSHUFFLE_DIR ../../third-party/bitshuffle)

set(bitshuffle_src
  ${BITSHUFFLE_DIR}/src/bshuf_h5plugin.c
  ${BITSHUFFLE_DIR}/src/bshuf_h5filter.c
  ${BITSHUFFLE_DIR}/src/bitshuffle.c
  ${BITSHUFFLE_DIR}/src/bitshuffle_core.c
  ${BITSHUFFLE_DIR}/src/iochain.c
  ${BITSHUFFLE_DIR}/lz4/lz4.c)

set(bitshuffle_inc
  ${BITSHUFFLE_DIR}/src/bitshuffle.h
  ${BITSHUFFLE_DIR}/src/bitshuffle_core.h)

# this is the "object library" target: compiles the sources only once
add_library(h5bshuf_objects OBJECT ${bitshuffle_src})

target_include_directories(h5bshuf_objects PRIVATE "${BITSHUFFLE_DIR}/src")
target_include_directories(h5bshuf_objects PRIVATE "${BITSHUFFLE_DIR}/lz4")
target_include_directories(h5bshuf_objects PRIVATE "${HDF5_INCLUDE_DIRS}")

target_compile_definitions(h5bshuf_objects PRIVATE
  -DBSHUF_VERSION_MAJOR=${BS_VERSION_MAJOR}
  -DBSHUF_VERSION_MINOR=${BS_VERSION_MINOR}
  -DBSHUF_VERSION_POINT=${BS_VERSION_POINT}
)

target_compile_definitions(h5bshuf_objects PRIVATE ${HDF5_DEFINITIONS})

# shared libraries need PIC
set_property(TARGET h5bshuf_objects PROPERTY POSITION_INDEPENDENT_CODE 1)

# shared and static libraries built from the same object files
add_library(h5bshuf SHARED $<TARGET_OBJECTS:h5bshuf_objects>)
add_library(h5bshuf_static STATIC $<TARGET_OBJECTS:h5bshuf_objects>)

target_include_directories(h5bshuf PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${BITSHUFFLE_DIR}/src>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(h5bshuf PRIVATE ${HDF5_LIBRARIES})
target_link_libraries(h5bshuf_static PRIVATE ${HDF5_LIBRARIES})

install(
  TARGETS h5bshuf
  EXPORT "${TARGETS_EXPORT_NAME}"
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}   # .so files are libraries
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}   # .dll files are binaries
)

install(FILES ${bitshuffle_inc} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
