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
__all__ = []
def _init_module() :
    import os
    for root,dirs,files in os.walk(__path__[0]) :
        for file_name in files :
            if file_name.startswith('__') : continue
            base,ext = os.path.splitext(file_name)
            if ext == '.py' :
                subdir = root[len(__path__[0]) + 1:]
                if subdir:
                    base = '%s.%s' % (subdir,base)
                __all__.append(base)
_init_module()

