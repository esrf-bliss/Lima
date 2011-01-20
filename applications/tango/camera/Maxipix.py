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
#=============================================================================
#
# file :        Maxipix.py
#
# description : Python source for the Maxipix and its commands. 
#                The class is derived from Device. It represents the
#                CORBA servant object which will be accessed from the
#                network. All commands which can be executed on the
#                Pilatus are implemented in this file.
#
# project :     TANGO Device Server
#
# copyleft :    European Synchrotron Radiation Facility
#               BP 220, Grenoble 38043
#               FRANCE
#
#=============================================================================
#         (c) - Bliss - ESRF
#=============================================================================
#
import PyTango
import sys, types, os, time

from Lima import Core
from Lima.Maxipix.MpxCommon import MpxError

class Maxipix(PyTango.Device_4Impl):

    Core.DEB_CLASS(Core.DebModApplication, 'LimaCCDs')
    

#------------------------------------------------------------------
#    Device constructor
#------------------------------------------------------------------
    def __init__(self,*args) :
        PyTango.Device_4Impl.__init__(self,*args)


        self.init_device()

#------------------------------------------------------------------
#    Device destructor
#------------------------------------------------------------------
    def delete_device(self):
        pass

#------------------------------------------------------------------
#    Device initialization
#------------------------------------------------------------------
    @Core.DEB_MEMBER_FUNCT
    def init_device(self):
        self.set_state(PyTango.DevState.ON)
        self.get_device_properties(self.get_device_class())
        
	_PriamAcq = _MaxipixAcq.getPriamAcq()
        self.__SignalLevel = {'LOW_FALL': _PriamAcq.LOW_FALL,\
                              'HIGH_RISE': _PriamAcq.HIGH_RISE}
        self.__ReadyMode =   {'EXPOSURE': _PriamAcq.EXPOSURE,\
                              'EXPOSURE_READOUT': _PriamAcq.EXPOSURE_READOUT}
        self.__GateMode =    {'INACTIVE': _PriamAcq.INACTIVE,\
                              'ACTIVE': _PriamAcq.ACTIVE}
	self.__FillMode =    _MaxipixAcq.mpxFillModes
        
        #Init default Path
        if self.config_path:
            try:
                _MaxipixAcq.setPath(self.config_path)
            except MpxError as error:
                PyTango.Except.throw_exception('DevFailed',\
                                               'MpxError: %s'%(error),\
                                               'Maxipix Class') 


        #Load default config
        if self.config_name:
            try:
                _MaxipixAcq.loadConfig(self.config_name)
            except MpxError as error:
                PyTango.Except.throw_exception('DevFailed',\
                                               'MpxError: %s'%(error),\
                                               'Maxipix Class') 
	    
	#set the priamAcq attributes with properties if any 
        for attName in ['fill_mode','ready_mode','ready_level','gate_mode','gate_level','shutter_level','trigger_level'] :
            self.__setMaxipixAttr(attName,None)


