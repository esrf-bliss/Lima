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

from Lima import Core
from Utils import getDataFromFile,BasePostProcess

class FlatfieldDeviceServer(BasePostProcess) :
    FLATFIELD_TASK_NAME = 'FlatField'

    def __init__(self,cl,name) :
        self.__flatFieldTask = None
        self.__flatFieldImage = Core.Processlib.Data()
        
        BasePostProcess__init__(self,cl,name)
        FlatfieldDeviceServer.init_device(self)

    def set_state(self,state) :
	if(state == PyTango.DevState.OFF) :
	    if(self.__flatFieldTask) :
		self.__flatFieldTask = None
		ctControl = _control_ref()
		extOpt = ctControl.externalOperation()
		extOpt.delOp(self.FLATFIELD_TASK_NAME)
	elif(state == PyTango.DevState.ON) :
	    if not self.__backGroundTask:
                ctControl = _control_ref()
                extOpt = ctControl.externalOperation()
                self.__flatFieldTask = extOpt.addOp(Core.FLATFIELDCORRECTION,
                                                    self.FLATFIELD_TASK_NAME,
                                                    self._runLevel)
                self.__flatFieldTask.setFlatFieldImage(self.__flatFieldImage)
	PyTango.Device_4Impl.set_state(self,state)

    def setFlatFieldImage(self,filepath) :
        self.__flatFieldImage = getDataFromFile(filepath)
        if(self.__flatFieldTask) :
            self.__flatFieldTask.setFlatFieldImage(self.__flatFieldImage)


class FlatfieldDeviceServerClass(PyTango.DeviceClass) :
        #	 Class Properties
    class_property_list = {
	}


    #	 Device Properties
    device_property_list = {
	}


    #	 Command definitions
    cmd_list = {
        'setFlatFieldImage':
        [[PyTango.DevVarStringArray,"Full path of background image file"],
         [PyTango.DevVoid,""]],
	'Start':
	[[PyTango.DevVoid,""],
	 [PyTango.DevVoid,""]],
	'Stop':
	[[PyTango.DevVoid,""],
	 [PyTango.DevVoid,""]],
	}


    #	 Attribute definitions
    attr_list = {
	'RunLevel':
	    [[PyTango.DevLong,
	    PyTango.SCALAR,
	    PyTango.READ_WRITE]],
	}


#------------------------------------------------------------------
#    RoiCounterDeviceServerClass Constructor
#------------------------------------------------------------------
    def __init__(self, name):
	PyTango.DeviceClass.__init__(self, name)
	self.set_type(name);

_control_ref = None
def set_control_ref(control_class_ref) :
    global _control_ref
    _control_ref= control_class_ref

def get_tango_specific_class_n_device() :
   return FlatfieldDeviceServerClass,FlatfieldDeviceServer
