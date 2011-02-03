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

import itertools
import weakref
import PyTango
import sys
import numpy
import processlib
from Lima import Core
from Utils import getDataFromFile,BasePostProcess

#==================================================================
#   Roi2spectrum Class Description:
#
#
#==================================================================


class Roi2spectrumDeviceServer(BasePostProcess) :

#--------- Add you global variables here --------------------------
    ROI_SPECTRUM_TASK_NAME = "Roi2SpectrumTask"
#------------------------------------------------------------------
#    Device constructor
#------------------------------------------------------------------
    def __init__(self,cl, name):
	self.__roi2spectrumMgr = None
                
	BasePostProcess.__init__(self,cl,name)
	Roi2spectrumDeviceServer.init_device(self)

    def set_state(self,state) :
	if(state == PyTango.DevState.OFF) :
	    if(self.__roi2spectrumMgr) :
		self.__roi2spectrumMgr = None
		ctControl = _control_ref()
		extOpt = ctControl.externalOperation()
		extOpt.delOp(self.ROI_SPECTRUM_TASK_NAME)
	elif(state == PyTango.DevState.ON) :
	    if not self.__roi2spectrumMgr:
                ctControl = _control_ref()
                extOpt = ctControl.externalOperation()
                self.__roi2spectrumMgr = extOpt.addOp(Core.ROI2SPECTRUM,
                                                      self.ROI_SPECTRUM_TASK_NAME,
                                                      self._runLevel)
            self.__roi2spectrumMgr.clearCounterStatus()
            
	PyTango.Device_4Impl.set_state(self,state)

#------------------------------------------------------------------
#    Read BufferSize attribute
#------------------------------------------------------------------
    def read_BufferSize(self, attr):
	value_read = self.__roi2spectrumMgr.getBufferSize()
	attr.set_value(value_read)


#------------------------------------------------------------------
#    Write BufferSize attribute
#------------------------------------------------------------------
    def write_BufferSize(self, attr):
	data=[]
	attr.get_write_value(data)
        self.__roi2spectrumMgr.setBufferSize(data[0])


#------------------------------------------------------------------
#    Read CounterStatus attribute
#------------------------------------------------------------------
    def read_CounterStatus(self, attr):
	value_read = self.__roi2spectrumMgr.getCounterStatus()
	attr.set_value(value_read)


#==================================================================
#
#    Roi2spectrum command methods
#
#==================================================================
    def add(self,argin):
        if not len(argin) % 4:
            self.__roi2spectrumMgr.add(self.__get_roi_list_from_argin(argin))
        else:
            raise AttributeError('should be a roi vector as follow [x0,y0,width0,height0,x1,y1,width1,heigh1,...')
    
    def set(self,argin):
        if not len(argin) % 4:
            self.__roi2spectrumMgr.set(self.__get_roi_list_from_argin(argin))
        else:
            raise AttributeError('should be a roi vector as follow [x0,y0,width0,height0,x1,y1,width1,heigh1,...')

    
    def get(self):
        returnList = []
        for roi in self.__roi2spectrumMgr.get():
            p = roi.getTopLeft()
            s = roi.getSize()
            returnList.extend((p.x,p.y,s.getWidth(),s.getHeight()))
        return returnList

    def getRoiMode(self) :
        return self.__roi2spectrumMgr.getRoiMode()

    def setRoiMode(self,argin) :
        self.__roi2spectrumMgr.setRoiMode(*argin)

    def clearAllRoi(self):
        self.__roi2spectrumMgr.clearAllRoi()

    def setMaskFile(self,argin) :
        mask = getDataFromFile(argin)
        self.__roi2spectrumMgr.setMask(mask)
    
    def readImage(self,argin) :
        roiId,fromImageId = argin
        startImage,data = self.__roi2spectrumMgr.createImage(roiId,fromImageId)
        #Overflow
        if fromImageId >= 0 and startImage != fromImageId :
            raise 'Overrun ask id %d, given id %d (no more in memory' % (fromImageId,startImage)
        self._data_cache = data         # Tango is not so beautiful
        return data.buffer.ravel()
    
    def __get_roi_list_from_argin(self,argin) :
        rois = []
        for x,y,w,h in itertools.izip(itertools.islice(argin,0,len(argin),4),
                                      itertools.islice(argin,1,len(argin),4),
                                      itertools.islice(argin,2,len(argin),4),
                                      itertools.islice(argin,3,len(argin),4)) :
            roi = Core.Roi(x,y,w,h)
            rois.append(roi)
        return rois
#==================================================================
#
#    Roi2spectrumClass class definition
#
#==================================================================
class Roi2spectrumDeviceServerClass(PyTango.DeviceClass):

    #	 Class Properties
    class_property_list = {
	}


    #	 Device Properties
    device_property_list = {
	}


    #	 Command definitions
    cmd_list = {
        'add':
        [[PyTango.DevVarLongArray,"roi vector [x0,y0,width0,height0,x1,y1,width1,heigh1,...]"],
         [PyTango.DevVoid,""]],
        'set':
        [[PyTango.DevVarLongArray,"roi vector [x0,y0,width0,height0,x1,y1,width1,heigh1,...]"],
	[PyTango.DevVoid,""]],
        'get':
        [[PyTango.DevVoid,""],
        [PyTango.DevVarLongArray,"roi vector [x0,y0,width0,height0,x1,y1,width1,heigh1,...]"]],
        'clearAllRoi':
        [[PyTango.DevVoid,""],
         [PyTango.DevVoid,""]],
##        'setMaskFile':
##        [[PyTango.DevVarStringArray,"Full path of mask file"],
##         [PyTango.DevVoid,""]],
        'readImage':
        [[PyTango.DevVarLongArray,"[roiId,from which frame"],
         [PyTango.DevVarLongArray,"The image"]],
	'Start':
	[[PyTango.DevVoid,""],
	 [PyTango.DevVoid,""]],
	'Stop':
	[[PyTango.DevVoid,""],
	 [PyTango.DevVoid,""]],
        'getRoiMode' :
        [[PyTango.DevVoid,""],
         [PyTango.DevVarLongArray,"roi list mode"]],
        'setRoiMode':
        [[PyTango.DevVarLongArray,"roiId,mode"],
         [PyTango.DevVoid,""]],
	}


    #	 Attribute definitions
    attr_list = {
	'BufferSize':
	    [[PyTango.DevLong,
	    PyTango.SCALAR,
	    PyTango.READ_WRITE]],
	'CounterStatus':
	    [[PyTango.DevLong,
	    PyTango.SCALAR,
	    PyTango.READ]],
	'RunLevel':
	    [[PyTango.DevLong,
	    PyTango.SCALAR,
	    PyTango.READ_WRITE]],
	}


#------------------------------------------------------------------
#    Roi2spectrumDeviceServerClass Constructor
#------------------------------------------------------------------
    def __init__(self, name):
	PyTango.DeviceClass.__init__(self, name)
	self.set_type(name);



_control_ref = None
def set_control_ref(control_class_ref) :
    global _control_ref
    _control_ref= control_class_ref

def get_tango_specific_class_n_device() :
   return Roi2spectrumDeviceServerClass,Roi2spectrumDeviceServer
