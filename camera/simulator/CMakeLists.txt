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
set(simu_srcs src/SimulatorFrameBuilder.cpp src/SimulatorCamera.cpp src/SimulatorInterface.cpp
              src/SimulatorSyncCtrlObj.cpp src/SimulatorDetInfoCtrlObj.cpp src/SimulatorShutterCtrlObj.cpp)

file(STRINGS "VERSION" simu_ver)
add_library(limasimulator SHARED ${simu_srcs})

target_include_directories(limasimulator PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(limasimulator PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../common/include")
target_include_directories(limasimulator PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../hardware/include")
target_link_libraries(limasimulator limacore)
if(WIN32)
	target_compile_definitions(limasimulator PRIVATE LIBSIMULATOR_EXPORTS)
endif()
set_target_properties(limasimulator PROPERTIES VERSION ${simu_ver} SOVERSION "1")
if(WIN32)
    set_target_properties(limasimulator PROPERTIES PREFIX "lib")
endif()
if(WIN32)
    install(TARGETS limasimulator
            DESTINATION lib)
else()
    install(TARGETS limasimulator
            LIBRARY DESTINATION lib)
endif()

if (COMPILE_SIP)
    set(NAME "simulator")
    include(CameraRunSIP)
    install(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Simulator")
endif()

if(COMPILE_TEST)
	add_subdirectory(test/)
endif()
