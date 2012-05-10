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
import os

root_name = __path__[0]

csadmin_dirs = ['/csadmin/local', '/csadmin/common']
script_get_os = 'scripts/get_compat_os.share'
get_os = None
for d in csadmin_dirs:
	aux_get_os = os.path.join(d, script_get_os)
	if os.path.exists(aux_get_os):
		get_os = aux_get_os
		break
if get_os is not None:
        compat_plat = os.popen(get_os).readline().strip()
	plat = None
	compat_plat_list = compat_plat.split()
        for aux_plat in compat_plat_list:
        	if aux_plat.strip() in os.listdir(root_name):
        		plat = aux_plat
        		break
	if plat is None:
		raise ImportError, ('Could not find Lima directory for %s '
				    '(nor compat. %s) platform(s) at %s' %
				    (compat_plat_list[0],
				     compat_plat_list[1:], root_name))
        lima_plat = os.path.join(root_name, plat)
        __path__.insert(0, lima_plat)
	
# This mandatory variable is systematically overwritten by 'make install'
os.environ['LIMA_LINK_STRICT_VERSION'] = 'MINOR'

if get_os is not None:
        all_dirs = os.listdir(lima_plat)
        all_dirs.remove('lib')

        __all__ = all_dirs
        del plat, compat_plat, aux_plat, lima_plat, all_dirs

del root_name, csadmin_dirs, get_os, script_get_os, d, aux_get_os
del os
