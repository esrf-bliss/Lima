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
CS_SCRIPTS   = /csadmin/common/scripts
OS           = $(shell $(CS_SCRIPTS)/get_os.share)

BLISS_DRV_BASE = /segfs/bliss/source/driver

ifndef ESPIA_DRV_BASE
ifeq ($(OS), suse82)
ARCH = x86
ESPIA_DRV_BASE = $(BLISS_DRV_BASE)/linux/espia/rel
else # OS != suse82
ESRF_VER       = $(shell $(CS_SCRIPTS)/get_esrflinux)
ESRF_MAJ_VER   = $(shell echo $(ESRF_VER) | cut -c1-4)
ESRF_COMP_BASE = $(BLISS_DRV_BASE)/linux-2.6/compile/$(ESRF_MAJ_VER)
ARCH           = $(shell $(CS_SCRIPTS)/get_esrflinux --architecture)
ifneq ($(OS), redhate5)
ESPIA_DRV_BASE = $(error Invalid ESRF/Linux version: $(ESRF_VER))
else # OS == redhate5
ifneq ($(ARCH), x86_64)
ESPIA_DRV_BASE = $(error Invalid ESRF/Linux architecture: $(ARCH))
else # ARCH == x86_64
ESPIA_DRV_BASE = $(ESRF_COMP_BASE)/espia/release
endif # ARCH == x86_64
endif # ESRF_VER == redhate5
endif # OS != suse82
endif # ESPIA_DRV_BASE

ifndef ESPIA_DRV_INCL
ESPIA_DRV_INCL = $(ESPIA_DRV_BASE)/src
endif

ifndef ESPIA_DRV_LIB
ESPIA_DRV_LIB = $(ESPIA_DRV_BASE)/lib/$(ARCH)
endif
