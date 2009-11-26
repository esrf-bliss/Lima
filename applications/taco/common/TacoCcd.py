import os, sys, string, time, re
import numpy as N
from TacoServer import *
from lima import *

DevCcdBase			= 0xc180000

# CCD Commands
DevCcdStart			= DevCcdBase + 1
DevCcdStop			= DevCcdBase + 2
DevCcdRead			= DevCcdBase + 3
DevCcdSetExposure		= DevCcdBase + 4
DevCcdGetExposure		= DevCcdBase + 5
DevCcdSetRoI			= DevCcdBase + 6
DevCcdGetRoI			= DevCcdBase + 7
DevCcdSetBin			= DevCcdBase + 8
DevCcdGetBin			= DevCcdBase + 9
DevCcdSetTrigger		= DevCcdBase + 10
DevCcdGetTrigger		= DevCcdBase + 11
DevCcdGetLstErrMsg		= DevCcdBase + 12
DevCcdXSize			= DevCcdBase + 13
DevCcdYSize			= DevCcdBase + 14
DevCcdSetADC			= DevCcdBase + 15
DevCcdGetADC			= DevCcdBase + 16
DevCcdSetSpeed			= DevCcdBase + 17
DevCcdGetSpeed			= DevCcdBase + 18
DevCcdSetShutter		= DevCcdBase + 19
DevCcdGetShutter		= DevCcdBase + 20
DevCcdSetFrames			= DevCcdBase + 21
DevCcdGetFrames			= DevCcdBase + 22
DevCcdCommand			= DevCcdBase + 23
DevCcdDepth			= DevCcdBase + 24
DevCcdSetMode			= DevCcdBase + 25
DevCcdGetMode			= DevCcdBase + 26
DevCcdSetChannel		= DevCcdBase + 27
DevCcdGetChannel		= DevCcdBase + 28
DevCcdSetRingBuf		= DevCcdBase + 29
DevCcdGetRingBuf		= DevCcdBase + 30
DevCcdLive			= DevCcdBase + 31
DevCcdWriteFile			= DevCcdBase + 32
DevCcdReset			= DevCcdBase + 33
DevCcdGetIdent			= DevCcdBase + 34
DevCcdGetType			= DevCcdBase + 35
DevCcdSetKinWinSize		= DevCcdBase + 36
DevCcdGetKinWinSize		= DevCcdBase + 37
DevCcdSetKinetics		= DevCcdBase + 38
DevCcdGetKinetics		= DevCcdBase + 39
DevCcdCorrect			= DevCcdBase + 40
DevCcdSetFilePar		= DevCcdBase + 41
DevCcdGetFilePar		= DevCcdBase + 42
DevCcdHeader			= DevCcdBase + 43
DevCcdSetFormat			= DevCcdBase + 44
DevCcdGetFormat			= DevCcdBase + 45
DevCcdSetViewFactor		= DevCcdBase + 46
DevCcdGetViewFactor		= DevCcdBase + 47
DevCcdSetHwPar			= DevCcdBase + 48
DevCcdGetHwPar			= DevCcdBase + 49
DevCcdGetCurrent		= DevCcdBase + 50
DevCcdGetBuffer			= DevCcdBase + 51
DevCcdGetBufferInfo		= DevCcdBase + 52
DevCcdReadAll			= DevCcdBase + 53
DevCcdWriteAll			= DevCcdBase + 54
DevCcdDezinger			= DevCcdBase + 55
DevCcdSetThreshold		= DevCcdBase + 56
DevCcdGetThreshold		= DevCcdBase + 57
DevCcdSetMaxExposure		= DevCcdBase + 58
DevCcdGetMaxExposure		= DevCcdBase + 59
DevCcdSetGain			= DevCcdBase + 60
DevCcdGetGain			= DevCcdBase + 61
DevCcdReadJpeg			= DevCcdBase + 62
DevCcdRefreshTime		= DevCcdBase + 63
DevCcdOutputSize		= DevCcdBase + 64
DevCcdGetTGradient		= DevCcdBase + 65
DevCcdGetChanges		= DevCcdBase + 66
DevCcdCalibrate			= DevCcdBase + 67
DevCcdSetThumbnail1		= DevCcdBase + 68
DevCcdSetThumbnail1		= DevCcdBase + 69
DevCcdWriteThumbnail1		= DevCcdBase + 70
DevCcdWriteThumbnail1		= DevCcdBase + 71

