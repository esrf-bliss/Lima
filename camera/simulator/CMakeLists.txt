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

set(NAME "simulator")

set(simu_srcs src/SimulatorFrameBuilder.cpp src/SimulatorCamera.cpp src/SimulatorInterface.cpp
              src/SimulatorSyncCtrlObj.cpp src/SimulatorDetInfoCtrlObj.cpp src/SimulatorShutterCtrlObj.cpp)

add_library(lima${NAME} SHARED ${simu_srcs})
limatools_set_library_soversion(lima${NAME} "VERSION")

target_include_directories(lima${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(lima${NAME} limacore)
if(WIN32)
	target_compile_definitions(lima${NAME} PRIVATE LIBSIMULATOR_EXPORTS)
endif()
if(WIN32)
    set_target_properties(lima${NAME} PROPERTIES PREFIX "lib")
endif()
if(WIN32)
    install(TARGETS lima${NAME}
            DESTINATION lib)
else()
    install(TARGETS lima${NAME}
            LIBRARY DESTINATION lib)
endif()

if (LIMA_ENABLE_PYTHON)
    limatools_run_sip_for_camera(${NAME})
    install(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Simulator")
endif()

if(LIMA_ENABLE_TESTS)
	add_subdirectory(test)
endif()
