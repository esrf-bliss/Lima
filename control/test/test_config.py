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

from __future__ import print_function

import sys
import time

from Lima import Core
from Lima import Simulator

Core.DEB_GLOBAL(Core.DebModTest)


class TestConfig:

    Core.DEB_CLASS(Core.DebModTest, 'TestConfig')

    @Core.DEB_MEMBER_FUNCT
    def __init__(self):

        self.cam = Simulator.Camera()
        self.cam_hw = Simulator.Interface(self.cam)

        self.ct_control = Core.CtControl(self.cam_hw)
        self.ct_saving = self.ct_control.saving()
        self.ct_image = self.ct_control.image()
        self.ct_acq = self.ct_control.acquisition()
        self.ct_config = self.ct_control.config()
        self.conf_dict = {
            'conf1': {
                'exptime': 0.1,
                'nbframes': 3,
                'bin': Core.Bin(2, 2),
                'rot': Core.Rotation_180,
                'prefix': 'conf1_',
                'opolicy': Core.CtSaving.Overwrite
            },
            'conf2': {
                'exptime': 0.8,
                'nbframes': 2,
                'bin': Core.Bin(4, 4),
                'rot': Core.Rotation_90,
                'prefix': 'conf1_',
                'opolicy': Core.CtSaving.Abort
            }
        }

    @Core.DEB_MEMBER_FUNCT
    def __del__(self):
        del self.ct_control
        del self.cam_hw

    @Core.DEB_MEMBER_FUNCT
    def start(self):
        for conf in list(self.conf_dict.keys()):
            self.ct_acq.setAcqExpoTime(self.conf_dict[conf]['exptime'])
            self.ct_acq.setAcqNbFrames(self.conf_dict[conf]['nbframes'])
            self.ct_image.setRotation(self.conf_dict[conf]['rot'])
            # self.ct_image.setBin(self.conf_dict[conf]['bin'])
            self.ct_saving.setPrefix(self.conf_dict[conf]['prefix'])
            self.ct_saving.setOverwritePolicy(self.conf_dict[conf]['opolicy'])

            # store conf. in memory
            self.ct_config.store(conf, ['Acquisition', 'Image', 'Saving'])

        # save confs to file
        self.ct_config.setFilename('/tmp/test_config.cfg')
        self.ct_config.save()

        # remove all
        self.ct_config.remove(Core.CtConfig.All)

        # reload confs from file
        self.ct_config.load()

        # apply each conf and run an acquisition
        for conf in list(self.conf_dict.keys()):
            print("Run acq, with configuration {0}".format(conf))
            self.ct_config.apply(conf)
            # bin = self.ct_image.getBin()
            rot = self.ct_image.getRotation()
            nbframes = self.ct_acq.getAcqNbFrames()
            exptime = self.ct_acq.getAcqExpoTime()
            prefix = self.ct_saving.getPrefix()
            opolicy = self.ct_saving.getOverwritePolicy()

            # check if file is not corrupted
            # if bin.getX() != self.conf_dict[conf]['bin'].getX() or bin.getY() != self.conf_dict[conf]['bin'].getY():
            #     print ("Binning mode corrupted, was {0} is {1}, exit 1".format( self.conf_dict[conf]['bin'], bin))
            #     exit(1)
            if rot != self.conf_dict[conf]['rot']:
                print("rotation mode corrupted, was {0} is {1}, exit 1".format(self.conf_dict[conf]['rot'], rot))
                exit(1)
            if nbframes != self.conf_dict[conf]['nbframes']:
                print("Nb.frames corrupted, was {0} is {1}, exit 1".format(self.conf_dict[conf]['nbframes'], nbframes))
                exit(1)
            if exptime != self.conf_dict[conf]['exptime']:
                print("Exposure time corrupted, was {0} is {1}, exit 1".format(self.conf_dict[conf]['exptime'], exptime))
                exit(1)
            if prefix != self.conf_dict[conf]['prefix']:
                print("Saving prefix corrupted, was {0} is {1}, exit 1".format(self.conf_dict[conf]['prefix'], prefix))
                exit(1)
            if opolicy != self.conf_dict[conf]['opolicy']:
                print("Saving overwrite policy corrupted, was {0} is {1}, exit 1".format(self.conf_dict[conf]['opolicy'], opolicy))
                exit(1)

            print("  -> Rotation is {0}, nb. frames is {1} and exptime is {2}".format(rot, nbframes, exptime))

            self.ct_control.prepareAcq()
            self.ct_control.startAcq()

            lastimg = self.ct_control.getStatus().ImageCounters.LastImageReady
            while lastimg != nbframes-1:
                time.sleep(exptime)
                lastimg = self.ct_control.getStatus().ImageCounters.LastImageReady

        print("All is fine !!")
        exit(0)


@Core.DEB_FUNCT
def main(argv):
    tst_config = TestConfig()

    tst_config.start()


if __name__ == '__main__':
    main(sys.argv)
