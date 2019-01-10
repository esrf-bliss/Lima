###########################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2019
# European Synchrotron Radiation Facility
# CS40220 38043 Grenoble Cedex 9
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

project(meta)

set(NAME "meta") # This is used by LimaTools

# Include additional modules that are used inconditionnaly
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

# Check if project is being used directly or via add_subdirectory
set(CAMERA_MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(CAMERA_MASTER_PROJECT ON)
endif()

if (CAMERA_MASTER_PROJECT)
  find_package(Lima REQUIRED)
endif()

# CMake additional macros
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${LIMA_CMAKE_INCLUDE_DIRS} ${CMAKE_MODULE_PATH})

# Set version
include(project_version)

# Enable python binding code compilation using sip generator
if (CAMERA_MASTER_PROJECT)	
  option(LIMA_ENABLE_PYTHON "compile python binding code?" OFF)	
endif()

if (LIMA_ENABLE_PYTHON)
  # Find python interpreter and libs
  find_package(PythonInterp)
  find_package(PythonLibs)
  if(${PYTHONINTERP_FOUND})
    # python site-packages folder
    execute_process(
      COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print (get_python_lib())"
      OUTPUT_VARIABLE _PYTHON_SITE_PACKAGES_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
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

file(GLOB_RECURSE META_INCS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# Library definition
add_library(meta SHARED
  src/MetaInterface.cpp
  src/MetaSyncCtrlObj.cpp
  src/MetaDetInfoCtrlObj.cpp
  ${META_INCS}
)

# Generate export macros
generate_export_header(meta)

# Set version and output name
set_target_properties(meta PROPERTIES
  OUTPUT_NAME "lima${PROJECT_NAME_LOWER}"
  VERSION "${VERSION}"
  SOVERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}")

# Add dependencies
target_include_directories(meta
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"           # For export header
    PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(meta PUBLIC limacore)

if(WIN32)
  target_compile_definitions(meta
    PRIVATE meta_EXPORTS
    PUBLIC NOMINMAX)
  set_target_properties(meta PROPERTIES PREFIX "lib" IMPORT_PREFIX "lib")
endif()

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# Binding code for python
if(LIMA_ENABLE_PYTHON)
  include(LimaTools)
  limatools_run_sip_for_camera(meta)
endif()

# Generate and install package config file and version
if(CAMERA_MASTER_PROJECT)
  set (PROJECT_LIBRARIES meta)
  set(SIP_INSTALL_DIR ${CMAKE_INSTALL_DATADIR}/sip/lima)
  set(CMAKE_INSTALL_DIR ${CMAKE_INSTALL_DATADIR}/cmake/lima)
  include(package_config)
endif()

## Installation

install(
  TARGETS meta
  EXPORT "${TARGETS_EXPORT_NAME}"
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}   # import library
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}   # .so files are libraries
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}   # .dll files are binaries
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}  # this does not actually install anything (but used by downstream projects)
)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  COMPONENT devel
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.h"
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/project_version.h
    ${PROJECT_BINARY_DIR}/meta_export.h
  COMPONENT devel
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

if(LIMA_ENABLE_PYTHON)
  install(
    FILES python/__init__.py
    DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Meta"
  )
endif()

## Tests
if(CAMERA_ENABLE_TESTS)
    enable_testing()
    #add_subdirectory(test)
endif()
