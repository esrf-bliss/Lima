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

set(hardware_srcs
    src/HwInterface.cpp
    src/HwCap.cpp
    src/HwSyncCtrlObj.cpp
    src/HwFrameInfo.cpp
    src/HwFrameCallback.cpp
    src/HwBufferCtrlObj.cpp
    src/HwBufferMgr.cpp
    src/HwShutterCtrlObj.cpp
    src/HwMaxImageSizeCallback.cpp
    src/HwDetInfoCtrlObj.cpp
    src/HwBinCtrlObj.cpp
    src/HwRoiCtrlObj.cpp
    src/HwFlipCtrlObj.cpp
    src/HwSerialLine.cpp
    src/HwBufferSave.cpp
    src/HwVideoCtrlObj.cpp
    src/HwEventCtrlObj.cpp
    src/HwSavingCtrlObj.cpp
    src/HwReconstructionCtrlObj.cpp
)

if(UNIX)
    list(INSERT hardware_srcs
        src/HwFileEventMgr.cpp
    )
endif()

file(GLOB_RECURSE hardware_incs "include/*.h")

# Propagate to parent scope
# set(hardware_srcs hardware_srcs_list PARENT_SCOPE)
# set(hardware_incs hardware_incs_list PARENT_SCOPE)

add_library(hardware OBJECT ${hardware_srcs} ${hardware_incs})

target_include_directories(hardware PUBLIC "include")
