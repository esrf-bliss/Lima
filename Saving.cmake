###########################################################################
# This file is part of LImA, a Library for Image Acquisition
#
#  Copyright (C) : 2009-2017
#  European Synchrotron Radiation Facility
#  BP 220, Grenoble 38043
#  FRANCE
#
#  Contact: lima@esrf.fr
#
#  This is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  This software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################

set(saving_definitions)
set(saving_libs)
set(saving_private_libs)
set(saving_includes)

if(LIMA_ENABLE_EDFGZ)
  find_package(ZLIB)
  if(${ZLIB_FOUND})
    list(APPEND saving_definitions -DWITH_Z_COMPRESSION)
    list(APPEND saving_libs ${ZLIB_LIBRARIES})
    list(APPEND saving_includes ${ZLIB_INCLUDE_DIRS})
    else()
    message(FATAL_ERROR "ZLIB library not found, please install or disable LIMA_ENABLE_EDFGZ")
  endif()
endif()

if(LIMA_ENABLE_EDFLZ4)
  find_package(LZ4)
  if (${LZ4_FOUND})
    list(APPEND saving_definitions -DWITH_LZ4_COMPRESSION)
    list(APPEND saving_libs ${LZ4_LIBRARIES})
  	list(APPEND saving_includes ${LZ4_INCLUDE_DIRS})
  else()
    message(FATAL_ERROR "LZ4 library: required version = 1.9.1, please update or switch off LIMA_ENABLE_EDFLZ4")
  endif()
endif()

if(LIMA_ENABLE_CBF)
  find_package(CBF)
  if (${CBF_FOUND})
    list(APPEND saving_definitions -DWITH_CBF_SAVING)
    list(APPEND saving_definitions -DPROTOTYPES)
    list(APPEND control_srcs control/src/CtSaving_Cbf.cpp)
    list(APPEND saving_libs ${CBF_LIBRARIES} crypto)
    list(APPEND saving_includes ${CBF_INCLUDE_DIRS})
  else()
    message(FATAL_ERROR "CBF library not found, please install or disable LIMA_ENABLE_CBF")
  endif()
endif()

if(LIMA_ENABLE_FITS)
  find_package(CCFits)
  if(${CCFITS_FOUND})
    list(APPEND saving_definitions -DWITH_FITS_SAVING)
    list(APPEND control_srcs control/src/CtSaving_Fits.cpp)
    list(APPEND saving_libs ${CFITSIO_LIBRARY} ${CCFITS_LIBRARY})
    list(APPEND saving_includes ${CFITSIO_INCLUDE_DIR})
  else()
    message(FATAL_ERROR "CFITSIO and/or CCFITS library not found, please install or disable LIMA_ENABLE_FITS")
  endif()
endif()

if(LIMA_ENABLE_HDF5)
  #set (HDF5_USE_STATIC_LIBRARIES ON)
  find_package(HDF5 COMPONENTS CXX HL)
  if(${HDF5_FOUND})
    list(APPEND saving_definitions -DWITH_HDF5_SAVING ${HDF5_DEFINITIONS})
    list(APPEND control_srcs control/src/CtSaving_Hdf5.cpp)
    list(APPEND saving_libs ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES} ${LIB_SZIP} ${LIB_ZLIB})
    list(APPEND saving_includes ${HDF5_INCLUDE_DIRS})
  else()
    message(FATAL_ERROR "HDF5 library not found, please install or disable LIMA_ENABLE_HDF5")
  endif()

  if(LIMA_ENABLE_HDF5_BS)
    #set(BITSHUFFLE_EXTERNALLY_CONFIGURED ON)
    add_subdirectory(control/bitshuffle)

    list(APPEND saving_definitions -DWITH_BS_COMPRESSION)
    list(APPEND saving_includes ${CMAKE_CURRENT_SOURCE_DIR}/third-party/bitshuffle/src)
    list(APPEND saving_private_libs h5bshuf_static)

    # set(LIB_BS_INCLUDE_DIR "/usr/local/include/" CACHE PATH "Path to BitShuffle include files")
    # link_directories(${LIB_HDF5_PLUGIN})
    # find_library(LIB_HDF5_BS h5bshuf ${LIB_HDF5_PLUGIN})
    # find_path(LIB_BS_INCLUDE_DIR bitshuffle.h)
    # list(APPEND saving_libs ${LIB_HDF5_BS})
    # list(APPEND saving_includes ${LIB_BS_INCLUDE_DIR})
  endif()
endif()

if(LIMA_ENABLE_NXS)
  find_package(NXS)
  if(${NXS_FOUND})
    list(APPEND saving_definitions -DWITH_NXS_SAVING)
    list(APPEND control_srcs control/src/CtSaving_NXS.cpp)
    list(APPEND saving_includes ${NXS_INCLUDE_DIRS})
    list(APPEND saving_libs ${NXS_LIBRARIES})
  else()
    message(FATAL_ERROR "NEXUS cpp library not installed, please install or disable LIMA_ENABLE_NXS")
  endif()
endif()

if(LIMA_ENABLE_TIFF)
  find_package(TIFF)
  if(${TIFF_FOUND})
    list(APPEND saving_definitions -DWITH_TIFF_SAVING)
    list(APPEND control_srcs control/src/CtSaving_Tiff.cpp)
    list(APPEND saving_libs ${TIFF_LIBRARIES})
    list(APPEND saving_includes ${TIFF_INCLUDE_DIRS})
  else()
    message(FATAL_ERROR "TIFF library not found, please install or disable LIMA_ENABLE_TIFF")
  endif()
endif()
