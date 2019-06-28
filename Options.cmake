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

# Use submodules
option(LIMA_BUILD_SUBMODULES "Build with submodules (vs prebuilt packages)?" ON)

if(UNIX)
  # Spec-like shared memory
  if(DEFINED ENV{COMPILE_SPS_IMAGE})
    set(LIMA_ENABLE_SPS_IMAGE "$ENV{COMPILE_SPS_IMAGE}" CACHE BOOL "compile sps_image?" FORCE)
  else()
    option(LIMA_ENABLE_SPS_IMAGE "compile sps_image?" OFF)
  endif()

  # openGL real-time display
  if(DEFINED ENV{COMPILE_GLDISPLAY})
    set(LIMA_ENABLE_GLDISPLAY "$ENV{COMPILE_GLDISPLAY}" CACHE BOOL "compile gldisplay?" FORCE)
  else()
    option(LIMA_ENABLE_GLDISPLAY "compile sps_image?" OFF)
  endif()
endif()

#--------------------------------------------------------------------------------
# Compile tests
#--------------------------------------------------------------------------------
option(LIMA_ENABLE_TESTS "compile test directories ?" OFF)

#--------------------------------------------------------------------------------
# Compile with trace (debug information)
#--------------------------------------------------------------------------------
option(LIMA_ENABLE_DEBUG "compile with trace ?" OFF)

#--------------------------------------------------------------------------------
# libconfig
#--------------------------------------------------------------------------------
if(DEFINED ENV{LIMA_ENABLE_CONFIG})
    set(LIMA_ENABLE_CONFIG "$ENV{LIMA_ENABLE_CONFIG}" CACHE BOOL "compile with libconfig code?" FORCE)
else()
    option(LIMA_ENABLE_CONFIG "compile with libconfig?" OFF)
endif()

#--------------------------------------------------------------------------------
# Saving formats can be enabled from environment variables using config.inc file
#--------------------------------------------------------------------------------
if(UNIX)
	# The formats have not been tested on windows
	if(DEFINED ENV{LIMA_ENABLE_CBF})
		set(LIMA_ENABLE_CBF "$ENV{LIMA_ENABLE_CBF}" CACHE BOOL "compile CBF saving code?" FORCE)
	else()
		option(LIMA_ENABLE_CBF "compile CBF saving code?" OFF)
	endif()
	if(DEFINED ENV{LIMA_ENABLE_FITS})
		set(LIMA_ENABLE_FITS "$ENV{LIMA_ENABLE_FITS}" CACHE BOOL "compile FITS saving code?" FORCE)
	else()
		option(LIMA_ENABLE_FITS "compile FITS saving code?" OFF)
	endif()
	if(DEFINED ENV{LIMA_ENABLE_NXS})
		set(LIMA_ENABLE_NXS "$ENV{LIMA_ENABLE_NXS}" CACHE BOOL "compile Nexus saving code?" FORCE)
	else()
		option(LIMA_ENABLE_NXS "compile Nexus saving code?" OFF)
	endif()
endif()

if(DEFINED ENV{LIMA_ENABLE_TIFF})
  set(LIMA_ENABLE_TIFF "$ENV{LIMA_ENABLE_TIFF}" CACHE BOOL "compile TIFF saving code?" FORCE)
else()
  option(LIMA_ENABLE_TIFF "compile TIFF saving code?" OFF)
endif()

if(DEFINED ENV{LIMA_ENABLE_EDFLZ4})
  set(LIMA_ENABLE_EDFLZ4 "$ENV{LIMA_ENABLE_EDFLZ4}" CACHE BOOL "compile EDF.LZ4 saving code?" FORCE)
else()
  option(LIMA_ENABLE_EDFLZ4 "compile EDF.LZ4 saving code?" OFF)
endif()

if(DEFINED ENV{LIMA_ENABLE_EDFGZ})
    set(LIMA_ENABLE_EDFGZ "$ENV{LIMA_ENABLE_EDFGZ}" CACHE BOOL "compile EDF.GZ saving code?" FORCE)
else()
    option(LIMA_ENABLE_EDFGZ "compile EDF.GZ saving code?" OFF)
endif()

if(DEFINED ENV{LIMA_ENABLE_HDF5})
    set(LIMA_ENABLE_HDF5 "$ENV{LIMA_ENABLE_HDF5}" CACHE BOOL "compile HDF5 saving code?" FORCE)
else()
    option(LIMA_ENABLE_HDF5 "compile HDF5 saving code?" OFF)
endif()

if(DEFINED ENV{LIMA_ENABLE_HDF5_BS})
    set(LIMA_ENABLE_HDF5_BS "$ENV{LIMA_ENABLE_HDF5_BS}" CACHE BOOL "compile HDF5/BS saving code?" FORCE)
else()
    option(LIMA_ENABLE_HDF5_BS "compile HDF5/BS saving code?" OFF)
endif()

# Compile python wrapping code generated using SIP
IF(DEFINED ENV{LIMA_ENABLE_PYTHON})
    set(LIMA_ENABLE_PYTHON "$ENV{LIMA_ENABLE_PYTHON}" CACHE BOOL "compile python modules?" FORCE)
else()
    option(LIMA_ENABLE_PYTHON "compile python modules?" OFF)
endif()

# Python Tango server
IF(DEFINED ENV{LIMA_ENABLE_PYTANGO_SERVER})
    set(LIMA_ENABLE_PYTANGO_SERVER "$ENV{LIMA_ENABLE_PYTANGO_SEVER}" CACHE BOOL "install python tango server?" FORCE)
else()
    option(LIMA_ENABLE_PYTANGO_SERVER "install python tango server?" OFF)
endif()