#==================================================================
# 
# Some Utils
#
#==================================================================

    def __getDictKey(self,dict, value):
        try:
            ind = dict.values().index(value)                            
        except ValueError:
            return None
        return dict.keys()[ind]

    def __getDictValue(self,dict, key):
        try:
            value = dict[key.upper()]
        except KeyError:
            return None
        return value

    def __getMaxipixAttr(self,attr_name):

        _PriamAcq = _MaxipixAcq.getPriamAcq()
	name = ''.join([name.capitalize() for name in attr_name.split('_')])
        attr = getattr(self,attr_name)
        if attr_name.count('level'):
           dictInstance = self.__SignalLevel
        else:
           dictInstance = getattr(self,'_Maxipix__%s' % name)
        if attr_name.count('fill_mode'): getMethod = getattr(_MaxipixAcq,'get%s' % name)
        else: getMethod = getattr(_PriamAcq,'get%s' % name)
        setattr(self,attr_name, self.__getDictKey(dictInstance,getMethod()))
        return getattr(self,attr_name)

    def __getValueList(self, attr_name):
	name = ''.join([name.capitalize() for name in attr_name.split('_')])
        if attr_name.count('level'):
            valueList = self.__SignalLevel.keys()
        elif attr_name.count('mode'):
            valueList = getattr(self,'_Maxipix__%s' % name).keys()
        elif attr_name.count('config_name'):
            valueList = self.__getConfigNameList()
        else:
            valueList = []

        return valueList

    def __setMaxipixAttr(self,attr_name, key=None):

        _PriamAcq = _MaxipixAcq.getPriamAcq()
	name = ''.join([name.capitalize() for name in attr_name.split('_')])
        attr = getattr(self,attr_name)
        if attr_name.count('level'):
            dictInstance = self.__SignalLevel
        else:
            dictInstance = getattr(self,'_Maxipix__%s' % name)
        if attr_name.count('fill_mode'):
            getMethod = getattr(_MaxipixAcq,'get%s' % name)
            setMethod = getattr(_MaxipixAcq,'set%s' % name)
        else:
            getMethod = getattr(_PriamAcq,'get%s' % name)
            setMethod = getattr(_PriamAcq,'set%s' % name)
            
        if key != None:
            # just set a new value for this attribute
            attrValue = self.__getDictValue(dictInstance,key)
            if attrValue == None:
                PyTango.Except.throw_exception('DevFailed',\
                                               'Wrong value %s: %s'%(attr_name,key),\
                                               'Maxipix Class')
            else:
                setMethod(attrValue)
                attrNewKey = key     
        else:
            # here set attribute from the property value
            # if the property is missing (=[])i then initialize the attribute by reading the hardware
            if attr == []:
	        attrNewKey = self.__getDictKey(dictInstance,getMethod())
            elif type(attr) is not types.StringType:
	        PyTango.Except.throw_exception('WrongData',\
                                               'Wrong value %s: %s'%(attr_name,attr),\
                'Maxipix Class')
            else:
                attrValue = self.__getDictValue(dictInstance,attr)
                if attrValue == None:
                    PyTango.Except.throw_exception('WrongData',\
                                                   'Wrong value %s: %s'%(attr_name,attr),\
                                                   'Maxipix Class')
                else:
                    setMethod(attrValue)
                    attrNewKey = attr
        # set the new attribute value as upper string
        setattr(self,attr_name, attrNewKey.upper())

    def __getConfigNameList(self):
        spath= os.path.normpath(self.config_path)
        if not os.path.isdir(spath):
            PyTango.Except.throw_exception('WrongData',\
                                           'Invalid path: %s'%(self.config_path),\
                                           'Maxipix Class') 

        else:
            dirList = os.listdir(spath)
            fileDict={}
            fileList=[]
            for file in dirList:
                if file.endswith('.cfg'):
                    filePath = spath+'/'+file
                    fileStat = os.stat(filePath)
                    modifiedTime = fileStat.st_mtime
                    fileDict[modifiedTime]= file.strip('.cfg')
        if fileDict:
            timeList = fileDict.keys();timeList.sort()
            for mTime in timeList:
                fileList.append(fileDict[mTime])
                #fileList.append(time.ctime(mTime))
        return fileList
                     
        
