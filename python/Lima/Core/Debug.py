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
from limacore import DebParams, DebObj
import os, sys, types
import functools

if sys.version_info < (3, 0):
    # Python 2: str
    def to_cstring(str):
        return str
else:
    # Python 3: bytes
    def to_cstring(str):
        return str.encode()

def DEB_GLOBAL_FUNCT(fn):
    return DEB_FUNCT(fn, True, 2)

def DEB_MEMBER_FUNCT(fn):
    return DEB_FUNCT(fn, False, 2)

def DEB_FUNCT(fn, in_global=True, frame=1, deb_container=None):
    frame = sys._getframe(frame)
    if in_global:
        n_dict = frame.f_globals
    else:
        n_dict = frame.f_locals
    deb_params = n_dict['deb_params']
    code = frame.f_code
    filename =  os.path.basename(code.co_filename)
    lineno = frame.f_lineno
    @functools.wraps(fn)
    def real_fn(*arg, **kw):
        # no longer exists on py3
        if sys.version_info < (3, 0):
            sys.exc_clear()
        fn_globals = dict(fn.__globals__)
        cstr = map(to_cstring, [fn.__name__, '', filename])
        deb_obj = DebObj(deb_params, False, cstr[0], cstr[1], cstr[2], lineno)
        fn_globals['deb'] = deb_obj
        if deb_container is not None:
            deb_container.add(deb_obj)
        new_fn = types.FunctionType(fn.__code__, fn_globals, fn.__name__,
                                    fn.__defaults__)
        return new_fn(*arg, **kw)
    return real_fn
        
def DEB_GLOBAL(deb_mod):
    DEB_PARAMS(deb_mod, '', True, 2)

def DEB_CLASS(deb_mod, class_name):
    DEB_PARAMS(deb_mod, class_name, False, 2)
    
def DEB_PARAMS(deb_mod, class_name, in_global=True, frame=1):
    frame = sys._getframe(frame)
    g_dict, l_dict = frame.f_globals, frame.f_locals
    mod_name = g_dict['__name__']
    if mod_name == '__main__':
        file_name = frame.f_code.co_filename
        mod_name = os.path.basename(file_name).strip('.py')
    if in_global:
        d_dict = g_dict
    else:
        d_dict = l_dict
    cstr = map(to_cstring, [class_name, mod_name])
    d_dict['deb_params'] = DebParams(deb_mod, *cstr)
