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

.NOTPARALLEL:

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

ifndef COMPILE_MYTHEN3
COMPILE_MYTHEN3 = 0
endif

ifneq ($(COMPILE_MYTHEN3),0)
sub-dirs += mythen3
endif

ifndef COMPILE_UEYE
COMPILE_UEYE = 0
endif

ifneq ($(COMPILE_UEYE),0)
sub-dirs += ueye
endif

ifndef COMPILE_ANDOR
COMPILE_ANDOR = 0
endif

ifneq ($(COMPILE_ANDOR),0)
sub-dirs += andor
endif

ifndef COMPILE_ANDOR3
COMPILE_ANDOR3 = 0
endif

ifneq ($(COMPILE_ANDOR3),0)
sub-dirs += andor3
endif

ifndef COMPILE_XH
COMPILE_XH = 0
endif

ifneq ($(COMPILE_XH),0)
sub-dirs += xh
endif

ifndef COMPILE_XSPRESS3
COMPILE_XSPRESS3 = 0
endif

ifneq ($(COMPILE_XSPRESS3),0)
sub-dirs += xspress3
endif

ifndef COMPILE_ULTRA
COMPILE_ULTRA = 0
endif

ifneq ($(COMPILE_ULTRA),0)
sub-dirs += ultra
endif

ifndef COMPILE_MERLIN
COMPILE_MERLIN = 0
endif

ifneq ($(COMPILE_MERLIN),0)
sub-dirs += merlin
endif

ifndef COMPILE_XPAD
COMPILE_XPAD = 0
endif

ifneq ($(COMPILE_XPAD),0)
sub-dirs += xpad
endif

ifndef COMPILE_MARCCD
COMPILE_MARCCD = 0
endif

ifneq ($(COMPILE_MARCCD),0)
sub-dirs += marccd
endif

ifndef COMPILE_PILATUS
COMPILE_PILATUS = 0
endif

ifneq ($(COMPILE_PILATUS),0)
sub-dirs += pilatus
endif

ifndef COMPILE_POINTGREY
COMPILE_POINTGREY = 0
endif

ifneq ($(COMPILE_POINTGREY),0)
sub-dirs += pointgrey
endif

ifndef COMPILE_IMXPAD
COMPILE_IMXPAD = 0
endif

ifneq ($(COMPILE_IMXPAD),0)
sub-dirs += imxpad
endif

ifndef COMPILE_RAYONIXHS
COMPILE_RAYONIXHS = 0
endif

ifneq ($(COMPILE_RAYONIXHS),0)
sub-dirs += rayonixhs
endif

ifndef COMPILE_AVIEX
COMPILE_AVIEX = 0
endif

ifneq ($(COMPILE_AVIEX),0)
sub-dirs += aviex
endif

ifndef COMPILE_V4l2
COMPILE_V4l2 = 0
endif

ifneq ($(COMPILE_V4l2),0)
sub-dirs += v4l2
endif

ifndef COMPILE_EIGER
COMPILE_EIGER = 0
endif

ifneq ($(COMPILE_EIGER),0)
sub-dirs += eiger
endif

ifndef COMPILE_DEXELA
COMPILE_DEXELA = 0
endif

ifneq ($(COMPILE_DEXELA),0)
sub-dirs += dexela
endif

ifndef COMPILE_PIXIRAD
COMPILE_PIXIRAD = 0
endif

ifneq ($(COMPILE_PIXIRAD),0)
sub-dirs += pixirad
endif

ifndef COMPILE_HEXITEC
COMPILE_HEXITEC = 0
endif

ifneq ($(COMPILE_HEXITEC),0)
sub-dirs += hexitec
endif

ifndef COMPILE_SLSDETECTOR
COMPILE_SLSDETECTOR = 0
endif

ifneq ($(COMPILE_SLSDETECTOR),0)
sub-dirs += slsdetector
endif

include ../global.inc
