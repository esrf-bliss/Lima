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
import os, sys, re, DLFCN

VSEP = '.'
version_re = re.compile('v?([0-9]+)(\\.([0-9]+)(\\.([0-9]+))?)?')


def version_code(s):
        return map(int, s.strip('v').split(VSEP))
	
def version_cmp(x, y):
        return cmp(version_code(x), version_code(y))

def good_version_dir(v, r, f):
	m = version_re.match(v)
	d = os.path.join(r, v)
	if not (m and m.group(5) and v.startswith('v') and
		(os.path.isdir(d) or os.path.islink(d))):
		return False
	c = version_code(v)[:len(f)]
	return c == f

def load_prepare(mod_path, depends_on, has_dependent):
	root_name = mod_path[0]
	mod_name = os.path.basename(root_name)

	env_var_name = 'LIMA_%s_VERSION' % mod_name.upper()
	if env_var_name in os.environ:
		version = os.environ[env_var_name]
	else:
	        version = 'LAST'

	if version.upper() == 'LAST':
		version_filter = []
	elif version_re.match(version):
		version_filter = version_code(version)
	else:
		raise ImportError('Invalid %s: %s' % (env_var_name, version))

	def good_dir(v, r=root_name, f=version_filter):
		return good_version_dir(v, r, f)

	version_dirs = [x for x in os.listdir(root_name) if good_dir(x)]
	if not version_dirs:
	        raise ImportError('Invalid %s: %s' % (env_var_name, version))

	version_dirs.sort(version_cmp)
	version = version_dirs[-1]
	mod_dir = os.path.join(root_name, version)
	ld_open_flags = sys.getdlopenflags()

	cleanup_data = mod_path, mod_dir, ld_open_flags, has_dependent

	if not depends_on:
		return load_ld_prepare(cleanup_data)

	cap_dep = depends_on.upper()
	dep_version_fname = os.path.join(mod_dir, '%s_VERSION' % cap_dep)
	dep_version_file = open(dep_version_fname, 'rt')
	dep_version = dep_version_file.readline().strip()

	link_strict_version = os.environ['LIMA_LINK_STRICT_VERSION']
	if link_strict_version == 'MINOR':
		dep_version = VSEP.join(dep_version.split(VSEP)[:2])
	elif link_strict_version != 'FULL':
		raise ImportError('Invalid LIMA_LINK_STRICT_VERSION var: %s' %
				  link_strict_version)
	
	env_var_name = 'LIMA_%s_VERSION' % cap_dep
	os.environ[env_var_name] = dep_version

	return cleanup_data

def load_ld_prepare(cleanup_data):
	mod_path, mod_dir, ld_open_flags, has_dependent = cleanup_data
	sys.setdlopenflags(ld_open_flags | DLFCN.RTLD_GLOBAL)
	if has_dependent:
		sys.path.insert(0, mod_dir)
	else:
		mod_path.append(mod_dir)
	return cleanup_data
	
def load_dep_cleanup(cleanup_data):
	return load_ld_prepare(cleanup_data)

def load_cleanup(cleanup_data):
	mod_path, mod_dir, ld_open_flags, has_dependent = cleanup_data
	if has_dependent:
		sys.path.remove(mod_dir)
	sys.setdlopenflags(ld_open_flags)

