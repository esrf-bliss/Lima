############################################################################
# This file is part of gldisplay, a submodule of LImA project the
# Library for Image Acquisition
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
# along with this program; if not, see <http:#www.gnu.org/licenses/>.
############################################################################

import sys
import time
import weakref
import threading
import getopt

from Lima import Core
from Lima import Simulator

Core.DEB_GLOBAL(Core.DebModTest)

class TestControl:
	Core.DEB_CLASS(Core.DebModTest, 'TestControl')

	class ImageStatusCallback(Core.CtControl.ImageStatusCallback):
		def __init__(self, test_control, cb_end):
			Core.CtControl.ImageStatusCallback.__init__(self)
			self.test_control = weakref.ref(test_control)
			self.cb_end = cb_end

		def imageStatusChanged(self, image_status):
			test_control = self.test_control()
			if not test_control:
				return
			self.cb_end.clear()
			test_control.imageStatusChanged(image_status)
			del test_control
			self.cb_end.set()

	@Core.DEB_MEMBER_FUNCT
	def __init__(self):
		self.simu = Simulator.Camera()
		self.simu_hw = Simulator.Interface(self.simu)
		self.ct_control = Core.CtControl(self.simu_hw)
		self.cb_end = threading.Event()
		self.cb = self.ImageStatusCallback(self, self.cb_end)
		self.ct_control.registerImageStatusCallback(self.cb)
		self.acq_state = Core.AcqState()

	@Core.DEB_MEMBER_FUNCT
	def __del__(self):
		del self.ct_control
		del self.simu_hw

	@Core.DEB_MEMBER_FUNCT
	def start(self, exp_time, nb_frames, prepare_timeout, sleep_time):
		ct_acq = self.ct_control.acquisition()
		ct_acq.setAcqExpoTime(exp_time)
		ct_acq.setAcqNbFrames(nb_frames)
		self.sleep_time = sleep_time
		self.ct_control.setPrepareTimeout(prepare_timeout)
		self.ct_control.prepareAcq()
		deb.Always('prepareAcq finished')
		self.acq_state.set(Core.AcqState.Acquiring)
		self.ct_control.startAcq()

	@Core.DEB_MEMBER_FUNCT
	def waitAcq(self):
		self.acq_state.waitNot(Core.AcqState.Acquiring)

	@Core.DEB_MEMBER_FUNCT
	def sync(self):
		self.waitAcq()
		self.cb_end.wait()

	@Core.DEB_MEMBER_FUNCT
	def imageStatusChanged(self, img_status):
		if self.acq_state.get() != Core.AcqState.Acquiring:
			return

		ct_acq = self.ct_control.acquisition()
		nb_frames = ct_acq.getAcqNbFrames()
		last_img_ready = img_status.LastImageReady
		if last_img_ready == nb_frames - 1:
			deb.Always('Acq. is ready')
			self.acq_state.set(Core.AcqState.Idle)

		time.sleep(self.sleep_time)

		deb.Always('Forcing read frame %d memory' % last_img_ready)
		image = self.ct_control.ReadImage(last_img_ready)
		data = ' ' + image.buffer.tostring()


class TestControlAutoSync:
	Core.DEB_CLASS(Core.DebModTest, 'TestControlAutoSync')

	def __init__(self):
		self.test_control = TestControl()
		self.traceRefCount(1)

	def __del__(self):
		self.traceRefCount(2)
		self.test_control.sync()
		self.traceRefCount(3)

	@Core.DEB_MEMBER_FUNCT
	def traceRefCount(self, point):
		deb.Trace("%s* refcount(test_control): %s" % 
			  (point, sys.getrefcount(self.test_control)))

	def __getattr__(self, name):
		return getattr(self.test_control, name)


@Core.DEB_FUNCT
def main(argv):
	verbose = False

	opts, args = getopt.getopt(argv[1:], 'v')
	for opt, val in opts:
		if opt == '-v':
			verbose = True

	deb_type_flags = Core.DebParams.AllFlags if verbose else 0
	Core.DebParams.setTypeFlags(deb_type_flags)

	exp_time = 1e-3

	test_control = TestControlAutoSync()

	i = 0
	nb_frames = 2
	prepare_timeout = 0.2
	sleep_time = 0.1
	deb.Always("AcqNb: %s" % i)
	test_control.start(exp_time, nb_frames, prepare_timeout, sleep_time)
	test_control.waitAcq()

	i += 1
	deb.Always("AcqNb: %s" % i)
	nb_frames = 1
	sleep_time = 0.5
	test_control.start(exp_time, nb_frames, prepare_timeout, sleep_time)
	test_control.waitAcq()

	i += 1
	deb.Always("AcqNb: %s" % i)
	err = 'ImageStatusCallback still active'
	ok = False
	try:
		test_control.start(exp_time, nb_frames, prepare_timeout, 
				   sleep_time)
	except Core.Exception, e:
		if err in e.args[0]:
			ok = True
			deb.Always('Got good exception: %s' % e)
	if not ok:
		raise RuntimeError, 'Expected exception: %s' % err

if __name__ == '__main__':
	main(sys.argv)
