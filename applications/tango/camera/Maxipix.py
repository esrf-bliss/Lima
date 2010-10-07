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
import sys


class Maxipix(PyTango.Device_4Impl):
    def __init__(self,*args) :
        PyTango.Device_4Impl.__init__(self,*args)
        self.init_device()

   def delete_device(self) :
        pass

    def init_device(self):
        self.set_state(PyTango.DevState.ON)
        self.get_device_properties(self.get_device_class())
        _PriamAcq = _MaxipixAcq.getPriamAcq()
        self.__SignalLevel = {'LOW_FALL': _PriamAcq.SignalLevel.LOW_FALL,\
                              'HIGH_RISE': _PriamAcq.SignalLevel.HIGH_RISE}
        self.__ReadyMode = {'EXPOSURE': _PriamAcq.ReadyMode.EXPOSURE,\
                            'EXPOSURE_READOUT': _PriamAcq.ReadyMode.EXPOSURE_READOUT}
        self.__GateMode = {'INACTIVE': _PriamAcq.GateMode.INACTIVE,\
                            'ACTIVE': _PriamAcq.GateMode.ACTIVE}

        #init MpxAcq with espia device board number
	#if not self.espia_dev_nb:
	#    #No property set, default will be board #0
	#    self.espia_dev_nb = 0
	#_MaxipixAcq.init(self.espia_dev_nb)
	
        #Init default Path
        if self.config_path:
            _MaxipixAcq.setPath(self.config_path)

        #Load default config
        if self.config_name:
            _MaxipixAcq.loadConfig(self.config_name)
	    
	#set the reconstruction fill mode
	if self.fill_mode:
	    _MaxipixAcq.setFillMode(self.fill_mode)
	else:
	    self.fill_mode = _MaxipixAcq.getFillMode()


        _PriamAcq = _MaxipixAcq.getPriamAcq()


	#set the ready_mode 
        for att_name in ['ready_mode','ready_level','gate_mode','gate_level','shutter_level','trigger_level'] :
            self.__applyPriamAcqAttr(att_name,None)



    def __getDictKey(self,dict, value):
        try:
            ind = dict.values().index(value)                            
        except ValueError:
            return None
        return dict.keys(ind)

    def __getDictValue(self,dict, key):
        try:
            value = dict[key.upper()]
        except KeyError:
            return None
        return value

    def __getPriamAcqAttr(self,attr_name):

        _PriamAcq = _MaxipixAcq.getPriamAcq()
	name = ''.join([name.capitalize() for name in attr_name.split('_')])
        attr = getattr(self,attr_name)
        if attr_name.count('level'):
           dictInstance = self.__SignalLevel
        else:
           dictInstance = getattr(self,'__%s' % name)
        getMethod = getattr(_PriamAcq,'get%s' % name)
        return self.getDictKey(dictInstance,getMethod())


    def __setPriamAcqAttr(self,attr_name, value=None):

        _PriamAcq = _MaxipixAcq.getPriamAcq()
	name = ''.join([name.capitalize() for name in attr_name.split('_')])
        attr = getattr(self,attr_name)
        if attr_name.count('level'):
           dictInstance = self.__SignalLevel
        else:
           dictInstance = getattr(self,'__%s' % name)
        getMethod = getattr(_PriamAcq,'get%s' % name)
        setMethod = getattr(_PriamAcq,'set%s' % name)
       
        if value:
        # just set a new value for this attribute
            attr_value = self.__getDictValue(dictInstance,value)
            attr = attr_value
            setMethod(attr_value)
        else:
        # here set attribute from the property value
        # if the property is missing then initialize the attribute by reading the hardware
            if  attr is not None:
                attr_value = self.__getDictValue(dictInstance,attr)
                if attr_value is None:
                    raise PyTango.DevFailed('Wrong value %s: %s'%s(attr_name,attr_value)  
	        setMethod(attr_value)
	    else:
	        attr = self.__getDictKey(getMethod())

	            
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
        fill_mode = ""
        if self.fill_mode:
            fill_mode = self.fill_mode
        attr.set_value(fill_mode)

    ## @brief Write the gap fill mode
    #
    def write_fill_mode(self,attr) :
        data = []
        attr.get_write_value(data)
	
        _MaxipixAcq.setFillMode(data[0])
        self.fill_mode = data[0]

    ## @brief read the fill mode
    #
    def read_espia_dev_nb(self,attr) :
        espia_dev_nb = 0
        if self.espia_dev_nb:
            espia_dev_nb = self.espia_dev_nb
        attr.set_value(espia_dev_nb)


    ## @brief read the ready_mode
    # EXPSURE-0, EXPOSURE_READOUT-1
    def read_ready_mode(self,attr) :
        ready_mode  = self.__getPriamAcqAttr('ready_mode')
        attr.set_value(ready_mode)

    ## @brief Write the ready_mode
    # EXPSURE-0, EXPOSURE_READOUT-1
    def write_ready_mode(self,attr) :
        data = []
        attr.get_write_value(data)
        self.__setPriamAcqAttr('ready_mode',data[0])

    ## @brief read the ready_level
    # LOW_FALL-0, HIGH_RISE-1
    def read_ready_level(self,attr) :
        ready_level  = self.__getPriamAcqAttr('ready_level')
        attr.set_value(ready_level)

    ## @brief Write the ready_level
    # LOW_FALL-0, HIGH_RISE-1
    def write_ready_level(self,attr) :
        data = []
        attr.get_write_value(data)
        self.__setPriamAcqAttr('ready_level',data[0])

    ## @brief read the shutter_level
    # LOW_FALL-0, HIGH_RISE-1
    def read_shutter_level(self,attr) :
        shutter_level  = self.__getPriamAcqAttr('shutter_level')
        attr.set_value(shutter_level)

    ## @brief Write the shutter_level
    # LOW_FALL-0, HIGH_RISE-1
    def write_shutter_level(self,attr) :
        data = []
        attr.get_write_value(data)
        self.__setPriamAcqAttr('shutter_level',data[0])

    ## @brief read the gate_mode
    # FRAME-0, SEQUENCE-1
    def read_gate_mode(self,attr) :
        gate_mode  = self.__getPriamAcqAttr('gate_mode')
        attr.set_value(gate_mode)

    ## @brief Write the gate_mode
    # FRAME-0, SEQUENCE-1
    def write_gate_mode(self,attr) :
        data = []
        attr.get_write_value(data)
        self.__setPriamAcqAttr('gate_mode',data[0])
	
    ## @brief read the gate_level
    # LOW_FALL-0, HIGH_RISE-1
    def read_gate_level(self,attr) :
        gate_level  = self.__getPriamAcqAttr('gate_level')
        attr.set_value(gate_level)

    ## @brief Write the gate_level
    # LOW_FALL-0, HIGH_RISE-1
    def write_gate_level(self,attr) :
        data = []
        attr.get_write_value(data)
        self.__setPriamAcqAttr('gate_level',data[0])
	
    ## @brief read the trigger_level
    # LOW_FALL-0, HIGH_RISE-1
    def read_trigger_level(self,attr) :
        trigger_level  = self.__getPriamAcqAttr('gate_level')
        attr.set_value(trigger_level)

    ## @brief Write the trigger_level
    # LOW_FALL-0, HIGH_RISE-1
    def write_trigger_level(self,attr) :
        data = []
        attr.get_write_value(data)
        self.__setPriamAcqAttr('trigger_level',data[0])
	

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
        [PyTango.DevShort,
         "The ready output signal level",[]],	  
       'gate_level':
        [PyTango.DevShort,
         "The gate output signal level",[]],	  
       'shutter_level':
        [PyTango.DevShort,
         "The shutter output signal level",[]],	  
       'trigger_level':
        [PyTango.DevShort,
         "The trigger output signal level",[]],	  
       'ready_mode':
        [PyTango.DevShort,
         "The ready output signal level",[]],	  
       'gate_mode':
        [PyTango.DevShort,
         "The gate output signal level",[]],	  
       'shutter_mode':
        [PyTango.DevShort,
         "The shutter output signal level",[]],	  

        }

    cmd_list = {}

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

