###########################################################################
# This file is part of LImA, a Library for Image Acquisition
#
#  Copyright (C) : 2009-2017
#  European Synchrotron Radiation Facility
#  CS40220 38043 Grenoble Cedex 9 
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

function(limatools_set_library_soversion lib_name version_file)
  
    file(STRINGS ${version_file}  version)
    # for lib version as 1.2.3 soverion is fixed to 1.2
    string(REGEX MATCH "^([0-9]+)\\.([0-9]+)" soversion "${version}")
  
    set_target_properties(${lib_name} PROPERTIES VERSION "${version}" SOVERSION "${soversion}")
  
endfunction()


function(limatools_run_sip_for_camera cam_name)
  
  set(NAME ${cam_name})
  set(INCLUDES)
  file(GLOB sipfiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/sip" "${CMAKE_CURRENT_SOURCE_DIR}/sip/*.sip")
  foreach(sipfile ${sipfiles})
    set(INCLUDES 
      "${INCLUDES}
%Include ${sipfile}")
  endforeach()

  set(IMPORTS 
    "${IMPORTS}
%Import limacore.sip")

  if(SIP_VERSION_STR VERSION_LESS "4.12")
    configure_file(${CMAKE_SOURCE_DIR}/sip/limamodules_before_4_12.sip.in sip/lima${NAME}.sip)
  else()
    configure_file(${CMAKE_SOURCE_DIR}/sip/limamodules.sip.in sip/lima${NAME}.sip)
  endif()
  set(SIP_CONCAT_PARTS 1)
  set(SIP_INCLUDES ${SIP_INCLUDES}
    "${CMAKE_SOURCE_DIR}/third-party/Processlib/sip"
    "${CMAKE_BINARY_DIR}/sip/core"
    "${CMAKE_SOURCE_DIR}/third-party/Processlib/tasks/sip"
    "${CMAKE_SOURCE_DIR}/common/sip"
    "${CMAKE_SOURCE_DIR}/hardware/sip"
    "${CMAKE_SOURCE_DIR}/control/sip"
    "${CMAKE_SOURCE_DIR}/control/software_operation/sip"
    "${CMAKE_CURRENT_SOURCE_DIR}/sip")

  add_sip_python_module(lima${NAME} ${CMAKE_CURRENT_BINARY_DIR}/sip/lima${NAME}.sip)
  target_include_directories(python_module_lima${NAME} PRIVATE
    ${PYTHON_INCLUDE_DIRS}
    "${CMAKE_SOURCE_DIR}/sip"
    "${CMAKE_SOURCE_DIR}/sip/core"
    "${CMAKE_SOURCE_DIR}/third-party/Processlib/sip")
  target_link_libraries(python_module_lima${NAME} lima${NAME})
endfunction()
  

  