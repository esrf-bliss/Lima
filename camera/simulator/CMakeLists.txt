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

set(SIMU_SRCS
    src/SimulatorFrameBuilder.cpp
    src/SimulatorFrameLoader.cpp
    src/SimulatorFramePrefetcher.cpp
    src/SimulatorCamera.cpp
    src/SimulatorInterface.cpp
    src/SimulatorSyncCtrlObj.cpp
    src/SimulatorDetInfoCtrlObj.cpp
    src/SimulatorShutterCtrlObj.cpp
    src/SimulatorBinCtrlObj.cpp
)

file(GLOB_RECURSE SIMU_INCS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_library(lima${NAME} SHARED ${SIMU_SRCS} ${SIMU_INCS})
limatools_set_library_soversion(lima${NAME} "VERSION")

target_include_directories(lima${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(lima${NAME} limacore)
if(WIN32)
    target_compile_definitions(lima${NAME} PRIVATE LIBSIMULATOR_EXPORTS)
endif()

limatools_set_install_libdir(lima${NAME})

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

if (LIMA_ENABLE_PYTHON)
    limatools_run_sip_for_camera(${NAME})
    add_custom_command(TARGET lima${NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/python ${CMAKE_BINARY_DIR}/python/Lima/Simulator)
    install(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Simulator")
endif()

if(LIMA_ENABLE_TESTS)
    add_subdirectory(test)
endif()
