###########################################################################
# This file is part of LImA, a Library for Image Acquisition
#
#  Copyright (C) : 2009-2018
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

from Lima import Core, Simulator
import time

cam = Simulator.Camera()

frame_dim = Core.FrameDim(Core.Size(800, 600), Core.Bpp32)
cam = Simulator.Camera()
getter = cam.getFrameGetter()
getter.setFrameDim(frame_dim)

hwint = Simulator.Interface(cam)
control = Core.CtControl(hwint)

acq = control.acquisition()
acq.setAcqNbFrames(1)

img = control.image()
roi = Core.Roi(300, 50, 450, 300)
img.setRoi(roi)
# img.setBin(Core.Bin(2, 2))
img.setRotation(Core.Rotation_90)

control.prepareAcq()
control.startAcq()

while control.getStatus().AcquisitionStatus == Core.AcqRunning:
    time.sleep(0.1)

frame = control.ReadImage(0)

control.stopAcq()