# CCD States
DevCcdReady			= DevCcdBase + 1
DevCcdAcquiring			= DevCcdBase + 2
DevCcdFault			= DevCcdBase + 3
DevCcdSaving			= DevCcdBase + 4
DevCcdNotYetInitialised		= DevCcdBase + 5
DevCcdInitializing		= DevCcdBase + 6
DevCcdReadout			= DevCcdBase + 7
DevCcdCorrecting		= DevCcdBase + 8
DevCcdBusy			= DevCcdBase + 9
DevCcdAborting			= DevCcdBase + 10
DevCcdNoRemote			= DevCcdBase + 11

# CCD Errors
DevErrCcdState			= DevCcdBase + 1
DevErrCcdController		= DevCcdBase + 2
DevErrCcdNotEnoughDisk		= DevCcdBase + 3
DevErrCcdNoDirPermission	= DevCcdBase + 4
DevErrCcdNoDirectory		= DevCcdBase + 5
DevErrCcdLongPath		= DevCcdBase + 6
DevErrCcdEmptyPath		= DevCcdBase + 7
DevErrCcdNotAccessible		= DevCcdBase + 8
DevErrCcdNoFilePermission	= DevCcdBase + 9
DevErrCcdFileExist		= DevCcdBase + 10
DevErrCcdCmdNotProc		= DevCcdBase + 11
DevErrCcdCameraModel		= DevCcdBase + 12
DevErrCcdProcessImage		= DevCcdBase + 13
DevErrCcdCameraNotActiveYet	= DevCcdBase + 14

class TacoCcdError:
    pass

        
