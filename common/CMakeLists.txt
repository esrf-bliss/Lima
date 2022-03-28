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

set(common_srcs
    src/Constants.cpp
    src/SizeUtils.cpp
    src/Timestamp.cpp
    src/ThreadUtils.cpp
    src/Exceptions.cpp
    src/MemUtils.cpp
    src/AcqState.cpp
    src/Debug.cpp
    src/VideoUtils.cpp
    src/Event.cpp
    src/Timer.cpp
    src/DirectoryEventUtils.cpp
    src/AppPars.cpp
)

if(UNIX)
    list(INSERT common_srcs
        src/RegExUtils.cpp
        src/SimplePipe.cpp
    )
endif()

file(GLOB_RECURSE common_incs "include/*.h")

# Propagate to parent scope
# set(common_srcs ${common_srcs_list} PARENT_SCOPE)
# set(common_incs ${common_incs_list} PARENT_SCOPE)

add_library(common OBJECT ${common_srcs} ${common_incs})

target_include_directories(common PUBLIC "include")
