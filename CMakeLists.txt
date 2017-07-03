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

cmake_minimum_required(VERSION 2.8)
project (lima)

#--------------------------------------------------------------------------------
# cmake/ directory contains tools 
#--------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(LimaTools)

find_package(Threads REQUIRED)

if(UNIX)
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
endif()


if(UNIX)
#--------------------------------------------------------------------------------
# Spec-like shared memory
#--------------------------------------------------------------------------------
  if(DEFINED ENV{COMPILE_SPS_IMAGE})
    set(LIMA_ENABLE_SPS_IMAGE "$ENV{COMPILE_SPS_IMAGE}" CACHE BOOL "compile sps_image?" FORCE)
  else()
    option(LIMA_ENABLE_SPS_IMAGE "compile sps_image?" OFF)
  endif()
  
# openGL real-time display
  if(DEFINED ENV{COMPILE_GLDISPLAY})
    set(LIMA_ENABLE_GLDISPLAY "$ENV{COMPILE_GLDISPLAY}" CACHE BOOL "compile gldisplay?" FORCE)
  else()
    option(LIMA_ENABLE_GLDISPLAY "compile sps_image?" OFF)
  endif()
endif()

#--------------------------------------------------------------------------------
#Compile tests
#--------------------------------------------------------------------------------
option(LIMA_ENABLE_TESTS "compile test directories ?" OFF)

#--------------------------------------------------------------------------------
# libconfig
#--------------------------------------------------------------------------------
if(DEFINED ENV{LIMA_ENABLE_CONFIG})
    set(LIMA_ENABLE_CONFIG "$ENV{LIMA_ENABLE_CONFIG}" CACHE BOOL "compile with libconfig code?" FORCE)
else()
    option(LIMA_ENABLE_CONFIG "compile with libconfig?" OFF)
endif()
      
#--------------------------------------------------------------------------------
# Saving formats can be enabled from environment variables using config.inc file
#--------------------------------------------------------------------------------
if(DEFINED ENV{LIMA_ENABLE_CBF})
    set(LIMA_ENABLE_CBF "$ENV{LIMA_ENABLE_CBF}" CACHE BOOL "compile CBF saving code?" FORCE)
else()
    option(LIMA_ENABLE_CBF "compile CBF saving code?" OFF)
endif()
if(DEFINED ENV{LIMA_ENABLE_EDFGZ})
    set(LIMA_ENABLE_EDFGZ "$ENV{LIMA_ENABLE_EDFGZ}" CACHE BOOL "compile EDF.GZ saving code?" FORCE)
else()
    option(LIMA_ENABLE_EDFGZ "compile EDF.GZ saving code?" OFF)
endif()
if(DEFINED ENV{LIMA_ENABLE_EDFLZ4})
    set(LIMA_ENABLE_EDFLZ4 "$ENV{LIMA_ENABLE_EDFLZ4}" CACHE BOOL "compile EDF.LZ4 saving code?" FORCE)
else()
    option(LIMA_ENABLE_EDFLZ4 "compile EDF.LZ4 saving code?" OFF)
endif()
if(DEFINED ENV{LIMA_ENABLE_FITS})
    set(LIMA_ENABLE_FITS "$ENV{LIMA_ENABLE_FITS}" CACHE BOOL "compile FITS saving code?" FORCE)
else()
    option(LIMA_ENABLE_FITS "compile FITS saving code?" OFF)
endif()
if(DEFINED ENV{LIMA_ENABLE_HDF5})
    set(LIMA_ENABLE_HDF5 "$ENV{LIMA_ENABLE_HDF5}" CACHE BOOL "compile HDF5 saving code?" FORCE)
else()
    option(LIMA_ENABLE_HDF5 "compile HDF5 saving code?" OFF)
endif()
if(DEFINED ENV{LIMA_ENABLE_NXS})
    set(LIMA_ENABLE_NXS "$ENV{LIMA_ENABLE_NXS}" CACHE BOOL "compile Nexus saving code?" FORCE)
else()
    option(LIMA_ENABLE_NXS "compile Nexus saving code?" OFF) 
endif()
if(DEFINED ENV{LIMA_ENABLE_TIFF})
    set(LIMA_ENABLE_TIFF "$ENV{LIMA_ENABLE_TIFF}" CACHE BOOL "compile TIFF saving code?" FORCE)
else()
    option(LIMA_ENABLE_TIFF "compile TIFF saving code?" OFF)
endif()

