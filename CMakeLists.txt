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

cmake_minimum_required(VERSION 3.1)

project (lima)
include(cmake/project_version.cmake)

set(NAME "core")

# Include additional modules that are used unconditionally
include(GNUInstallDirs)
include(GenerateExportHeader)

# If conda build, always set lib dir to 'lib'
if($ENV{CONDA_BUILD})
  set(CMAKE_INSTALL_LIBDIR "lib")
endif()

# Set lower / upper case project names
string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)
string(TOLOWER "${PROJECT_NAME}" PROJECT_NAME_LOWER)

# Set targets export name (used by lima and dependencies)
set(TARGETS_EXPORT_NAME "${PROJECT_NAME_LOWER}-targets")
#set(namespace "${PROJECT_NAME}::")

# Enable C++11 and later
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC AND MSVC_VERSION GREATER 1500)
    # Compiler specific (parallel build)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# CMake additional macros
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(LimaTools)

if(UNIX AND LIMA_ENABLE_NUMA)
    # Numa is needed for advanced buffer management
    find_package(Numa REQUIRED)
endif()

# Import pthread
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

### Define options

include(Options.cmake)

### Find dependencies

# Processlib third-party is mandatory
if (LIMA_BUILD_SUBMODULES)
    add_subdirectory(third-party/Processlib)
else()
    find_package(Processlib REQUIRED)
endif()

# Enable python binding code compilation using sip generator
if(LIMA_ENABLE_PYTHON)
    find_package(PythonInterp)
    find_package(PythonLibs)
    if(${PYTHONINTERP_FOUND})
      # python site-packages folder
      execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print (get_python_lib())"
        OUTPUT_VARIABLE _PYTHON_SITE_PACKAGES_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)
  	else()
  		message(FATAL_ERROR "No python found, please install or disable LIMA_ENABLE_PYTHON")
    endif()

    set(PYTHON_SITE_PACKAGES_DIR ${_PYTHON_SITE_PACKAGES_DIR} CACHE PATH "where should python modules be installed?")

    find_package(SIP REQUIRED)
    include(SIPMacros)
    if(WIN32)
      set(SIP_TAGS WIN32_PLATFORM)
    elseif(UNIX)
      set(SIP_TAGS POSIX_PLATFORM)
    endif(WIN32)
    set(SIP_EXTRA_OPTIONS -e -g)

    find_package(NumPy REQUIRED)
 endif()

#--------------------------------------------------------------------------------
# Define source files
#--------------------------------------------------------------------------------

# add_subdirectory(common)
# add_subdirectory(control)
# add_subdirectory(control/software_operation)
# add_subdirectory(hardware)

set(common_srcs
    common/src/Constants.cpp
    common/src/SizeUtils.cpp
    common/src/Timestamp.cpp
    common/src/ThreadUtils.cpp
    common/src/Exceptions.cpp
    common/src/MemUtils.cpp
    common/src/AcqState.cpp
    common/src/Debug.cpp
    common/src/VideoUtils.cpp
    common/src/Event.cpp
    common/src/Timer.cpp
    common/src/DirectoryEventUtils.cpp)

if(UNIX)
    list(APPEND common_srcs
        common/src/RegExUtils.cpp
        common/src/SimplePipe.cpp
    )
endif()

file(GLOB_RECURSE common_incs "common/include/*.h")

set(hardware_srcs
    hardware/src/HwInterface.cpp
    hardware/src/HwCap.cpp
    hardware/src/HwSyncCtrlObj.cpp
    hardware/src/HwFrameInfo.cpp
    hardware/src/HwFrameCallback.cpp
    hardware/src/HwBufferCtrlObj.cpp
    hardware/src/HwBufferMgr.cpp
    hardware/src/HwShutterCtrlObj.cpp
    hardware/src/HwMaxImageSizeCallback.cpp
    hardware/src/HwDetInfoCtrlObj.cpp
    hardware/src/HwBinCtrlObj.cpp
    hardware/src/HwRoiCtrlObj.cpp
    hardware/src/HwFlipCtrlObj.cpp
    hardware/src/HwSerialLine.cpp
    hardware/src/HwBufferSave.cpp
    hardware/src/HwVideoCtrlObj.cpp
    hardware/src/HwEventCtrlObj.cpp
    hardware/src/HwSavingCtrlObj.cpp
    hardware/src/HwReconstructionCtrlObj.cpp
)

