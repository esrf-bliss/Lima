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

if (NOT BITSHUFFLE_EXTERNALLY_CONFIGURED)
  cmake_minimum_required(VERSION 2.8)

  find_library(LIB_HDF5 hdf5)
  find_path(LIB_HDF5_INCLUDE_DIR hdf5.h)

endif (NOT BITSHUFFLE_EXTERNALLY_CONFIGURED)

set(BS_VERSION_MAJOR "0")
set(BS_VERSION_MINOR "3")
set(BS_VERSION_POINT "4")

add_definitions(-DBSHUF_VERSION_MAJOR=${BS_VERSION_MAJOR} 
                -DBSHUF_VERSION_MINOR=${BS_VERSION_MINOR} 
                -DBSHUF_VERSION_POINT=${BS_VERSION_POINT}
                -fPIC -g -O3 -ffast-math -march=native -std=c99 -fno-strict-aliasing)

set(BITSHUFFLE_DIR ../../third-party/bitshuffle)
set(bitshuffle_src ${BITSHUFFLE_DIR}/src/bshuf_h5plugin.c
    ${BITSHUFFLE_DIR}/src/bshuf_h5filter.c
    ${BITSHUFFLE_DIR}/src/bitshuffle.c
    ${BITSHUFFLE_DIR}/src/bitshuffle_core.c
    ${BITSHUFFLE_DIR}/src/iochain.c
    ${BITSHUFFLE_DIR}/lz4/lz4.c)

add_library(h5bshuf SHARED ${bitshuffle_src})

target_include_directories(h5bshuf PRIVATE "${BITSHUFFLE_DIR}/src")
target_include_directories(h5bshuf PRIVATE "${BITSHUFFLE_DIR}/lz4")
target_include_directories(h5bshuf PRIVATE "${LIB_HDF5_INCLUDE_DIR}")

target_link_libraries(h5bshuf ${LIB_HDF5})
include(GNUInstallDirs)
install(TARGETS h5bshuf LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR})

