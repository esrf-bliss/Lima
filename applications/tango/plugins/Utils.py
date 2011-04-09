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
import PyTango

try:
    import EdfFile
except ImportError:
    EdfFile = None
    
from Lima import Core

def getDataFromFile(filepath,index = 0) :
    try:
        datas = getDatasFromFile(filepath,index,index)
        return datas[0]
    except:
        return Core.Processlib.Data()   # empty

##@brief the function read all known data file
#
#@todo add more file format
def getDatasFromFile(filepath,fromIndex = 0,toIndex = -1) :
    returnDatas = []
    try:
        f = EdfFile.EdfFile(filepath)
        if toIndex < 0 :
            toIndex = f.GetNumImages()
        for i in range(fromIndex,toIndex + 1) :
            a = f.ReadImage(i)
            header = f.GetHeader(i)
            rData = Core.Processlib.Data()
            rData.buffer = a
            rData.header = header
            returnDatas.append(rData)
    except:
        return returnDatas


class BasePostProcess(PyTango.Device_4Impl) :

    def __init__(self,*args) :
        self._runLevel = 0
        PyTango.Device_4Impl.__init__(self,*args)

    def __getattr__(self,name) :
        if name.startswith('is_') and name.endswith('_allowed') :
            self.__dict__[name] = self.__global_allowed
            return self.__global_allowed
        raise AttributeError('%s has no attribute %s' %
                             (self.__class__.name,name))

    def __global_allowed(self,*args) :
        return self.get_state() == PyTango.DevState.ON

    def is_RunLevel_allowed(self,mode) :
        if(PyTango.AttReqType.READ_REQ == mode) :
            return True
        else:
            return self.get_state() == PyTango.DevState.OFF
    
    def is_set_state_allowed(self) :
        return True

    def init_device(self):
	self.set_state(PyTango.DevState.OFF)
	self.get_device_properties(self.get_device_class())

    def Start(self) :
        self.set_state(PyTango.DevState.ON)

    def Stop(self) :
        self.set_state(PyTango.DevState.OFF)

#------------------------------------------------------------------
#    Read RunLevel attribute
#------------------------------------------------------------------
    def read_RunLevel(self, attr):
	attr.set_value(self._runLevel)

#------------------------------------------------------------------
#    Write RunLevel attribute
#------------------------------------------------------------------
    def write_RunLevel(self, attr):
	data=[]
	attr.get_write_value(data)
        self._runLevel = data[0]
