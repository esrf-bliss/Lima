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
include ../config.inc

ifneq ($(COMPILE_CORE), 0)
sip-modules += core
endif

ifneq ($(COMPILE_SIMULATOR), 0)
sip-modules += simulator
endif

ifneq ($(COMPILE_ESPIA), 0)
sip-modules += espia
endif

ifneq ($(COMPILE_FRELON), 0)
sip-modules += frelon
endif

ifneq ($(COMPILE_MAXIPIX), 0)
sip-modules += maxipix
endif

ifneq ($(COMPILE_BASLER), 0)
sip-modules += basler
endif

ifneq ($(COMPILE_PROSILICA), 0)
sip-modules += prosilica
endif


ifneq ($(COMPILE_ROPERSCIENTIFIC), 0)
sip-modules += roperscientific
endif

ifneq ($(COMPILE_ADSC), 0)
sip-modules += adsc
endif

ifneq ($(COMPILE_MYTHEN), 0)
sip-modules += mythen
endif


ifneq ($(COMPILE_UEYE), 0)
sip-modules += ueye
endif

all:	src

src:
	for d in $(sip-modules); do \
		$(MAKE) -C $$d || break; \
	done

config:	clean mrproper
	python configure.py

clean:
	for d in $(sip-modules); do \
		$(MAKE) -C $$d clean || break; \
	done

mrproper:
	python mrproper.py

test:
	@true
