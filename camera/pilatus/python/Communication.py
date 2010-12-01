from __future__ import with_statement

import socket
import threading
import select
import os

from Lima import Core

class _AsyncSocket(threading.Thread) :
    Core.DEB_CLASS(Core.DebModCameraCom, '_AsyncSocket')
    
    def __init__(self,cnt,cond) :
        threading.Thread.__init__(self)
        
        self.__cond = cond
        self.__cnt = cnt
        self.__socket = None
        self.__sync,self.__wakeUp = os.pipe()
        self.__stop = False
        
    def connect(self,host,port) :
        with self.__cond:
            if self.__socket is not None:
                self.__socket.close()
            try:
                self.__socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.__socket.setsockopt(socket.IPPROTO_TCP,
                                         socket.TCP_NODELAY,1)
                self.__socket.connect((host,port))
            
            except (socket.gaierror,socket.error):
                self.__socket = None
                raise
            else:
                os.write(self.__wakeUp,"|")
                self.__cnt._state = Communication.OK
        self.__init()

    def __init(self) :
        self.send('SetThreshold')
        self.send('exptime')
        self.send('expperiod')
        self.send('imgpath %s' % Communication.DEFAULT_PATH)
        self.send('delay')
        self.send('nexpframe')
        self.send('setackint 0')
        self.send('dbglvl 1')
        
    def __reinit(self) :
	self.__init()
        self.send('nimages')
        
    def quit(self) :
        with self.__cond:
            self.__stop = True
            if self.__socket:
                self.__socket.close()
                self.__socket = None
            os.write(self.__wakeUp,"|")
        self.join()

    @Core.DEB_MEMBER_FUNCT
    def send(self,msg) :
        deb.Param('message=%s' % msg)

        msg += '\x18'
        self.__socket.send(msg)

    @Core.DEB_MEMBER_FUNCT
    def run(self) :
        with self.__cond:
            while not self.__stop :
                rlist = self.__socket and [self.__socket,self.__sync] or [self.__sync]
                self.__cond.release()
                evtRlist,_,_ = select.select(rlist, [], [])
                self.__cond.acquire()
                
                if self.__sync in evtRlist:
                    os.read(self.__sync,1024)
                if self.__socket in evtRlist:
                    messages = self.__socket.recv(16384)
                    if not messages:         # connection closed
                        self.__socket = None
                        self.__cnt._state = Communication.DISCONNECTED
                    else:
                        for msg in messages.split('\x18') :
                            deb.Trace("message rx : %s" % msg)
                            if msg[:2] == '15': # generique response
                                if msg[3:5] == 'OK': # will check what the message is about
                                    real_message = msg[6:]
                                    if real_message.startswith('Settings:') : # Threshold and gain is already set,read them
                                        gain_string,threshold_string,_ = msg[15:].split(';')
                                        
                                        threshold_tocken,threshold_value,threshold_unit = threshold_string.split()
                                        self.__cnt._threshold = int(threshold_value)

                                        gain_value = ' '.join(gain_string.split()[:-1])
                                        self.__cnt._gain = Communication.GAIN_SERVER_RESPONSE[gain_value]
                                    elif real_message.startswith('/tmp/setthreshold') :
                                        self.__cnt._state = Communication.OK
                                        self.__reinit() # resync with server
                                    elif real_message.startswith('Exposure') :
                                        columnPos = real_message.find(':')
                                        lastSpace = real_message.rfind(' ')
                                        if real_message[9:].startswith('time') :
                                            self.__cnt._exposure = float(real_message[columnPos + 1:lastSpace])
                                        elif real_message[9:].startswith('period'):
                                            self.__cnt._exposure_period = float(real_message[columnPos + 1:lastSpace])
                                        else: # Exposures per frame
                                            self.__cnt._exposure_per_frame = int(real_message[columnPos + 1:])
                                            
                                        self.__cnt._state = Communication.OK
                                    elif real_message.startswith('Delay') :
                                        columnPos = real_message.find(':')
                                        lastSpace = real_message.rfind(' ')
                                        self.__cnt._hardware_trigger_delay = float(real_message[columnPos + 1:lastSpace])
                                        self.__cnt._state = Communication.OK
                                    elif real_message.startswith('N images') :
                                        columnPos = real_message.find(':')
                                        self.__cnt._nimages = int(real_message[columnPos+1:])
                                        self.__cnt._state = Communication.OK
                                    elif self.__cnt._state == Communication.SETTING_THRESHOLD:
				        self.__cnt._state = Communication.OK
                                else:   # ERROR MESSAGE
                                    if self.__cnt._state == Communication.SETTING_THRESHOLD:
                                        self.__cnt._state = Communication.OK
                                        print 'Threshold setting failed'
                                    elif self.__cnt._state == Communication.SETTING_EXPOSURE:
                                        self.__cnt._state = Communication.OK
					print 'Exposure setting failed'
                                    else:
                                        print msg[2:]
                                self.__cond.notifyAll()
			    elif msg[:2] == '13': #Acquisition Killed
				self.__cnt._state = Communication.OK
                            elif msg:
                                if msg[0] == '7':
                                    if msg[2:4] == 'OK':
                                        self.__cnt._state = Communication.OK
                                    else:
                                        self.__cnt._state = Communication.ERROR
                                        msg = msg[2:]
                                        self.__cnt._error_message = msg[msg.find(' '):]
                                        
                                elif msg[0] == '1':
                                    if msg[2:5] == 'ERR':
                                        if self.__cnt._state == Communication.KILL_ACQUISITION:
                                            self.__cnt._state = Communication.OK
                                        else:
                                            self.__cnt._error_message = msg[6:]
                                            self.__cnt._state = Communication.ERROR

                                        
                
            
        
