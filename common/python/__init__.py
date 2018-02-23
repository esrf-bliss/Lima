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

import processlib as Processlib
import limacore
from limacore import *

_DebParams = DebParams


def _to_bytes(arg):
    if isinstance(arg, bytes):
        return arg
    return arg.encode()

class DebParams(_DebParams):
	def __init__(self, deb_mod, class_name, mod_name):
		_DebParams.__init__(self, deb_mod, _to_bytes(class_name), _to_bytes(mod_name))
		
#def DebParams(deb_mod, class_name, mod_name):
#    _DebParams(deb_mod, _to_bytes(class_name), _to_bytes(mod_name))

limacore.DebParams = DebParams
from .Debug import *
