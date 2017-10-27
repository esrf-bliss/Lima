###########################################################################
# This file is part of LImA, a Library for Image Acquisition
#
#  Copyright (C) : 2009-2017
#  European Synchrotron Radiation Facility
#  BP 220, Grenoble 38043
#  FRANCE
# 
#  Contact: lima@esrf.fr
# 
#  This is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
# 
#  This software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
from Lima import Core,Simulator

cam=Simulator.Camera()
hwint=Simulator.Interface(cam)
ct=Core.CtControl(hwint)
saving=ct.saving()
acq=ct.acquisition()

saving.setDirectory('c:\\temp'.encode())
saving.setPrefix('test_h5_'.encode())
saving.setFormat(saving.HDF5)
saving.setSuffix('.h5'.encode())
saving.setSavingMode(saving.AutoFrame)
acq.setAcqNbFrames(10)
saving.setFramesPerFile(10)

# set overwrite mode to save multiple dataset in a single file
saving.setOverwritePolicy(saving.MultiSet)

#ct.prepareAcq()
#ct.startAcq()