class TacoCcdAcq(TacoServer):

    cmd_list = {
        DevState:		[D_VOID_TYPE, D_LONG_TYPE,
                                 'getState', 'DevState'],
        DevStatus:		[D_VOID_TYPE, D_STRING_TYPE,
                                 'getStatus', 'DevStatus'],
        DevCcdReset:		[D_VOID_TYPE, D_VOID_TYPE,
                                 'reset', 'DevCcdReset'],
        DevCcdXSize:		[D_VOID_TYPE, D_LONG_TYPE,
                                 'getXSize', 'DevCcdXSize'],
        DevCcdYSize:		[D_VOID_TYPE, D_LONG_TYPE,
                                 'getYSize', 'DevCcdYSize'],
        DevCcdDepth:		[D_VOID_TYPE, D_LONG_TYPE,
                                 'getDepth', 'DevCcdDepth'],
        DevCcdGetType:		[D_VOID_TYPE, D_LONG_TYPE,
                                 'getType',  'DevCcdGetType'],
        DevCcdGetLstErrMsg:	[D_VOID_TYPE, D_STRING_TYPE,
                                 'getLstErrMsg', 'DevCcdGetLstErrMsg'],
        DevCcdSetFrames:	[D_LONG_TYPE, D_VOID_TYPE,
                                 'setNbFrames', 'DevCcdSetFrames'],
        DevCcdGetFrames:	[D_VOID_TYPE, D_LONG_TYPE,
                                 'getNbFrames', 'DevCcdGetFrames'],
        DevCcdSetTrigger:	[D_LONG_TYPE, D_VOID_TYPE,
                                 'setTrigger', 'DevCcdSetTrigger'],
        DevCcdGetTrigger:	[D_VOID_TYPE, D_LONG_TYPE,
                                 'getTrigger', 'DevCcdGetTrigger'],
        DevCcdSetExposure:	[D_FLOAT_TYPE, D_VOID_TYPE,
                                 'setExpTime', 'DevCcdSetExposure'],
        DevCcdGetExposure:	[D_VOID_TYPE, D_FLOAT_TYPE,
                                 'getExpTime', 'DevCcdGetExposure'],
        DevCcdSetBin:		[D_VAR_LONGARR, D_VOID_TYPE,
                                 'setBin', 'DevCcdSetBin'],
        DevCcdGetBin:		[D_VOID_TYPE, D_VAR_LONGARR, 
                                 'getBin', 'DevCcdGetBin'],
        DevCcdSetRoI:		[D_VAR_LONGARR, D_VOID_TYPE,
                                 'setRoi', 'DevCcdSetRoI'],
        DevCcdGetRoI:		[D_VOID_TYPE, D_VAR_LONGARR, 
                                 'getRoi', 'DevCcdGetRoI'],
        DevCcdSetFilePar:	[D_VAR_STRINGARR, D_VOID_TYPE,
                                 'setFilePar', 'DevCcdSetFilePar'],
        DevCcdGetFilePar:	[D_VOID_TYPE, D_VAR_STRINGARR, 
                                 'getFilePar', 'DevCcdGetFilePar'],
        DevCcdSetChannel:	[D_LONG_TYPE, D_VOID_TYPE,
                                 'setChannel', 'DevCcdSetChannel'],
        DevCcdGetChannel:	[D_VOID_TYPE, D_LONG_TYPE, 
                                 'getChannel', 'DevCcdGetChannel'],
        DevCcdSetMode:		[D_LONG_TYPE, D_VOID_TYPE,
                                 'setMode', 'DevCcdSetMode'],
        DevCcdGetMode:		[D_VOID_TYPE, D_LONG_TYPE, 
                                 'getMode', 'DevCcdGetMode'],
        DevCcdSetHwPar:		[D_STRING_TYPE, D_VOID_TYPE,
                                 'setHwPar', 'DevCcdSetHwPar'],
        DevCcdGetHwPar:		[D_VOID_TYPE, D_STRING_TYPE, 
                                 'getHwPar', 'DevCcdGetHwPar'],
        DevCcdSetKinetics:	[D_LONG_TYPE, D_VOID_TYPE,
                                 'setKinetics', 'DevCcdSetKinetics'],
        DevCcdGetKinetics:	[D_VOID_TYPE, D_LONG_TYPE,
                                 'getKinetics', 'DevCcdGetKinetics'],
        DevCcdStart:		[D_VOID_TYPE, D_VOID_TYPE,
                                 'startAcq', 'DevCcdStart'],
        DevCcdStop:		[D_VOID_TYPE, D_VOID_TYPE,
                                 'stopAcq', 'DevCcdStop'],
        DevCcdRead:		[D_VAR_LONGARR, D_OPAQUE_TYPE,
                                 'readFrame', 'DevCcdRead'],
        DevCcdLive:		[D_VOID_TYPE, D_VOID_TYPE,
                                 'startLive', 'DevCcdLive'],
        DevCcdGetCurrent:	[D_VOID_TYPE, D_LONG_TYPE, 
                                 'getCurrent', 'DevCcdGetCurrent'],
        DevCcdCommand:		[D_STRING_TYPE, D_STRING_TYPE, 
                                 'execCommand', 'DevCcdCommand'],
        DevCcdGetChanges:	[D_VOID_TYPE, D_LONG_TYPE, 
                                 'getChanges', 'DevCcdGetChanges'],
    }

    AutoSave = 8

    deb_params = DebParams(DebModApplication, "TacoCcdAcq")
    
    def __init__(self, dev_name, dev_class=None, cmd_list=None):
        deb_obj = DebObj(TacoCcdAcq.deb_params, "__init__")
        
	if dev_class is None:
            dev_class = 'TacoCcdDevClass'
        if cmd_list is None:
            cmd_list = self.cmd_list
        TacoServer.__init__(self, dev_name, dev_class, cmd_list)
        self.dev_name = dev_name

    def reset(self):
        #self.debugReset('Reseting the device!')
        pass
        
    def getResources(self, default_resources):
        #self.debugTrace('Getting device resources ...')
        pars = {}
        for res_name, def_val in default_resources.items():
            val = esrf_getresource(self.dev_name, res_name)
            if not val:
                val = def_val
            pars[res_name] = val
        return pars
    
    def getState(self):
        #self.debugState('Query device state ...')
        self.state = DevCcdReady
        #self.debugState('Device state: 0x%08x (%d)' % (state, state))
        return self.state

    def getStatus(self):
        state_desc = { DevCcdReady:     'CCD is Ready',
                       DevCcdAcquiring: 'CCD is Acquiring' }
        state = self.getState()
        status = state_desc[state]
        #self.debugState('Device status: %s (0x%08x)' % (status, state))
        return status

    def getFrameDim(self, max_dim=False):
        fdim = FrameDim(Size(1024, 1024), Bpp16)
        #self.debugCmd('Frame dim: %s' % fdim)
        return fdim
    
    def getXSize(self):
        deb_obj = DebObj(TacoCcdAcq.deb_params, "getXSize")
        frame_dim = self.getFrameDim(max_dim=True)
        width = frame_dim.getSize().getWidth()
        deb_obj.Return("width=%s" % width)
        return width
    
    def getYSize(self):
        frame_dim = self.getFrameDim(max_dim=True)
        return frame_dim.getSize().getHeight()
    
    def getDepth(self):
        frame_dim = self.getFrameDim(max_dim=True)
        return frame_dim.getDepth()

    def getType(self):
        type_nb = 0
        #self.debugCmd('Getting type: %s (#%s)' % (ccd_type, type_nb))
        return type_nb

    def getLstErrMsg(self):
        err_msg = ''
        #self.debugCmd('Getting last err. msg: %s' % err_msg)
        return err_msg
    
    def setTrigger(self, ext_trig):
        #self.debugCmd('Setting trigger: %s' % ext_trig)
        pass
    
    def getTrigger(self):
        ext_trig = 0
        #self.debugCmd('Getting trigger: %s' % ext_trig)
        return ext_trig
    
    def setNbFrames(self, nb_frames):
        #self.debugCmd('Setting nb. frames: %s' % nb_frames)
        pass
    
    def getNbFrames(self):
        nb_frames = 1
        #self.debugCmd('Getting nb. frames: %s' % nb_frames)
        return nb_frames
    
    def setExpTime(self, exp_time):
        #self.debugCmd('Setting exp. time: %s' % exp_time)
        pass
    
    def getExpTime(self):
        exp_time = 1
        #self.debugCmd('Getting exp. time: %s' % exp_time)
        return exp_time

    def setBin(self, bin):
        # SPEC format Y,X -> incompat. with getBin ...
        bin = Bin(bin[1], bin[0])
        #self.debugCmd('Setting binning: %s' % bin)

    def getBin(self):
        bin = Bin(1, 1)
        #self.debugCmd('Getting binning: %s' % bin)
        return [bin.getX(), bin.getY()]

    def setRoi(self, roi):
        roi = Roi(Point(roi[0], roi[1]), Point(roi[2], roi[3]))
        #self.debugCmd('Setting roi: %s' % roi)

    def getRoi(self):
        roi = Roi()
        #self.debugCmd('Getting roi: %s' % roi)
        tl = roi.getTopLeft()
        br = roi.getBottomRight()
        return [tl.getX(), tl.getY(), br.getX(), br.getY()]
            
    def setFilePar(self, file_par_arr):
        file_par = CcdFilePar(from_arr=file_par_arr)
        config = self.getConfig()
        config.setParam('FilePar', file_par)
        config.apply()

    def getFilePar(self):
        config = self.getConfig()
        file_par = config.getParam('FilePar')
        return file_par.strArray()

    def setChannel(self, input_chan):
        #self.debugCmd('Setting input channel: %s' % input_chan)
        pass
    
    def getChannel(self):
        input_chan = 0
        #self.debugCmd('Getting input channel: %s' % input_chan)
        return input_chan
        
    def setMode(self, mode):
        #self.debugCmd('Setting mode: %s (0x%x)' % (mode, mode))
        auto_save = (mode & self.AutoSave) != 0
        
    def getMode(self):
        auto_save = False
        mode = (auto_save and self.AutoSave) or 0
        #self.debugCmd('Getting mode: %s (0x%x)' % (mode, mode))
        return mode

    def setHwPar(self, hw_par_str):
        hw_par = map(int, string.split(hw_par_str))
        #self.debugCmd('Setting hw par: %s' % hw_par)
        
    def getHwPar(self):
        hw_par = []
        #self.debugCmd('Getting hw par: %s' % hw_par)
        hw_par_str = string.join(map(str, hw_par))
        return hw_par_str
        
    def setKinetics(self, kinetics):
        #self.debugCmd('Setting the profile: %s' % kinetics)
        pass
    
    def getKinetics(self):
        kinetics = 0
        #self.debugCmd('Getting the profile: %s' % kinetics)
        return kinetics
    
    def startAcq(self):
        #self.debugCmd('Starting the device')
        pass
    
    def stopAcq(self):
        #self.debugCmd('Stopping the device')
        pass
    
    def readFrame(self, frame_data):
        frame_nb, frame_size = frame_data
        frame_dim = self.getFrameDim()
        if frame_size != frame_dim.getMemSize():
            raise ValueError, ('Client expects %d bytes, frame has %d' % \
                               (frame_size, frame_dim.getMemSize()))
        shape = (frame_dim.getHeight(), frame_dim.getWidth())
        data = N.zeros(shape, N.uint16)
        s = data.tostring()
        if len(s) != frame_size:
            raise ValueError, ('Client expects %d bytes, data str has %d' % \
                               (frame_size, len(s)))
        return s

    def startLive(self):
        pass
    
    def getCurrent(self):
        last_frame_nb = 0
        return last_frame_nb

    def execCommand(self, cmd):
        #self.debugCmd('Sending cmd: %s' % cmd)
        resp = ''
        #self.debugCmd('Received response:')
        #for line in resp.split('\r\n'):
            #self.debugCmd(line)
        return resp

    def getChanges(self):
        changes = 0
        #self.debugCmd('Getting changes: %s' % changes)
        return changes

    