#==================================================================
#
#    Maxipix read/write attribute methods
#
#==================================================================
	            
    ## @brief Read threshold noise of a maxipix chips
    #
    def read_threshold_noise(self,attr) :
        dac = _MaxipixAcq.mpxDacs
        thlNoises = dac.getThlNoise(0)

        attr.set_value(thlNoises,len(thlNoises))
        
    ## @brief Write threshold noise of a maxipix chips
    #
    def write_threshold_noise(self,attr) :
        data = []
        attr.get_write_value(data)

        dac = _MaxipixAcq.mpxDacs
        dac.setThlNoise(0,data)

    ## @brief Read the global threshold
    #
    def read_threshold(self,attr) :
        dac = _MaxipixAcq.mpxDacs
        thl = dac.getThl()
	if thl is None: thl = -1

        attr.set_value(thl)

    ## @brief Write the global threshold
    #
    def write_threshold(self,attr) :
        data = []
        attr.get_write_value(data)
        
        dac = _MaxipixAcq.mpxDacs
        dac.setThl(data[0])
	_MaxipixAcq.applyChipFsr(0)

    ## @brief Read the energy step
    #
    # energy step is the coef which link the global threshold with energy
    # threshold
    # 
    def read_energy_calibration(self,attr) :
        dac = _MaxipixAcq.mpxDacs
        values = dac.getECalibration()
        
        attr.set_value(values,len(values))
        
    ## @brief Write the energy step
    #
    def write_energy_calibration(self,attr) :
        data = []
        attr.get_write_value(data)

        dac = _MaxipixAcq.mpxDacs
        dat.setECalibration(data)

    ## @brief Read the energy threshold
    #
    # energy_threshold = energy_step * threshold (global)
    def read_energy_threshold(self,attr) :
        dac = _MaxipixAcq.mpxDacs
        value = dac.getEThl()
	if value is None: value = -1
	
        attr.set_value(value)

    ## @brief Write the energy threshold
    #
    def write_energy_threshold(self,attr) :
        data = []
        attr.get_write_value(data)
        
        dac = _MaxipixAcq.mpxDacs
        dac.setEThl(data[0])
	_MaxipixAcq.applyChipFsr(0)
        
    ## @brief read the config name
    #
    def read_config_name(self,attr) :
        cfg_name = ""
        if self.config_name:
            cfg_name = self.config_name
        attr.set_value(cfg_name)

    ## @brief Write the config name and load it
    #
    def write_config_name(self,attr) :
        data = []
        attr.get_write_value(data)
        _MaxipixAcq.loadConfig(data[0])
        self.config_name = data[0]

    ## @brief read the config path
    #
    def read_config_path(self,attr) :
        cfg_path = ""
        if self.config_path:
            cfg_path = self.config_path
        attr.set_value(cfg_path)

    ## @brief Write the config path
    #
    def write_config_path(self,attr) :
        data = []
        attr.get_write_value(data)
        _MaxipixAcq.setPath(data[0])
        self.config_path = data[0]

    ## @brief read the fill mode
    #
    def read_fill_mode(self,attr) :
        fill_mode  = self.__getMaxipixAttr('fill_mode')
        attr.set_value(fill_mode)

    ## @brief Write the gap fill mode
    #
    def write_fill_mode(self,attr) :
        data = []
        attr.get_write_value(data)
        self.__setMaxipixAttr('fill_mode',data[0])


    ## @brief read the board id
    #
    def read_espia_dev_nb(self,attr) :
        espia_dev_nb = 0
        if self.espia_dev_nb:
            espia_dev_nb = self.espia_dev_nb
        attr.set_value(espia_dev_nb)


    ## @brief read the ready_mode
    # EXPOSURE-0, EXPOSURE_READOUT-1
    def read_ready_mode(self,attr) :
        ready_mode  = self.__getMaxipixAttr('ready_mode')
        attr.set_value(ready_mode)

    ## @brief Write the ready_mode
    # EXPOSURE-0, EXPOSURE_READOUT-1
    def write_ready_mode(self,attr) :
        data = []
        attr.get_write_value(data)
        self.__setMaxipixAttr('ready_mode',data[0])

    ## @brief read the ready_level
    # LOW_FALL-0, HIGH_RISE-1
    def read_ready_level(self,attr) :
        ready_level  = self.__getMaxipixAttr('ready_level')
        attr.set_value(ready_level)

    ## @brief Write the ready_level
    # LOW_FALL-0, HIGH_RISE-1
    def write_ready_level(self,attr) :
        data = []
        attr.get_write_value(data)
        self.__setMaxipixAttr('ready_level',data[0])

    ## @brief read the shutter_level
    # LOW_FALL-0, HIGH_RISE-1
    def read_shutter_level(self,attr) :
        shutter_level  = self.__getMaxipixAttr('shutter_level')
        attr.set_value(shutter_level)

    ## @brief Write the shutter_level
    # LOW_FALL-0, HIGH_RISE-1
    def write_shutter_level(self,attr) :
        data = []
        attr.get_write_value(data)
        self.__setMaxipixAttr('shutter_level',data[0])

    ## @brief read the gate_mode
    # FRAME-0, SEQUENCE-1
    def read_gate_mode(self,attr) :
        gate_mode  = self.__getMaxipixAttr('gate_mode')
        attr.set_value(gate_mode)

    ## @brief Write the gate_mode
    # FRAME-0, SEQUENCE-1
    def write_gate_mode(self,attr) :
        data = []
        attr.get_write_value(data)
        self.__setMaxipixAttr('gate_mode',data[0])
	
    ## @brief read the gate_level
    # LOW_FALL-0, HIGH_RISE-1
    def read_gate_level(self,attr) :
        gate_level  = self.__getMaxipixAttr('gate_level')
        attr.set_value(gate_level)

    ## @brief Write the gate_level
    # LOW_FALL-0, HIGH_RISE-1
    def write_gate_level(self,attr) :
        data = []
        attr.get_write_value(data)
        self.__setMaxipixAttr('gate_level',data[0])
	
    ## @brief read the trigger_level
    # LOW_FALL-0, HIGH_RISE-1
    def read_trigger_level(self,attr) :
        trigger_level  = self.__getMaxipixAttr('gate_level')
        attr.set_value(trigger_level)

    ## @brief Write the trigger_level
    # LOW_FALL-0, HIGH_RISE-1
    def write_trigger_level(self,attr) :
        data = []
        attr.get_write_value(data)
        self.__setMaxipixAttr('trigger_level',data[0])

#==================================================================
#
#    Maxipix command methods
#
#==================================================================
#------------------------------------------------------------------
#    getAttrStringValueList command:
#
#    Description: return a list of authorized values if any
#    argout: DevVarStringArray   
#------------------------------------------------------------------
    @Core.DEB_MEMBER_FUNCT
    def getAttrStringValueList(self, attr_name):

        valueList = self.__getValueList(attr_name)
        return valueList

