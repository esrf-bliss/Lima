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
import sys, os, thread, time, random
import lima

class FrelonData:

    def __init__(self, edev_nr=0):
        self.edev = lima.Espia.Dev(edev_nr)
        self.eser_line = lima.Espia.SerialLine(self.edev)
        self.frelon = lima.Frelon.Camera(self.eser_line)
        self.fser_line = self.frelon.getSerialLine()

        self.finish_cond = lima.Cond()
        self.finish_req = False

        self.log_cond = lima.Cond()
        
    def getFrelonCamera(self):
        return self.frelon
    
    def getFrelonSerialLine(self):
        return self.fser_line

    def getFinishReq(self):
        self.finish_cond.acquire()
        finish_req = self.finish_req
        self.finish_cond.release()
        return finish_req

    def setFinishReq(self, finish_req):
        self.finish_cond.acquire()
        self.finish_req = finish_req
        self.finish_cond.release()

    def logMsg(self, msg):
        self.log_cond.acquire()
        print msg
        self.log_cond.release()

    def logMsgTime(self, msg, dt):
        self.logMsg('%s [%10.6f sec]' %(msg, dt))
        

def change_cache_mode(frelon_data):

    fser_line = frelon_data.getFrelonSerialLine()

    while not frelon_data.getFinishReq():
        cache_act = not fser_line.getCacheActive()
        t0 = time.time()
        fser_line.setCacheActive(cache_act)
        dt = time.time() - t0
        frelon_data.logMsgTime('Setting cache active: %d' % cache_act, dt)
        time.sleep(60)


def read_multi_line(frelon_data):

    fser_line = frelon_data.getFrelonSerialLine()

    multi_line_cmds = ['H', 'V', 'D', 'AOI', 'C', 'PLL']
    
    while not frelon_data.getFinishReq():
        for cmd in multi_line_cmds:
            t0 = time.time()
            fser_line.write(cmd)
            dt = time.time() - t0
            frelon_data.logMsgTime('Sending multi-line cmd: "%s"' % cmd, dt)
            t0 = time.time()
            ans = fser_line.readLine()
            dt = time.time() - t0
            frelon_data.logMsgTime('Received "%s" resp:\n"%s"' % (cmd, ans),
                                   dt)


def change_exp_time(frelon_data):

    frelon = frelon_data.getFrelonCamera()

    while not frelon_data.getFinishReq():
        exp_time = random.random() * 65.535
        t0 = time.time()
        frelon.setExpTime(exp_time)
        dt = time.time() - t0
        frelon_data.logMsgTime('Setting exp_time to: %s' % exp_time, dt)
        t0 = time.time()
        new_exp_time = frelon.getExpTime()
        dt = time.time() - t0
        frelon_data.logMsgTime('New exp_time: %s' % new_exp_time, dt)

  
def main(argv):

    frelon_data = FrelonData()

    thread_fn = {
        'cache_mode':	change_cache_mode,
        'multi_line':	read_multi_line,
        'exp_time':	change_exp_time,
    }
    
    thread_id = {}
    for name, fn in thread_fn.items():
        thread_id[name] = thread.start_new_thread(fn, (frelon_data,))

    raw_input()

    frelon_data.setFinishReq(True)
    
    
if __name__ == '__main__':
    main(sys.argv)