if(UNIX)
    list(APPEND hardware_srcs hardware/src/HwFileEventMgr.cpp)
endif()

file(GLOB_RECURSE hardware_incs "hardware/include/*.h")

set(control_srcs
    control/src/CtSaving.cpp
    control/src/CtControl.cpp
    control/src/CtAcquisition.cpp
    control/src/CtBuffer.cpp
    control/src/CtImage.cpp
    control/src/CtSaving_Compression.cpp
    control/src/CtSaving_Edf.cpp
    control/src/CtShutter.cpp
    control/src/CtAccumulation.cpp
    control/src/CtVideo.cpp
    control/src/CtEvent.cpp
)

file(GLOB_RECURSE control_incs "control/include/*.h")

set(software_operation_srcs
    control/software_operation/src/SoftOpInternalMgr.cpp
    control/software_operation/src/SoftOpExternalMgr.cpp
    control/software_operation/src/SoftOpId.cpp
)

file(GLOB_RECURSE software_operation_incs "control/software_operation/include/*.h")

set(extra_libs)
set(extra_includes)
set(extra_definitions)

# Optional for libconfig++ support
if(LIMA_ENABLE_CONFIG)
    find_package(Libconfig)
    if(${CONFIG_FOUND})
        list(APPEND extra_definitions -DWITH_CONFIG)
        list(APPEND extra_libs ${LIBCONFIG_LIBRARIES})
        list(APPEND common_srcs common/src/ConfigUtils.cpp)
        list(APPEND control_srcs control/src/CtConfig.cpp)
        list(APPEND extra_includes ${LIBCONFIG_INCLUDE_DIRS})
    else()
        message(FATAL_ERROR "libconfig++ not found, set LIB_CONFIG path or disable LIMA_ENABLE_CONFIG")
    endif()
endif()

# Option for Spec-like shared-memory (SPS)  support
if(LIMA_ENABLE_SPS_IMAGE)
    list(APPEND extra_definitions -DWITH_SPS_IMAGE)
    list(APPEND control_srcs control/src/CtSpsImage.cpp third-party/Sps/Src/sps.c)
endif()

# Option for extra saving formats edf.gz, edf.lz4, cbf, hdf5, tiff, fits
include(Saving.cmake)

#--------------------------------------------------------------------------------
# Add the library limacore definition
#--------------------------------------------------------------------------------
add_library(limacore SHARED
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/project_version.cc # Add version info
    ${common_srcs}
    ${hardware_srcs}
    ${control_srcs}
    ${software_operation_srcs}
    ${common_incs}
    ${hardware_incs}
    ${control_incs}
    ${software_operation_incs}
)

# Set version
set_target_properties(limacore PROPERTIES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}")

# Generate export macros
generate_export_header(limacore)

