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
.PHONY: src test clean

ifneq ($(sub-dirs),)
src-targets   += $(sub-dirs:%=%.src)
test-targets  += $(sub-dirs:%=%.test)
clean-targets += $(sub-dirs:%=%.clean)

$(src-targets):
	make -C $(@:%.src=%)   src

$(test-targets):
	make -C $(@:%.test=%)  test 

$(clean-targets):
	make -C $(@:%.clean=%) clean

else

ifneq ($(src-dirs),)
src-targets   += $(src-dirs:%=%.src)
clean-targets += $(src-dirs:%=%.clean)
endif

ifneq ($(test-dirs),)
test-targets  += $(test-dirs:%=%.test)
clean-targets += $(test-dirs:%=%.clean)
endif

$(src-targets):
	make -C $(@:%.src=%)

$(test-targets):
	make -C $(@:%.test=%)

$(clean-targets):
	make -C $(@:%.clean=%) clean

endif

src:	$(src-targets)

test:	$(test-targets)

clean:	$(clean-targets)