# Compile python wrapping code generated using SIP
IF(DEFINED ENV{LIMA_ENABLE_PYTHON})
    set(LIMA_ENABLE_PYTHON "$ENV{LIMA_ENABLE_PYTHON}" CACHE BOOL "compile python modules?" FORCE)
else()
    option(LIMA_ENABLE_PYTHON "compile python modules?" OFF)
endif()

#--------------------------------------------------------------------------------
# Enable python binding code compilation using sip generator
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_PYTHON)
    find_package(PythonInterp 2.6)
    find_package(PythonLibs 2.6)
    if(${PYTHONINTERP_FOUND})
      # python site-packages folder
      execute_process(
	COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print get_python_lib()"
	OUTPUT_VARIABLE _PYTHON_SITE_PACKAGES_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()
    find_package(SIP)
    include(SIPMacros)

    set(PYTHON_SITE_PACKAGES_DIR ${_PYTHON_SITE_PACKAGES_DIR} CACHE PATH "where should python modules be installed?")

    if(WIN32)
      if("${PYTHON_VERSION_MINOR}" STREQUAL "6")
	set(NUMPY_INCLUDE_DIR "C:/Python26/Lib/site-packages/numpy/core/include" CACHE PATH "Path to NumPy include folder")
        set(NUMPY_LIB_DIR "C:/Python26/Lib/site-packages/numpy/core/lib" CACHE PATH "Path to NumPy lib folder")
        include_directories(${NUMPY_INCLUDE_DIR})
        link_directories(${NUMPY_LIB_DIR})
      else() #assuming other version is 2.7
        set(NUMPY_INCLUDE_DIR "C:/Python27/Lib/site-packages/numpy/core/include" CACHE PATH "Path to NumPy include folder")
        set(NUMPY_LIB_DIR "C:/Python27/Lib/site-packages/numpy/lib" CACHE PATH "Path to NumPy lib folder")
        include_directories(${NUMPY_INCLUDE_DIR})
        link_directories(${NUMPY_LIB_DIR})
      endif()
    endif()
    if(!${SIP_FOUND})
        message(FATAL_ERROR "sip executable not found, cannot build python extensions")
    endif()
    if(WIN32)
        set(SIP_TAGS WIN32_PLATFORM)
    elseif(UNIX)
        set(SIP_TAGS POSIX_PLATFORM)
    endif(WIN32)
    set(SIP_EXTRA_OPTIONS -e -g)    
 endif()

#--------------------------------------------------------------------------------
#Processlib third-party is mandatory
#--------------------------------------------------------------------------------
set(PROCESSLIB_EXTERNALLY_CONFIGURED ON)
if(LIMA_ENABLE_PYTHON)
  set(PROCESSLIB_EXTERNALLY_ENABLE_PYTHON ON)
endif()
add_subdirectory(third-party/Processlib)

#--------------------------------------------------------------------------------
# Define source files
#--------------------------------------------------------------------------------
if(WIN32)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/core/include/WindowSpecific")
endif(WIN32)

set(common_srcs common/src/Constants.cpp common/src/SizeUtils.cpp common/src/Timestamp.cpp common/src/ThreadUtils.cpp
                common/src/Exceptions.cpp common/src/MemUtils.cpp common/src/RegExUtils.cpp common/src/AcqState.cpp
                common/src/Debug.cpp common/src/VideoUtils.cpp common/src/Event.cpp common/src/Timer.cpp
                common/src/DirectoryEventUtils.cpp)

if(WIN32)
    list(REMOVE_ITEM common_srcs common/src/RegExUtils.cpp)
endif()

set(hw_srcs hardware/src/HwInterface.cpp hardware/src/HwCap.cpp hardware/src/HwSyncCtrlObj.cpp hardware/src/HwFrameInfo.cpp
            hardware/src/HwFrameCallback.cpp hardware/src/HwBufferCtrlObj.cpp hardware/src/HwBufferMgr.cpp
            hardware/src/HwShutterCtrlObj.cpp hardware/src/HwMaxImageSizeCallback.cpp hardware/src/HwDetInfoCtrlObj.cpp
            hardware/src/HwBinCtrlObj.cpp hardware/src/HwRoiCtrlObj.cpp hardware/src/HwFlipCtrlObj.cpp hardware/src/HwSerialLine.cpp
            hardware/src/HwBufferSave.cpp hardware/src/HwVideoCtrlObj.cpp hardware/src/HwEventCtrlObj.cpp hardware/src/HwSavingCtrlObj.cpp
            hardware/src/HwFileEventMgr.cpp hardware/src/HwReconstructionCtrlObj.cpp)

if(WIN32)
    list(REMOVE_ITEM hw_srcs hardware/src/HwFileEventMgr.cpp)
endif()

set(control_srcs control/src/CtSaving.cpp control/src/CtControl.cpp control/src/CtAcquisition.cpp control/src/CtBuffer.cpp
                 control/src/CtImage.cpp control/src/CtSaving_Edf.cpp control/src/CtShutter.cpp control/src/CtAccumulation.cpp control/src/CtVideo.cpp
                 control/src/CtEvent.cpp)
set(ct_srcs control/software_operation/src/SoftOpInternalMgr.cpp control/software_operation/src/SoftOpExternalMgr.cpp control/software_operation/src/SoftOpId.cpp)

#--------------------------------------------------------------------------------
# Option for libconfig++ support
#--------------------------------------------------------------------------------
set(extra_libs)
if(LIMA_ENABLE_CONFIG)
    add_definitions(-DWITH_CONFIG)
    find_library(LIB_CONFIG config++)
    set(extra_libs ${extra_libs} config++)
    set(common_srcs ${common_srcs} common/src/ConfigUtils.cpp)
    set(control_srcs ${control_srcs} control/src/CtConfig.cpp)
endif()

#--------------------------------------------------------------------------------
# Option for Spec-like shared-memory (SPS)  support
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_SPS_IMAGE)
    add_definitions(-DWITH_SPS_IMAGE)
    set(ct_srcs ${ct_srcs} control/src/CtSpsImage.cpp third-party/Sps/Src/sps.c)
