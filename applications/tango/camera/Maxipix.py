#=============================================================================
#
# file :        Pilatus.py
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

        #Init default Path
        if self.config_path:
            _MaxipixAcq.setPath(self.config_path)

        #Load default config
        if self.config_name:
            _MaxipixAcq.loadConfig(self.config_name)
        
    ## @brief Read threshold noise of a maxipix chips
    #
    def read_threshold_noise(self,attr) :
        dac = _MaxipixAcq.getDacs()
        thlNoises = dac.getThlNoise(0)

        attr.set_value(thlNoises,len(thlNoises))
        
    ## @brief Write threshold noise of a maxipix chips
    #
    def write_threshold_noise(self,attr) :
        data = []
        attr.get_write_value(data)

        dac = _MaxipixAcq.getDacs()
        dac.setThlNoise(0,*data)

    ## @brief Read the global threshold
    #
    def read_threshold(self,attr) :
        dac = _MaxipixAcq.getDacs()
        thl = dac.getThl()

        attr.set_value(thl)

    ## @brief Write the global threshold
    #
    def write_threshold(self,attr) :
        data = []
        attr.get_write_value(data)
        
        dac = _MaxipixAcq.getDacs()
        dac.setThl(data[0])

    ## @brief Read the energy step
    #
    # energy step is the coef which link the global threshold with energy
    # threshold
    # 
    def read_energy_calibration(self,attr) :
        dac = _MaxipixAcq.getDacs()
        values = dat.getECalibration()
        
        attr.set_value(values,len(values))
        
    ## @brief Write the energy step
    #
    def write_energy_calibration(self,attr) :
        data = []
        attr.get_write_value(data)

        dac = _MaxipixAcq.getDacs()
        dat.setECalibration(*data)

    ## @brief Read the energy threshold
    #
    # energy_threshold = energy_step * threshold (global)
    def read_energy_threshold(self,attr) :
        dac = _MaxipixAcq.getDacs()
        value = dac.getEThl()
        attr.set_value(value)

    ## @brief Write the energy threshold
    #
    def write_energy_threshold(self,attr) :
        data = []
        attr.get_write_value(data)
        
        dac = _MaxipixAcq.getDacs()
        dac.setEThl(data[0])
        
    ## @brief read the config name
    #
    def read_config_name(self,attr) :
        cfg_name = ""
        if self.config_name:
            cfg_name = self.config_name
        attr.set_value(cfg_name)

    ## @brief Write the energy threshold
    #
    def write_config_name(self,attr) :
        data = []
        attr.get_write_value(data)
        _MaxipixAcq.loadConfig(data[0])
        self.config_name = data[0]


class MaxipixClass(PyTango.DeviceClass):

    class_property_list = {}

    device_property_list = {
        'config_path':
        [PyTango.DevString,
         "Path where configuration files are",[]],
        'config_name':
        [PyTango.DevString,
         "The default configuration loaded",[]],
        }

    cmd_list = {}

    attr_list = {
        'threshold_noise':
        [[PyTango.DevVarLongArray,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],
        'threshold':
        [[PyTango.DevLong,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],
        'energy_calibration':
        [[PyTango.DevVarDoubleArray,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],
        'energy_threshold':
        [[PyTango.DevDouble,
          PyTango.SCALAR,
          PyTango.READ_WRITE]],
        'config_name':
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

def get_control() :
    global _MaxipixAcq
    if _MaxipixAcq is None:
        _MaxipixAcq = MpxAcq(0)
    return _MaxipixAcq.getControl()

def close_interface() :
    global _MaxipixAcq
    _MaxipixAcq = None
    
def get_tango_specific_class_n_device():
    return MaxipixClass,Maxipix

