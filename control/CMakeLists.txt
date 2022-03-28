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

set(control_srcs
    src/CtSaving.cpp
    src/CtControl.cpp
    src/CtAcquisition.cpp
    src/CtBuffer.cpp
    src/CtImage.cpp
    src/CtSaving_ZBuffer.cpp
    src/CtSaving_Compression.cpp
    src/CtSaving_Edf.cpp
    src/CtShutter.cpp
    src/CtAccumulation.cpp
    src/CtVideo.cpp
    src/CtEvent.cpp
    src/CtTestApp.cpp
)

file(GLOB_RECURSE control_incs "include/*.h")

# Propagate to parent scope
# set(control_srcs control_srcs_list PARENT_SCOPE)
# set(control_incs control_incs_list PARENT_SCOPE)

add_library(control OBJECT ${control_srcs} ${control_incs})

target_include_directories(control PUBLIC "include")
