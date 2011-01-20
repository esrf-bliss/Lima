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
# file :        Frelon.py
#
# description : Python source for the Frelon and its commands.
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
from Lima import Core
from Lima import Frelon as FrelonAcq


class Frelon(PyTango.Device_4Impl):

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

    @Core.DEB_MEMBER_FUNCT
    def getAttrStringValueList(self, attr_name):
        valueList=[]
        dict_name = '_' + self.__class__.__name__ + '__' + ''.join([x.title() for x in attr_name.split('_')])
        d = getattr(self,dict_name,None)
        if d:
            valueList = d.keys()

        return valueList

    ## @brief read the espia board id
    #
    def read_espia_dev_nb(self,attr) :
        espia_dev_nb = 0
        if self.espia_dev_nb:
            espia_dev_nb = self.espia_dev_nb
        attr.set_value(espia_dev_nb)


class FrelonClass(PyTango.DeviceClass):

    class_property_list = {}

    device_property_list = {
        'espia_dev_nb':
        [PyTango.DevShort,
         "Espia board device number",[]],
        }

    cmd_list = {
        'getAttrStringValueList':
        [[PyTango.DevString, "Attribute name"],
         [PyTango.DevVarStringArray, "Authorized String value list"]],
        }

    attr_list = {
        'espia_dev_nb':
        [[PyTango.DevShort,
          PyTango.SCALAR,
          PyTango.READ]],
        }

    def __init__(self,name) :
        PyTango.DeviceClass.__init__(self,name)
        self.set_type(name)

#----------------------------------------------------------------------------
# Plugins
#----------------------------------------------------------------------------
_FrelonAcq = None

def get_control(espia_dev_nb = 0,**keys) :
    global _FrelonAcq
    if _FrelonAcq is None:
	_FrelonAcq = FrelonAcq.FrelonAcq(int(espia_dev_nb))
    return _FrelonAcq.getGlobalControl() 

def get_tango_specific_class_n_device():
    return FrelonClass,Frelon