#------------------------------------------------------------------
#    setDebugFlags command:
#
#    Description: Get the current acquired frame number
#    argout: DevVarDoubleArray    
#------------------------------------------------------------------
    @Core.DEB_MEMBER_FUNCT
    def setDebugFlags(self, deb_flags):
        deb_flags &= 0xffffffff
        deb.Param('Setting debug flags: 0x%08x' % deb_flags)
        Core.DebParams.setTypeFlags((deb_flags   >> 16)  & 0xff)
        Core.DebParams.setModuleFlags((deb_flags >>  0)  & 0xffff)

        deb.Trace('FormatFlags: %s' % Core.DebParams.getFormatFlagsNameList())
        deb.Trace('TypeFlags:   %s' % Core.DebParams.getTypeFlagsNameList())
        deb.Trace('ModuleFlags: %s' % Core.DebParams.getModuleFlagsNameList())

#------------------------------------------------------------------
#    getDebugFlags command:
#
#    Description: Get the current acquired frame number
#    argout: DevVarDoubleArray    
#------------------------------------------------------------------
    @Core.DEB_MEMBER_FUNCT
    def getDebugFlags(self):
        deb.Trace('FormatFlags: %s' % Core.DebParams.getFormatFlagsNameList())
        deb.Trace('TypeFlags:   %s' % Core.DebParams.getTypeFlagsNameList())
        deb.Trace('ModuleFlags: %s' % Core.DebParams.getModuleFlagsNameList())
        
        deb_flags = (((Core.DebParams.getTypeFlags()    & 0xff)   << 16) |
                     ((Core.DebParams.getModuleFlags()  & 0xffff) <<  0))
        deb_flags &= 0xffffffff
        deb.Return('Getting debug flags: 0x%08x' % deb_flags)
        return deb_flags

class MaxipixClass(PyTango.DeviceClass):

    class_property_list = {}

    device_property_list = {
        'espia_dev_nb':
        [PyTango.DevShort,
         "Espia board device number",[]],
        'config_path':
        [PyTango.DevString,
         "Path where configuration files are",[]],
        'config_name':
        [PyTango.DevString,
         "The default configuration loaded",[]],
        'fill_mode':
        [PyTango.DevString,
         "The default configuration loaded",[]],	 
       'ready_level':
        [PyTango.DevString,
         "The ready output signal level",[]],	  
       'gate_level':
        [PyTango.DevString,
         "The gate output signal level",[]],	  
       'shutter_level':
        [PyTango.DevString,
         "The shutter output signal level",[]],	  
       'trigger_level':
        [PyTango.DevString,
         "The trigger output signal level",[]],	  
       'ready_mode':
        [PyTango.DevString,
         "The ready output signal level",[]],	  
       'gate_mode':
        [PyTango.DevString,
         "The gate output signal level",[]],	  
        }

    cmd_list = {
        'getAttrStringValueList':
        [[PyTango.DevString, "Attribute name"],
         [PyTango.DevVarStringArray, "Authorized String value list"]],
        'getDebugFlags':
        [[PyTango.DevVoid, ""],
         [PyTango.DevULong, "Debug flag in HEX format"]],
        'setDebugFlags':
        [[PyTango.DevULong, "Debug flag in HEX format"],
         [PyTango.DevVoid, ""]],
	}

    attr_list = {
        'threshold_noise':
        [[PyTango.DevLong,
          PyTango.SPECTRUM,
          PyTango.READ_WRITE,5]],
        'threshold':
        [[PyTango.DevLong,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],
        'energy_calibration':
        [[PyTango.DevDouble,
          PyTango.SPECTRUM,
          PyTango.READ_WRITE,5]],
        'energy_threshold':
        [[PyTango.DevDouble,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],
        'config_name':
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],
        'config_path':
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],
        'fill_mode':	  
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],	  
        'espia_dev_nb':	  
        [[PyTango.DevShort,
          PyTango.SCALAR,
          PyTango.READ]],	  
        'ready_mode':	  
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],	  
        'ready_level':	  
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],	  
        'shutter_level':	  
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],	  
        'gate_mode':	  
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],	  
        'gate_level':	  
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],	  
        'trigger_level':	  
        [[PyTango.DevString,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],	  
        }


    def __init__(self,name) :
        PyTango.DeviceClass.__init__(self,name)
        self.set_type(name)


#----------------------------------------------------------------------------
#                              Plugins
#----------------------------------------------------------------------------
from Lima.Maxipix.MpxAcq import MpxAcq

_MaxipixAcq = None

def get_control(espia_dev_nb = '0',**keys) :
    #properties are passed here as string
    global _MaxipixAcq
    if _MaxipixAcq is None:
        _MaxipixAcq = MpxAcq(int(espia_dev_nb))
    return _MaxipixAcq.getControl()

def close_interface() :
    global _MaxipixAcq
    _MaxipixAcq = None
    
def get_tango_specific_class_n_device():
    return MaxipixClass,Maxipix