class CcdServer:

    def __init__(self, bin_name, pers_name):
        self.bin_name = bin_name
        self.pers_name = pers_name
        self.server_name = '%s/%s' % (bin_name, pers_name)
        #self.debugAlways('Getting devices for %s' % server_name)
        self.devices = []
        
    def getDevNameList(self, server_name=None):
        if server_name is None:
            server_name = self.server_name
            
        try:
            dev_name_list = dev_getdevlist(server_name)
        except:
            sys.exit(1)

        #self.debugAlways('Devices found in database for %s' % server_name)
        #for dev_name in dev_name_list:
            #self.debugAlways('         ' + dev_name)

        return dev_name_list

    def addDev(self, dev):
        self.devices.append(dev)

    def startup(self, sleep_forever=1):
        server_startup(self.devices, self.pers_name, self.bin_name)

        if sleep_forever:
            #self.debugAlways('That\'s all! Going to sleep ...')
            while 1:
                time.sleep(.01)

        
def main(argv):
    bin_name = os.path.basename(argv[0])
    try:
        pers_name = argv[1]
    except:
        print 'Usage: %s <pers_name>' % bin_name
        sys.exit(1)
		
    server = CcdServer(bin_name, pers_name)
    

if __name__ == '__main__':
    main(sys.argv)
    
        


    
