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
import os, sys, imp, glob

root_name = __path__[0]
mod_name = os.path.basename(root_name)

def version_code(s):
        return map(int, s.strip('v').split('.'))

def version_cmp(x, y):
        return cmp(version_code(x), version_code(y))

env_var_name = 'LIMA_%s_VERSION' % mod_name.upper()
try:
        version = os.environ[env_var_name]
except KeyError:
        version = 'LAST'

req_version = version

if version.upper() == 'LAST':
        version_dirs = [x for x in os.listdir(root_name) if x.startswith('v')]
        version_dirs.sort(version_cmp)
        version = version_dirs[-1]
else:
        if version[0] != 'v':
                version = 'v' + version

mod_path = os.path.join(root_name, version)
if not (os.path.isdir(mod_path) or os.path.islink(mod_path)):
        raise ImportError('Invalid %s: %s' % (env_var_name, req_version))

__path__.append(mod_path)

del root_name, mod_name, mod_path, x, env_var_name
del version, req_version, version_dirs, version_code, version_cmp
del os, sys, imp, glob
