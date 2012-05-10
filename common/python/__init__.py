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
from Lima import module_helper

mod_path = __path__
depends_on = None
has_dependent = True

cleanup_data = module_helper.load_prepare(mod_path, depends_on, has_dependent)

import processlib as Processlib
from limacore import *
from Debug import *

module_helper.load_cleanup(cleanup_data)

del mod_path, depends_on, has_dependent, cleanup_data
del module_helper
