############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2011
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
sub-dirs = common

include ../config.inc

ifndef COMPILE_SIMULATOR
COMPILE_SIMULATOR = 0
endif

ifneq ($(COMPILE_SIMULATOR),0)
sub-dirs += simulator
endif

ifndef COMPILE_FRELON
COMPILE_FRELON = 0
endif

ifneq ($(COMPILE_FRELON),0)
sub-dirs += frelon
endif

ifndef COMPILE_MAXIPIX
COMPILE_MAXIPIX = 0
endif

ifneq ($(COMPILE_MAXIPIX),0)
sub-dirs += maxipix
endif

ifndef COMPILE_BASLER
COMPILE_BASLER = 0
endif

ifneq ($(COMPILE_BASLER),0)
sub-dirs += basler
export CXXFLAGS
endif

ifndef COMPILE_PROSILICA
COMPILE_PROSILICA = 0
endif

ifneq ($(COMPILE_PROSILICA),0)
sub-dirs += prosilica
endif

ifndef COMPILE_ROPERSCIENTIFIC
COMPILE_ROPERSCIENTIFIC = 0
endif

ifneq ($(COMPILE_ROPERSCIENTIFIC),0)
sub-dirs += roperscientific
endif

ifndef COMPILE_ADSC
COMPILE_ADSC = 0
endif

ifneq ($(COMPILE_ADSC),0)
sub-dirs += adsc
endif

ifndef COMPILE_MYTHEN
COMPILE_MYTHEN = 0
endif

ifneq ($(COMPILE_MYTHEN),0)
sub-dirs += mythen
endif

ifneq ($(COMPILE_UEYE),0)
sub-dirs += ueye
endif

ifneq ($(COMPILE_XPAD),0)
sub-dirs += xpad
endif

include ../global.inc
