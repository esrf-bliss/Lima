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

class BackgroundSubstractionDeviceServer(BasePostProcess) :
    BACKGROUND_TASK_NAME = 'BackGroundTask'

    def __init__(self,cl,name) :
        self.__backGroundTask = None
        self.__backGroundImage = Core.Processlib.Data()
        
        BasePostProcess__init__(self,cl,name)
        BackgroundSubstractionDeviceServer.init_device(self)

    def set_state(self,state) :
	if(state == PyTango.DevState.OFF) :
	    if(self.__backGroundTask) :
		self.__backGroundTask = None
		ctControl = _control_ref()
		extOpt = ctControl.externalOperation()
		extOpt.delOp(self.BACKGROUND_TASK_NAME)
	elif(state == PyTango.DevState.ON) :
	    if not self.__backGroundTask:
                ctControl = _control_ref()
                extOpt = ctControl.externalOperation()
                self.__backGroundTask = extOpt.addOp(Core.BACKGROUNDSUBSTRACTION,
                                                     self.BACKGROUND_TASK_NAME,
                                                     self._runLevel)
                self.__backGroundTask.setBackgroundImage(self.__backGroundImage)
	PyTango.Device_4Impl.set_state(self,state)

    def setBackgroundImage(self,filepath) :
        self.__backGroundImage = getDataFromFile(filepath)
        if(self.__backGroundTask) :
            self.__backGroundTask.setBackgroundImage(self.__backGroundImage)


class BackgroundSubstractionDeviceServerClass(PyTango.DeviceClass) :
        #	 Class Properties
    class_property_list = {
	}


    #	 Device Properties
    device_property_list = {
	}


    #	 Command definitions
    cmd_list = {
        'setBackgroundImage':
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
   return BackgroundSubstractionDeviceServerClass,BackgroundSubstractionDeviceServer