class Communication:
    Core.DEB_CLASS(Core.DebModCameraCom, 'Communication')
    
    ERROR,DISCONNECTED,OK,SETTING_THRESHOLD,SETTING_EXPOSURE,SETTING_NB_IMAGE_IN_SEQUENCE,SETTING_EXPOSURE_PERIOD,SETTING_HARDWARE_TRIGGER_DELAY,SETTING_EXPOSURE_PER_FRAME,KILL_ACQUISITION,RUNNING = range(11)
    LOW,MID,HIGH,ULTRA_HIGH = range(4)  # GAIN enum
    GAIN_SERVER_RESPONSE = {'low' : LOW,'mid' : MID,'high' : HIGH,'ultra high' : ULTRA_HIGH}
    GAIN_VALUE2SERVER = {LOW : 'lowG',MID : 'midG',HIGH : 'highG',ULTRA_HIGH : 'uhighG'}
    INTERNAL,INTERNAL_TRIG_MULTI,EXTERNAL_START,EXTERNAL_MULTI_START,EXTERNAL_GATE = range(5)

    DEFAULT_PATH = '/lima_data'
    DEFAULT_TMPFS_SIZE = 8 * 1024 * 1024 * 1024 # 8Go
    DEFAULT_FILE_BASE = 'tmp_img_'
    DEFAULT_FILE_EXTENTION = '.edf'
    DEFAULT_FILE_PATERN = 'tmp_img_%.5d.edf'
    
    def __init__(self, host = None, port = None):
        self.__cond = threading.Condition()
        self._state = self.DISCONNECTED
        self.__asynSock = _AsyncSocket(self,self.__cond)
        self.__asynSock.start()

        self.__timeout = 10.
        self._error_message = None
	
        self._trigger_mode = Communication.INTERNAL
        self._gap_fill = False
        
        try:
            self.connect(host,port)
        except socket.error:
            pass

    def __del__(self) :
        self.quit()

    ##@brief init all variable of the Pilatus server (camserv)
    def _init_variable(self) :
        self._threshold = None
        self._gain = None
        self._exposure = None
        self._nimages = 1
        self._exposure_period = None
        self._hardware_trigger_delay = None
        self._exposure_per_frame = None

    def status(self) :
        with self.__cond:
            return self._state
        
    def error_message(self) :
        with self.__cond:
            return self._error_message

    def soft_reset(self) :
        with self.__cond:
            self._error_message = None
            self._state = self.OK
    def hard_reset(self) :
        with self.__cond:
            self.__asynSock.send('resetcam')

    def connect(self,host,port) :
	self._init_variable()

        if host is not None and port is not None:
	    self._init_variable()
            self.__asynSock.connect(host,port)
    
    def quit(self) :
        self.__asynSock.quit()
        
    def threshold(self) :
        with self.__cond:
            return self._threshold

    def gain(self) :
        with self.__cond:
            return self._gain
    
    def set_threshold_gain(self,value,gain = None) :
        with self.__cond :
            if self._state != self.OK :
                self.__cond.wait(self.__timeout)

            if self._state == self.OK:
                if gain is None:
                    self.__asynSock.send('SetThreshold %d' % value)
                else:
                    gainStr = Communication.GAIN_VALUE2SERVER[gain]
                    self.__asynSock.send('SetThreshold %s %d' % (gainStr,value))
                self._state = self.SETTING_THRESHOLD
            else:
                raise 'Could not set threshold, server is not idle'

            if self._gap_fill:
                self.__asynSock.send('gapfill -1')
            
    def exposure(self) :
        with self.__cond:
            return self._exposure

    def set_exposure(self,val) :
        with self.__cond:
	    # yet an other border-effect with the SPEC CCD interface
	    # to reach the GATE mode SPEC programs extgate + expotime = 0
            if self._trigger_mode == self.EXTERNAL_GATE:
	    	return
	    if self._state != self.OK :
                self.__cond.wait(self.__timeout)

            if self._state == self.OK:
                self._state = self.SETTING_EXPOSURE
                self.__asynSock.send('exptime %f' % val)
            else:
                raise 'Could not set exposure, server is not idle'

    def exposure_period(self) :
        with self.__cond:
            return self._exposure_period

    def set_exposure_period(self,val) :
        with self.__cond:
            if self._state != self.OK :
                self.__cond.wait(self.__timeout)

            if self._state == self.OK:
                self._state = self.SETTING_EXPOSURE_PERIOD
                self.__asynSock.send('expperiod %f' % val)
            else:
                raise 'Could not set exposure period, server not idle'
        
    def nb_images_in_sequence(self) :
        with self.__cond:
            return self._nimages

    def set_nb_images_in_sequence(self,nb) :
        with self.__cond:
            if self._state != self.OK :
                self.__cond.wait(self.__timeout)

            if self._state == self.OK:
                self._state = self.SETTING_NB_IMAGE_IN_SEQUENCE
                self.__asynSock.send('nimages %d' % nb)
            else:
                raise 'Could not set number image in sequence, server not idle'


    def hardware_trigger_delay(self) :
        with self.__cond:
            return self._hardware_trigger_delay

    def set_hardware_trigger_delay(self,value) :
        with self.__cond:
            if self._state != self.OK:
                self.__cond.wait(self.__timeout)

            if self._state == self.OK:
                self._state = self.SETTING_HARDWARE_TRIGGER_DELAY
                self.__asynSock.send('delay %f' % value)
            else:
                raise 'Could not set hardware trigger delay, server not idle'

    def nb_exposure_per_frame(self) :
        with self.__cond:
            return self._exposure_per_frame

    def set_nb_exposure_per_frame(self,val) :
        with self.__cond:
            if self._state != self.OK:
                self.__cond.wait(self.__timeout)

            if self._state == self.OK:
                self._state = self.SETTING_EXPOSURE_PER_FRAME
                self.__asynSock.send('nexpframe %d' % val)
            else:
                raise 'Could not set exposure per frame, server not idle'

    def trigger_mode(self) :
        with self.__cond:
            return self._trigger_mode
    
    ##@brief set the trigger mode
    #
    #Trigger can be:
    # - Internal (Software) == INTERNAL
    # - External start == EXTERNAL_START
    # - External multi start == EXTERNAL_MULTI_START
    # - External gate == EXTERNAL_GATE
    def set_trigger_mode(self,trigger_mode) :
        with self.__cond:
            if self.INTERNAL <= trigger_mode <= self.EXTERNAL_GATE:
                self._trigger_mode = trigger_mode
            else:
                raise 'Trigger can be only:Internal,Internal Trig Multi,External start,External multi start or External gate!'
    @Core.DEB_MEMBER_FUNCT
    def start_acquisition(self,image_number = 0) :
        with self.__cond:
            deb.Trace("State : %s, trigger mode : %s" % (self._state,self._trigger_mode))
	    if self._state == self.RUNNING:
		raise 'Could not start acquisition, you have to wait the end of the previous one'

            if self._trigger_mode != self.EXTERNAL_GATE:
                while self._exposure_period <= (self._exposure + 0.002999) :
                    self.__asynSock.send('expperiod %f' % (self._exposure + 0.003))
                    self.__cond.wait(self.__timeout)

            filename = self.DEFAULT_FILE_PATERN % image_number
            #Start Acquisition
            if self._trigger_mode == self.EXTERNAL_START:
                self.__asynSock.send('exttrigger %s' % filename)
            elif self._trigger_mode == self.EXTERNAL_MULTI_START:
                self.__asynSock.send('extmtrigger %s' % filename)
            elif self._trigger_mode == self.EXTERNAL_GATE:
                self.__asynSock.send('extenable %s' % filename)
            else:
                self.__asynSock.send('exposure %s' % filename)
                
            if(self._trigger_mode != self.INTERNAL ||
               self._trigger_mode != self.INTERNAL_TRIG_MULTI):
                self.__cond.wait(self.__timeout)
             
            self._state = self.RUNNING

    def stop_acquisition(self) :
        with self.__cond:
            if self._state == self.RUNNING:
                self._state = self.KILL_ACQUISITION
                self.__asynSock.send('k')
        
    
    def set_gapfill(self,val) :
        with self.__cond:
            self._gap_fill = val
            self.__asynSock.send('gapfill %d' % (self._gap_fill and -1 or 0))
        

    def gapfill(self) :
        with self.__cond:
            return self._gap_fill and True or False
