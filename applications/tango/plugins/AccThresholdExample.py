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
from Lima import Core

##@brief this is an plugin example for accumulation saturation callback
#
#To use this plugin, you have to set AccThresholdCallbackModule property in
#LimaCCD device to "AccThresholdExample".
#AccThresholdCallbackModule should be the filename of your plugin.
class _ExampleThreshold(Core.CtAccumulation.ThresholdCallback):
    Core.DEB_CLASS(Core.DebModApplication, '_ExampleThreshold')
    def __init__(self) :
        Core.CtAccumulation.ThresholdCallback.__init__(self)
    ##@brief the effective callback methode
    @Core.DEB_MEMBER_FUNCT
    def aboveMax(self,data,value) :
        deb.Trace('aboveMax: data=%s, value=%s' % (data,value))

##@brief this function is mandatory for an AccThresholdCallbackModule
#
def get_acc_threshold_callback() :
    return _ExampleThreshold()
