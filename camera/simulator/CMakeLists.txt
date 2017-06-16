############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2017
# European Synchrotron Radiation Facility
# BP 220, Grenoble 38043
# FRANCE
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
SET(simu_srcs src/SimulatorFrameBuilder.cpp src/SimulatorCamera.cpp src/SimulatorInterface.cpp
              src/SimulatorSyncCtrlObj.cpp src/SimulatorDetInfoCtrlObj.cpp src/SimulatorShutterCtrlObj.cpp)

FILE(STRINGS "VERSION" simu_ver)
ADD_LIBRARY(limasimulator SHARED
    ${simu_srcs}
)
TARGET_INCLUDE_DIRECTORIES(limasimulator PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
TARGET_INCLUDE_DIRECTORIES(limasimulator PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../common/include")
TARGET_INCLUDE_DIRECTORIES(limasimulator PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../hardware/include")
TARGET_LINK_LIBRARIES(limasimulator limacore)
if(WIN32)
	TARGET_COMPILE_DEFINITIONS(limasimulator PRIVATE LIBSIMULATOR_EXPORTS)
endif()
SET_TARGET_PROPERTIES(limasimulator PROPERTIES VERSION ${simu_ver} SOVERSION "1")
if(WIN32)
    SET_TARGET_PROPERTIES(limasimulator PROPERTIES PREFIX "lib")
endif()
if(WIN32)
    INSTALL(TARGETS limasimulator
            DESTINATION lib)
else()
    INSTALL(TARGETS limasimulator
            LIBRARY DESTINATION lib)
endif()

IF (COMPILE_SIP)
    SET(NAME "simulator")
    SET(INCLUDES)
    FILE(GLOB sipfiles
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/sip/*.sip")
    FOREACH(sipfile ${sipfiles})
	SET(INCLUDES
"${INCLUDES}
%Include ${sipfile}"
)
    ENDFOREACH()

    SET(IMPORTS
"%Import limacore.sip"
)

    IF(SIP_VERSION_STR VERSION_LESS "4.12")
	    CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/sip/limamodules_before_4_12.sip.in sip/limasimulator.sip)
    ELSE()
	    CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/sip/limamodules.sip.in sip/limasimulator.sip)
    ENDIF()

    SET(SIP_CONCAT_PARTS 1)
    SET(SIP_INCLUDES ${SIP_INCLUDES}
        "${CMAKE_SOURCE_DIR}/third-party/Processlib/sip"
        "${CMAKE_BINARY_DIR}/sip/core"
        "${CMAKE_SOURCE_DIR}/third-party/Processlib/tasks/sip"
        "${CMAKE_SOURCE_DIR}/common/sip"
        "${CMAKE_SOURCE_DIR}/hardware/sip"
        "${CMAKE_SOURCE_DIR}/control/sip"
        "${CMAKE_SOURCE_DIR}/control/software_operation/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/sip")
    SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/sip/limasimulator.sip PROPERTIES OBJECT_DEPENDS ${sipfiles})
    set(SIP_DISABLE_FEATURES WITH_CONFIG)
    set(SIP_CHECK_EXC ON)
    ADD_SIP_PYTHON_MODULE(limasimulator ${CMAKE_CURRENT_BINARY_DIR}/sip/limasimulator.sip)
    TARGET_INCLUDE_DIRECTORIES(python_module_limasimulator PRIVATE
                               ${PYTHON_INCLUDE_DIRS}
                               "${CMAKE_SOURCE_DIR}/sip"
                               "${CMAKE_SOURCE_DIR}/sip/core"
                              )
    TARGET_LINK_LIBRARIES(python_module_limasimulator limasimulator)
    INSTALL(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Simulator")
ENDIF()