endif()

#--------------------------------------------------------------------------------
# Option for openGL real-time display  support
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_GLDISPLAY)
  add_subdirectory(third-party/gldisplay)
endif()

#--------------------------------------------------------------------------------
# Option for extra saving formats edf.gz, edf.lz4, cbf, hdf5, tiff, fits ...
#--------------------------------------------------------------------------------
set(saving_libs)
set(saving_includes)

if(LIMA_ENABLE_EDFGZ)
    add_definitions(-DWITH_EDFGZ_SAVING)
    find_library(LIB_GZ z)
    set(saving_libs ${saving_libs} ${LIB_GZ})
endif()

if(LIMA_ENABLE_EDFLZ4)
    add_definitions(-DWITH_EDFLZ4_SAVING)
    find_library(LIB_LZ4 lz4)
    set(saving_libs ${saving_libs} ${LIB_LZ4})
endif()

if(LIMA_ENABLE_CBF)
    add_definitions(-DWITH_CBF_SAVING)
    set(ct_srcs ${ct_srcs} control/src/CtSaving_Cbf.cpp)
    find_library(LIB_CBF cbf)
    set(saving_libs ${saving_libs} ${LIB_CBF} crypto)
endif()

if(LIMA_ENABLE_FITS)
    add_definitions(-DWITH_FITS_SAVING)
    set(ct_srcs ${ct_srcs} control/src/CtSaving_Fits.cpp)
    find_library(LIB_CFITSIO cfitsio)
    find_library(LIB_CCFITS CCfits)
    set(saving_libs ${saving_libs} ${LIB_CFITSIO} ${LIB_CCFITS})
endif()

if(LIMA_ENABLE_HDF5)
    add_definitions(-DWITH_HDF5_SAVING)
    set(ct_srcs ${ct_srcs} control/src/CtSaving_Hdf5.cpp)
#    set(HDF5_EXTERNALLY_CONFIGURED 1)
#    set(HDF5_BUILD_CPP_LIB ON CACHE BOOL "" FORCE)
#    set(BUILD_SHARED_LIBS ON CACHE  BOOL "" FORCE)
#    add_subdirectory(third-party/hdf5)
#    set(saving_includes ${saving_includes} 
#    "${CMAKE_SOURCE_DIR}/third-party/hdf5/src"
#    "${CMAKE_SOURCE_DIR}/third-party/hdf5/c++/src")
    find_library(LIB_HDF5_CPP hdf5_cpp)
    find_library(LIB_HDF5 hdf5_serial)
    set(saving_libs ${saving_libs} hdf5_serial hdf5_cpp)    
    set(saving_includes ${saving_includes} "/usr/include/hdf5/serial")
endif()

if(LIMA_ENABLE_NXS)
    add_definitions(-DWITH_NXS_SAVING)    
    set(ct_srcs ${ct_srcs} control/src/CtSaving_NXS.cpp)
endif()