target_include_directories(limacore PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/hardware/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/control/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

if(LIMA_BUILD_SUBMODULES)
    target_include_directories(limacore PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/core/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/tasks/include>")
endif()

if(UNIX AND LIMA_ENABLE_NUMA)
    target_compile_definitions(limacore PUBLIC LIMA_USE_NUMA)
    target_include_directories(limacore PRIVATE "${NUMA_INCLUDE_DIR}")
endif()

if(LIMA_ENABLE_SPS_IMAGE)
    target_compile_definitions(limacore PUBLIC WITH_SPS_IMAGE)
    target_include_directories(limacore PRIVATE "${CMAKE_SOURCE_DIR}/third-party/Sps/Include")
endif()

# Set LIMA_NO_DEBUG if LIMA_ENABLE_DEBUG is set
if(NOT LIMA_ENABLE_DEBUG)
    target_compile_definitions(limacore PUBLIC LIMA_NO_DEBUG)
endif()

# add all include paths coming from saving format options
target_include_directories(limacore PRIVATE ${extra_includes} ${saving_includes})

# add compiler definitions required by saving libraries
target_compile_definitions(limacore PUBLIC ${extra_definitions} ${saving_definitions})

# add libraries
target_link_libraries(limacore PUBLIC Threads::Threads)
target_link_libraries(limacore PUBLIC processlib ${saving_libs} ${extra_libs})
target_link_libraries(limacore PRIVATE ${saving_private_libs})

if(UNIX)
    target_compile_definitions(limacore PUBLIC -DHAS_INOTIFY)
    target_link_libraries(limacore PUBLIC "rt")
    if(LIMA_ENABLE_NUMA)
        target_link_libraries(limacore PUBLIC ${NUMA_LIBRARY})
    endif()
endif()

if(WIN32)
    target_compile_definitions(limacore PRIVATE LIMACORE_EXPORTS)
    set_target_properties(limacore PROPERTIES PREFIX "lib")
    set_target_properties(limacore PROPERTIES IMPORT_PREFIX "lib")
endif()

#--------------------------------------------------------------------------------
# SIP generates binding code for python
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_PYTHON)
    set(INCLUDES)
    file(GLOB SIP_SOURCES
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/common/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/common/sip/*.sip"
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/hardware/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/hardware/sip/*.sip"
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/control/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/control/sip/*.sip"
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/sip/*.sip")
    foreach(SIP_SOURCES ${SIP_SOURCES})
        set(INCLUDES
"${INCLUDES}
%Include ${SIP_SOURCES}"
)
    endforeach()

    configure_file(sip/limacore.sip.in sip/limacore.sip)
    configure_file(sip/lima_init_numpy.cpp.in sip/core/limacore_init_numpy.cpp)
    list(APPEND LIMA_SIP_INCLUDE_DIRS
        "${CMAKE_CURRENT_BINARY_DIR}/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/common/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/hardware/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/control/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/sip")
    if (LIMA_BUILD_SUBMODULES)
      list(APPEND LIMA_SIP_INCLUDE_DIRS
          "${CMAKE_CURRENT_SOURCE_DIR}/sip")
      # This is required for LimaTools
      list(APPEND PROCESSLIB_SIP_INCLUDE_DIRS
          "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/sip"
          "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/tasks/sip")
    endif()
    set(SIP_INCLUDE_DIRS
        ${LIMA_SIP_INCLUDE_DIRS}
        ${PROCESSLIB_SIP_INCLUDE_DIRS})
    if (NOT LIMA_ENABLE_CONFIG)
      set(SIP_DISABLE_FEATURES ${SIP_DISABLE_FEATURES} WITH_CONFIG)
    endif()
    if (NOT LIMA_ENABLE_SPS_IMAGE)
      set(SIP_DISABLE_FEATURES ${SIP_DISABLE_FEATURES} WITH_SPS_IMAGE)
    endif()
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/sip/limacore.sip PROPERTIES OBJECT_DEPENDS ${SIP_SOURCES})
    # Run sip now !
    add_sip_python_module(limacore ${CMAKE_CURRENT_BINARY_DIR}/sip/limacore.sip FALSE)

    target_include_directories(python_module_limacore PRIVATE
        ${PYTHON_INCLUDE_DIRS}
        ${NUMPY_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/sip
        ${CMAKE_CURRENT_SOURCE_DIR}/sip/core
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/sip)
    target_link_libraries(python_module_limacore PRIVATE limacore ${NUMPY_LIBRARIES})

    add_custom_command(TARGET lima${NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/python/Lima ${CMAKE_BINARY_DIR}/python/Lima)
endif()

## Installation

include(Install.cmake)

#--------------------------------------------------------------------------------
# Option for openGL real-time display  support
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_GLDISPLAY)
    add_subdirectory(third-party/gldisplay)
endif()

#--------------------------------------------------------------------------------
# TESTS, run ctest or make test
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(control/test)
    add_subdirectory(common/test)
endif()

#--------------------------------------------------------------------------------
# CAMERA list is in cmake/CameraList.cmake file
#--------------------------------------------------------------------------------
if (LIMA_BUILD_SUBMODULES)
    include(CameraList)
endif()

#--------------------------------------------------------------------------------
# Python Tango server
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_PYTANGO_SERVER)
    add_subdirectory(applications/tango/python)
endif()

#--------------------------------------------------------------------------------
# PACKAGES debian at least, run cpack or make package
#--------------------------------------------------------------------------------
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "ESRF/BLISS") #required
set(CPACK_PACKAGE_CONTACT "lima@esrf.fr")
include(CPack)
