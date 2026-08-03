# Find SIP
# ~~~~~~~~
#
# SIP website: http://www.riverbankcomputing.co.uk/sip/index.php
#
# Find the installed version of SIP. FindSIP should be called after Python
# has been found.
#
# This file defines the following variables:
#
# SIP_VERSION - The version of SIP found expressed as a 6 digit hex number
#     suitable for comparision as a string.
#
# SIP_VERSION_STR - The version of SIP found as a human readable string.
#
# SIP_EXECUTABLE - Path and filename of the SIP command line executable.
#
# SIP_INCLUDE_DIR - Directory holding the SIP C++ header file.
#
# SIP_DEFAULT_SIP_DIR - Default directory where .sip files should be installed
#     into.

# Copyright (c) 2007, Simon Edwards <simon@simonzone.com>
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(SIP_VERSION)
  # Already in cache, be silent
  set(SIP_FOUND TRUE)
else(SIP_VERSION)

  set(find_sip_py ${CMAKE_CURRENT_LIST_DIR}/FindSIP.py)

  execute_process(COMMAND ${Python3_EXECUTABLE} ${find_sip_py} OUTPUT_VARIABLE sip_config)
  if(sip_config)
    string(REPLACE "\n" ";" sip_config_lines "${sip_config}")
    foreach(sip_config_line ${sip_config_lines})
      if(sip_config_line MATCHES "^sip_version:(.+)$")
        set(SIP_VERSION "${CMAKE_MATCH_1}")
      elseif(sip_config_line MATCHES "^sip_version_str:(.+)$")
        set(SIP_VERSION_STR "${CMAKE_MATCH_1}")
      elseif(sip_config_line MATCHES "^default_sip_dir:(.+)$")
        set(SIP_DEFAULT_SIP_DIR "${CMAKE_MATCH_1}")
      elseif(sip_config_line MATCHES "^sip_inc_dir:(.+)$")
        set(SIP_INCLUDE_DIR "${CMAKE_MATCH_1}")
      elseif(sip_config_line MATCHES "^lima_sip_abi_version:(.+)$")
        set(LIMA_SIP_ABI_VERSION "${CMAKE_MATCH_1}")
      endif()
    endforeach()
    set(SIP_FOUND TRUE)
  endif(sip_config)

  find_program(SIP_BUILD_EXECUTABLE sip-build)
  if(SIP_BUILD_EXECUTABLE)
    if(NOT SIP_FIND_QUIETLY)
      message(STATUS "Found sip-build executable: ${SIP_BUILD_EXECUTABLE}")
    endif(NOT SIP_FIND_QUIETLY)
    set(SIP_FOUND TRUE)
  else(SIP_BUILD_EXECUTABLE)
    message(FATAL_ERROR "Could not find sip-build executable")
  endif(SIP_BUILD_EXECUTABLE)

  if(NOT LIMA_SIP_ABI_VERSION)
    message(FATAL_ERROR "Could not find lima.sip python module installed")
  else(NOT LIMA_SIP_ABI_VERSION)
	  message(STATUS "Found lima.sip python module with ABI version: ${LIMA_SIP_ABI_VERSION}")
  endif(NOT LIMA_SIP_ABI_VERSION)

  if(SIP_FOUND)
    if(NOT SIP_FIND_QUIETLY)
      message(STATUS "Found SIP version: ${SIP_VERSION_STR}")
    endif(NOT SIP_FIND_QUIETLY)
  else(SIP_FOUND)
    if(SIP_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find SIP")
    endif(SIP_FIND_REQUIRED)
  endif(SIP_FOUND)

endif(SIP_VERSION)
