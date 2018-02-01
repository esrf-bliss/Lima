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

from __future__ import print_function

import sys
import time
import argparse

from Lima import Core
from Lima import Simulator

Core.DEB_GLOBAL(Core.DebModTest)

class TestSaving:

    Core.DEB_CLASS(Core.DebModTest, 'TestSaving')

    @Core.DEB_MEMBER_FUNCT
    def __init__(self, camera = 'simulator'):
        if camera == 'maxipix':
            try:
                from Lima import Maxipix
            except ImportError:
                print ("Cannot use the Maxipix camera plugin, Maxipix python module is not installed")
                sys.exit()
            self.cam = Maxipix.Camera(1,'/users/blissadm/local/maxipix/tpxatl25', 'tpxatl25', True)
            self.cam_hw = Maxipix.Interface(self.cam)
        else:
            self.cam = Simulator.Camera()
            self.cam_hw = Simulator.Interface(self.cam)
            
        self.ct_control = Core.CtControl(self.cam_hw)        
        self.ct_saving =self.ct_control.saving()
        self.ct_acq =self.ct_control.acquisition()

        self.format_list= [ fmt.lower() for fmt in self.ct_saving.getFormatListAsString() ]

        self.overwrite2limaoverwrite={'abort': self.ct_saving.Abort,
                                      'append': self.ct_saving.Append,
                                      'multiset': self.ct_saving.MultiSet,
                                      'overwrite': self.ct_saving.Overwrite} 
    @Core.DEB_MEMBER_FUNCT               
    def __del__(self):
		del self.ct_control
                del self.cam_hw
                
    @Core.DEB_MEMBER_FUNCT
    def start(self, exp_time, nb_frames, directory, prefix, fmt, overwrite, framesperfile, threads, repeats, log_stat):
        if fmt.lower() not in self.format_list:
            raise ValueError("Unsupported file format. Should be one of %s"%str(self.format_list))
          
        # TIFF does not support multiple frames per file
        if  fmt == 'tiff' or fmt == 'cbf': fpf = 1
        else: fpf= framesperfile
        print('[%d] Prepare acquistion: %2.4f sec. %d frames, %s/%s, <%s>, %d FramesPerFile (overwrite-mode: %s)'%(repeats, exp_time, nb_frames,directory,prefix,fmt.upper(),fpf, overwrite))
        self.ct_acq.setAcqExpoTime(exp_time)
        self.ct_acq.setAcqNbFrames(nb_frames)
        self.ct_saving.setDirectory(directory)
        self.ct_saving.setPrefix(prefix)
        self.ct_saving.setFormatAsString(fmt)
        self.ct_saving.setFormatSuffix()
        self.ct_saving.setFramesPerFile(fpf)
        self.ct_saving.setOverwritePolicy(self.overwrite2limaoverwrite[overwrite])
        self.ct_saving.setSavingMode(self.ct_saving.AutoFrame)
        #self.ct_saving.setNextNumber(0)
        self.ct_saving.setStatisticHistorySize(nb_frames)

        self.ct_saving.setEnableLogStat(log_stat)

        # Setting Pool thread can improve the performance on multi-core computer, e.g for compression purpose
        Core.Processlib.PoolThreadMgr.get().setNumberOfThread(threads)
  

        self.repeats = repeats
        self.ct_control.prepareAcq()
        deb.Trace('[%d] PrepareAcq finished'%(repeats))
        self.ct_control.startAcq()

    @Core.DEB_MEMBER_FUNCT
    def waitAcq(self):
        def acq_status():
            return self.ct_control.getStatus().AcquisitionStatus
        while acq_status() == Core.AcqRunning:
            time.sleep(0.1)
            sys.stdout.write(str(self.ct_control.getStatus()) + '\r')
            sys.stdout.flush()
        print()
        deb.Trace('[%d] Acq. finished'%self.repeats)
        mb=1024*1024
        stat = self.ct_saving.getStatisticCounters()
        print('[%d] statistics (MB/s) : incoming speed = %.2f, saving speed = %.2f, compression speed = %.2f, compression ratio = %.2f'%(self.repeats,stat[3]/mb, stat[0]/mb, stat[1]/mb, stat[2]))




@Core.DEB_FUNCT
def main(argv):
	parser = argparse.ArgumentParser(description='A Lima test program for saving format')
        parser.add_argument('-v', '--verbose', help='verbose mode, up to vvv', required=False, action='count')
        parser.add_argument('-e', '--exposure', type=float, help='exposure time in sec.', required=False,default=0.1)
        parser.add_argument('-n', '--nbframes', type=int, help='number of frames.', required=False, default=1)
        if sys.platform == 'win32': format_list = ['all','cbf','edf','edfgz','hdf5','hdf5gz','hdf5bs','raw']
        else: format_list = ['all','cbf','edf','edfgz','edflz4','fits','hdf5','hdf5gz','hdf5bs','tiff','raw']
        format_list.sort()
        parser.add_argument('-f', '--format', help='saving format', choices=format_list, required=False, default='all', nargs='+')
        parser.add_argument('-d', '--directory', help='saving directory', required=False, default='./data')
        parser.add_argument('-p', '--prefix', help='file name prefix', required=False, default='lima_test_format_')
        parser.add_argument('-o', '--overwrite', help='overwrite mode', choices=['abort','append','multiset','overwrite'], required=False, default='abort')        
        parser.add_argument('-F', '--framesperfile', help='number of frames per file', type=int, required=False, default=1)        
        parser.add_argument('-R', '--repeats', help='number of frames per file', type=int, required=False, default=1)        
        parser.add_argument('-t', '--threads', help='number of Processlib pool threads', type=int, required=False, default=2)
        camera_list = ['simulator', 'maxipix']
        parser.add_argument('-c', '--camera', help='camera to test', choices=camera_list, required=False, default='simulator')        
        parser.add_argument('-l', '--log-stat', help='log statistics', required=False, default=False, action='store_true')        
        args = parser.parse_args()

        if args.verbose == 1:
            Core.DebParams.setTypeFlags(Core.DebTypeTrace)
            Core.DebParams.setModuleFlags(Core.DebModTest)
        elif args.verbose == 2:
            Core.DebParams.setTypeFlags(Core.DebTypeTrace)
            Core.DebParams.setModuleFlags(Core.DebParams.AllFlags)
        elif args.verbose >= 3:
            Core.DebParams.setTypeFlags(Core.DebParams.AllFlags)
            Core.DebParams.setModuleFlags(Core.DebParams.AllFlags)

	exp_time = 0.1

	test_saving = TestSaving(args.camera)

        if args.format == 'all':
            format_list=test_saving.format_list
        else:
            format_list = args.format
        format_list.sort()
        
        for fmt in format_list:
            for repeat in range(1,args.repeats+1):
                try:
                    test_saving.start(args.exposure,
                                      args.nbframes,
                                      args.directory,
                                      args.prefix,
                                      fmt,
                                      args.overwrite,
                                      args.framesperfile,
                                      args.threads,
                                      repeat,
                                      args.log_stat)
                except Core.Exception, e:
                    raise RuntimeError
                    
                test_saving.waitAcq()

            
if __name__ == '__main__':
	main(sys.argv)