if(LIMA_ENABLE_TIFF)
    add_definitions(-DWITH_TIFF_SAVING)
    set(ct_srcs ${ct_srcs} control/src/CtSaving_Tiff.cpp)
    find_library(LIB_TIFF tiff)
    set(saving_libs ${saving_libs} tiff)
endif()

#--------------------------------------------------------------------------------
# Add the library limacore definition
#--------------------------------------------------------------------------------
add_library(limacore SHARED
    ${common_srcs}
    ${hw_srcs}
    ${control_srcs}
    ${ct_srcs}
)

limatools_set_library_soversion(limacore "common/VERSION")       
  
target_include_directories(limacore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/common/include")
target_include_directories(limacore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/hardware/include")
target_include_directories(limacore PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/control/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/include")
target_include_directories(limacore PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/core/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/tasks/include")
if(LIMA_ENABLE_SPS_IMAGE)
    target_include_directories(limacore PRIVATE "${CMAKE_SOURCE_DIR}/third-party/Sps/Include")
endif()
target_include_directories(limacore PRIVATE ${saving_includes})

if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(PUBLIC limacore "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(limacore "${CMAKE_THREAD_LIBS_INIT}")
endif()
if(UNIX)
    target_link_libraries(limacore "rt")
endif()
target_link_libraries(limacore processlib)
target_link_libraries(limacore ${saving_libs} ${extra_libs})
if(WIN32)
    target_compile_definitions(limacore PRIVATE LIMACORE_EXPORTS)
    set_target_properties(limacore PROPERTIES PREFIX "lib")
endif()
if(WIN32)
    install(TARGETS limacore
            DESTINATION lib
            PUBLIC_HEADER DESTINATION include)
else()
    install(TARGETS limacore
            LIBRARY DESTINATION lib
            PUBLIC_HEADER DESTINATION include)
endif()

#--------------------------------------------------------------------------------
# SIP generates binding code for python
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_PYTHON)
    set(NAME "core")
    set(INCLUDES)
    file(GLOB sipfiles
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/common/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/common/sip/*.sip"
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/hardware/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/hardware/sip/*.sip"
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/control/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/control/sip/*.sip"
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/sip/*.sip")
    foreach(sipfile ${sipfiles})
        set(INCLUDES
"${INCLUDES}
%Include ${sipfile}"
)
    endforeach()

    if(SIP_VERSION_STR VERSION_LESS "4.12")
	    configure_file(sip/core/limacore_before_4_12.sip.in sip/core/limacore.sip)
	    configure_file(hardware/sip/HwFrameInfo.sip_before_4_12 sip/core/HwFrameInfo.sip)
    else()
	    configure_file(sip/core/limacore.sip.in sip/core/limacore.sip)
	    configure_file(hardware/sip/HwFrameInfo.sip_after_4_12 sip/core/HwFrameInfo.sip)
    endif()
    set(SIP_CONCAT_PARTS 1)
    set(SIP_INCLUDES ${SIP_INCLUDES}
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/tasks/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/common/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/hardware/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/control/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/sip")
    if (NOT LIMA_ENABLE_CONFIG)
      set(SIP_DISABLE_FEATURES ${SIP_DISABLE_FEATURES} WITH_CONFIG)
    endif()
    if (NOT LIMA_ENABLE_SPS_IMAGE)
      set(SIP_DISABLE_FEATURES ${SIP_DISABLE_FEATURES} WITH_SPS_IMAGE)
    endif()
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/sip/core/limacore.sip PROPERTIES OBJECT_DEPENDS ${sipfiles} sip/core/HwFrameInfo.sip)
    # Run sip now !
    add_sip_python_module(limacore ${CMAKE_CURRENT_BINARY_DIR}/sip/core/limacore.sip)
    
    target_include_directories(python_module_limacore PRIVATE
        ${PYTHON_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/sip
        ${CMAKE_CURRENT_SOURCE_DIR}/sip/core
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/sip)
    target_link_libraries(python_module_limacore limacore)

    install(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima")
    install(FILES common/python/__init__.py common/python/Debug.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Core")
endif()

#--------------------------------------------------------------------------------
#TESTS, run ctest or make test
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_TESTS)
    enable_testing()
endif()

#--------------------------------------------------------------------------------
# CAMERA list is in cmake/CameraList.cmake file
#--------------------------------------------------------------------------------
include(CameraList)

#--------------------------------------------------------------------------------
# PACKAGES debian at least, run cpack or make package
#--------------------------------------------------------------------------------
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "ESRF/BLISS") #required
set(CPACK_PACKAGE_CONTACT "lima@esrf.fr")
include(CPack)
