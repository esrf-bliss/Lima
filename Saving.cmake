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

  if(LIMA_ENABLE_HDF5_JP2K)
    set(OPENJPEG_ROOT "" CACHE PATH "Path to OpenJPEG root directory")
    if(DEFINED ENV{OPENJPEG_ROOT})
      set(OPENJPEG_ROOT "$ENV{OPENJPEG_ROOT}" CACHE PATH "Path to OpenJPEG root directory" FORCE)
    endif()
    if(DEFINED ENV{OPENJPEG_INCLUDE_DIR})
      set(OPENJPEG_INCLUDE_DIR "$ENV{OPENJPEG_INCLUDE_DIR}" CACHE PATH "Path to OpenJPEG include directory" FORCE)
    endif()
    if(DEFINED ENV{OPENJPEG_LIBRARY})
      set(OPENJPEG_LIBRARY "$ENV{OPENJPEG_LIBRARY}" CACHE FILEPATH "Path to OpenJPEG library" FORCE)
    endif()

    find_path(OPENJPEG_INCLUDE_DIR openjpeg.h
      HINTS "${OPENJPEG_ROOT}/include" "${OPENJPEG_ROOT}"
      PATH_SUFFIXES openjpeg-2.5 openjpeg-2.4 openjpeg-2.3 openjpeg-2.2 openjpeg-2.1)
    find_library(OPENJPEG_LIBRARY openjp2
      HINTS "${OPENJPEG_ROOT}/lib" "${OPENJPEG_ROOT}/lib64" "${OPENJPEG_ROOT}")
    if(NOT OPENJPEG_INCLUDE_DIR OR NOT OPENJPEG_LIBRARY)
      message(FATAL_ERROR "OpenJPEG library not found, please install openjpeg development files or disable LIMA_ENABLE_HDF5_JP2K")
    endif()
    list(APPEND saving_includes ${OPENJPEG_INCLUDE_DIR})
    list(APPEND saving_libs ${OPENJPEG_LIBRARY})

    list(APPEND saving_definitions -DWITH_JP2K_COMPRESSION)

    set(KAKADU_ROOT "" CACHE PATH "Path to Kakadu root directory")
    set(KAKADU_LIBRARY_DIR "" CACHE PATH "Path to Kakadu library directory")
    if(DEFINED ENV{KAKADU_ROOT})
      set(KAKADU_ROOT "$ENV{KAKADU_ROOT}" CACHE PATH "Path to Kakadu root directory" FORCE)
    endif()
    if(DEFINED ENV{KAKADU_LIBRARY_DIR})
      set(KAKADU_LIBRARY_DIR "$ENV{KAKADU_LIBRARY_DIR}" CACHE PATH "Path to Kakadu library directory" FORCE)
    endif()
    if(DEFINED ENV{KAKADU_CORESYS_INCLUDE_DIR})
      set(KAKADU_CORESYS_INCLUDE_DIR "$ENV{KAKADU_CORESYS_INCLUDE_DIR}" CACHE PATH "Path to Kakadu coresys include directory" FORCE)
    endif()
    if(DEFINED ENV{KAKADU_SUPPORT_INCLUDE_DIR})
      set(KAKADU_SUPPORT_INCLUDE_DIR "$ENV{KAKADU_SUPPORT_INCLUDE_DIR}" CACHE PATH "Path to Kakadu support include directory" FORCE)
    endif()
    if(DEFINED ENV{KAKADU_AUX_LIBRARY})
      set(KAKADU_AUX_LIBRARY "$ENV{KAKADU_AUX_LIBRARY}" CACHE FILEPATH "Path to Kakadu auxiliary library" FORCE)
    endif()
    if(DEFINED ENV{KAKADU_CORE_LIBRARY})
      set(KAKADU_CORE_LIBRARY "$ENV{KAKADU_CORE_LIBRARY}" CACHE FILEPATH "Path to Kakadu core library" FORCE)
    endif()

    find_path(KAKADU_CORESYS_INCLUDE_DIR kdu_compressed.h
      HINTS "${KAKADU_ROOT}/coresys/common")
    find_path(KAKADU_SUPPORT_INCLUDE_DIR kdu_stripe_compressor.h
      HINTS "${KAKADU_ROOT}/apps/support")
    find_library(KAKADU_AUX_LIBRARY kdu_a86R
      HINTS "${KAKADU_LIBRARY_DIR}" "${KAKADU_ROOT}/lib/Linux-x86-64-gcc"
            "${KAKADU_ROOT}/lib")
    find_library(KAKADU_CORE_LIBRARY kdu_v86R
      HINTS "${KAKADU_LIBRARY_DIR}" "${KAKADU_ROOT}/lib/Linux-x86-64-gcc"
            "${KAKADU_ROOT}/lib")
    if(KAKADU_CORESYS_INCLUDE_DIR AND KAKADU_SUPPORT_INCLUDE_DIR AND
       KAKADU_AUX_LIBRARY AND KAKADU_CORE_LIBRARY)
      list(APPEND saving_definitions -DWITH_KAKADU_JP2K)
      list(APPEND saving_includes ${KAKADU_CORESYS_INCLUDE_DIR}
                                  ${KAKADU_SUPPORT_INCLUDE_DIR})
      list(APPEND saving_libs ${KAKADU_AUX_LIBRARY} ${KAKADU_CORE_LIBRARY}
                              dl)
    else()
      message(STATUS "Kakadu JP2K support disabled: set KAKADU_ROOT or Kakadu include/library paths")
    endif()
  endif()
endif()

if(LIMA_ENABLE_HDF5_JP2K AND NOT LIMA_ENABLE_HDF5)
  message(FATAL_ERROR "LIMA_ENABLE_HDF5_JP2K requires LIMA_ENABLE_HDF5")
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
