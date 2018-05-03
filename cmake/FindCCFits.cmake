# -- Check for CFITSIO and CCFITS library --
#
# Uses path overrides: CFITSIOINC_PATH and CFITSIOLIB_PATH
#
# Defines:
#
#  CFITSIO_FOUND - system has the CFITSIO library
#  CFITSIO_INCLUDE_DIR - the CFITSIO include directory
#  CFITSIO_LIBRARY - The library needed to use CFITSIO
#
#  CCFITS_FOUND
#  CCFITS_INCLUDE_DIR
#  CCFITS_LIBRARY
#
# Hints for non-standard locations:
#
#   CFITSIO_ROOT_DIR  (used for finding cfitsio only)
#   CCFITS_ROOT_DIR (used for finding CCfits only)
#   BASE_DIR (used for both, and other dependencies as well)
#
# These directories can be the --prefix=<dir> directory or the
# individual source directories for cfitsio or ccfits.
#
#   Copyright 2012  Jacek Becla, Daniel Liwei Wang
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
include(FindPackageHandleStandardArgs)

# Look for includes and libraries
find_path(CFITSIO_INCLUDE_DIR
  NAMES fitsio.h
  HINTS ${CFITSIO_ROOT_DIR} ${BASE_DIR}
  PATH_SUFFIXES include include/cfitsio
  )
find_library(CFITSIO_LIBRARY  cfitsio
  HINTS ${CFITSIO_ROOT_DIR} ${BASE_DIR}
  PATH_SUFFIXES lib)

find_package_handle_standard_args(CFITSIO  DEFAULT_MSG
  CFITSIO_INCLUDE_DIR CFITSIO_LIBRARY)
if(NOT CFITSIO_FOUND)
  message("Can't find CCfits. You can specify a non-standard (e.g., from source) installation using CFITSIO_ROOT_DIR")
endif(NOT CFITSIO_FOUND)



find_path(CCFITS_INCLUDE_DIR  CCfits/CCfits
  HINTS ${CCFITS_ROOT_DIR} ${BASE_DIR}
  PATH_SUFFIXES include)
find_library(CCFITS_LIBRARY  CCfits
  HINTS ${CCFITS_ROOT_DIR} ${BASE_DIR}
  PATH_SUFFIXES lib)
find_package_handle_standard_args(CCFITS  DEFAULT_MSG
  CCFITS_INCLUDE_DIR CCFITS_LIBRARY)

if(NOT CCFITS_FOUND)
  message("Can't find CCfits. You can specify a non-standard (e.g., from source) installation using CCFITS_ROOT_DIR")
endif(NOT CCFITS_FOUND)

#find_package_handle_standard_args(LIBSZ  DEFAULT_MSG  SZ_LIB)

mark_as_advanced(CFITSIO_LIBRARY CFITSIO_INCLUDE_DIR
  CCFITS_LIBRARY CCFITS_INCLUDE_DIR)
