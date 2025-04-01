from __future__ import annotations

import weakref
import enum
from Lima import Core


class MockedState(enum.Enum):
    ERROR = enum.auto()
    READY = enum.auto()
    RUNNING = enum.auto()


class MockedCamera:
    Core.DEB_CLASS(Core.DebModCamera, "mocked.Camera")

    def __init__(
        self,
        buffer_ctrl=None,
        trigger_multi: bool = True,
        supports_sum_binning: bool = False,
    ):
        self.control = None
        self.name = "mocked"
        self.width = 16
        self.height = 8
        self.bpp: Core.ImageType = Core.Bpp8
        # 55um x 55 um
        self.pixel_size = 55e-6, 55e-6

        # For hardware capability
        self.binning: Core.Bin = Core.Bin(1, 1)

        self.trigger_mode: Core.TrigMod
        if trigger_multi:
            self.trigger_mode = Core.IntTrigMult
        else:
            self.trigger_mode = Core.IntTrig

        self.supports_sum_binning: bool = supports_sum_binning

        self.__prepared = False
        self.__nb_frames = 1
        self.__expo_time = 1.0
        self.__acquired_frames = 0
        self.__buffer_ctrl = buffer_ctrl
        self.__status: MockedState = MockedState.READY
        self.acqthread = None

    def __str__(self):
        result = ""
        result += f"Name:           {self.name}\n"
        result += f"Width x Height: {self.width} x {self.height}\n"
        result += f"Bit per pixels  {self.bpp}\n"
        result += f"Pixel size:     {self.pixel_size[0] * 1e6:0.3f} x {self.pixel_size[1] * 1e6:0.3f} um\n"
        result += f"Trigger mode    {self.trigger_mode}\n"
        result += f"State:          {self.__status}\n"
        return result

    def hard_reset(self):
        pass

    def quit(self):
        self.control = None

    def prepareAcq(self):
        if not self.__prepared:
            if self.buffer_ctrl:
                # get the buffer mgr here, to be filled in the callback funct
                self.__buffer_mgr = self.buffer_ctrl.getBuffer()
            else:
                self.__buffer_mgr = None

            self.__prepared = True
            self.__acquired_frames = 0

    @property
    def status(self):
        return self.__status

    def startAcq(self):
        if self.__acquired_frames == 0:
            self.acqthread = acqThread(self)
            self.acqthread.start()

        self.__status = MockedState.RUNNING

        rc = self.detector.doSoftwareTrigger(0)

    def stopAcq(self):
        self._stopAcq(abort=True)

    def _stopAcq(self, abort=False):
        if abort:
            self.detector.abortOperation()
            if self.acqthread:
                self.acqthread.join()
                self.acqthread = None
        self.__prepared = False
        self.__status = self.READY

    @property
    def acq_nb_frames(self):
        return self.__nb_frames

    @acq_nb_frames.setter
    def acq_nb_frames(self, frames):
        self.__nb_frames = frames

    @property
    def acq_expo_time(self):
        return self.__expo_time

    @acq_expo_time.setter
    def acq_expo_time(self, time):
        self.__expo_time = time

    @property
    def acquiredFrames(self):
        return self.__acquired_frames

    @property
    def buffer_ctrl(self):
        return self.__buffer_ctrl()


class DetInfoCtrlObj(Core.HwDetInfoCtrlObj):
    def __init__(self, camera: MockedCamera):
        Core.HwDetInfoCtrlObj.__init__(self)
        self.__camera: MockedCamera = camera
        self.__width = camera.width
        self.__height = camera.height
        self.__bpp = camera.bpp

    def getMaxImageSize(self):
        return Core.Size(self.__width, self.__height)

    def getDetectorImageSize(self):
        return self.getMaxImageSize()

    def getDefImageType(self):
        return self.__bpp

    def getCurrImageType(self):
        return self.getDefImageType()

    def setCurrImageType(self):
        raise Core.Exceptions(Core.Hardware, Core.NotSupported)

    def getPixelSize(self):
        return self.__camera.pixel_size

    def getDetectorType(self):
        return "Mock"

    def getDetectorModel(self):
        return f"{self.__camera.name}"

    def registerMaxImageSizeCallback(self, cb):
        pass

    def unregisterMaxImageSizeCallback(self, cb):
        pass

    def get_min_exposition_time(self):
        return 1e-7

    def get_max_exposition_time(self):
        return 1e6

    def get_min_latency(self):
        return 0

    def get_max_latency(self):
        return 0


class MockedSyncCtrlObj(Core.HwSyncCtrlObj):
    def __init__(self, camera, det_info):
        Core.HwSyncCtrlObj.__init__(self)
        self.__camera = weakref.ref(camera)
        self.__det_info = weakref.ref(det_info)

        # Variables
        self.__exposure = camera.acq_expo_time
        self.__latency = det_info.get_min_latency()
        self.__nb_frames = 1

    def checkTrigMode(self, trig_mode):
        return trig_mode is not None

    def setTrigMode(self, trig_mode):
        cam = self.__camera()
        if self.checkTrigMode(trig_mode):
            cam.trigger_mode = trig_mode
        else:
            raise Core.Exceptions(Core.Hardware, Core.NotSupported)

    def getTrigMode(self):
        cam = self.__camera()
        trig_mode = cam.trigger_mode
        return trig_mode

    def setExpTime(self, exp_time):
        self.__exposure = exp_time
        cam = self.__camera()
        cam.acq_expo_time = exp_time

    def getExpTime(self):
        if self.__exposure is None:
            cam = self.__camera()
            self.__exposure = cam.acq_expo_time
        return self.__exposure

    def setLatTime(self, lat_time):
        self.__latency = lat_time

    def getLatTime(self):
        return self.__latency

    def setNbFrames(self, nb_frames):
        self.__nb_frames = nb_frames

    def getNbFrames(self):
        return self.__nb_frames

    def setNbHwFrames(self, nb_frames):
        self.setNbFrames(nb_frames)

    def getNbHwFrames(self):
        return self.getNbHwFrames()

    def getValidRanges(self):
        det_info = self.__det_info()
        return Core.HwSyncCtrlObj.ValidRangesType(
            det_info.get_min_exposition_time(),
            det_info.get_max_exposition_time(),
            det_info.get_min_latency(),
            det_info.get_max_latency(),
        )

    def prepareAcq(self):
        cam = self.__camera()
        exposure = self.__exposure
        cam.acq_nb_frames = self.__nb_frames
        cam.exposure = exposure


class MockedHwBinCtrlObj(Core.HwBinCtrlObj):
    def __init__(self, camera: MockedCamera):
        Core.HwBinCtrlObj.__init__(self)
        self.__camera: MockedCamera = camera

    def setBin(self, bin: Core.Bin):
        self.__camera.binning = Core.Bin(bin.getX(), bin.getY())

    def getBin(self) -> Core.Bin:
        bin = self.__camera.binning
        return Core.Bin(bin.getX(), bin.getY())

    def checkBin(self, bin: Core.Bin) -> Core.Bin:
        return Core.Bin(bin.getX(), bin.getY())


class MockedInterface(Core.HwInterface):
    def __init__(self, camera: MockedCamera | None = None):
        Core.HwInterface.__init__(self)
        self.__buffer = Core.SoftBufferCtrlObj()

        self.__camera: MockedCamera
        if camera is None:
            self.__camera = MockedCamera(self.__buffer)
        else:
            self.__camera = camera

        self.__acquisition_start_flag = False

        self.__detInfo = DetInfoCtrlObj(self.__camera)
        self.__syncObj = MockedSyncCtrlObj(self.__camera, self.__detInfo)
        self.__capabilities = [
            self.__detInfo,
            self.__syncObj,
            self.__buffer,
        ]

        if self.__camera.supports_sum_binning:
            binCtrlObj = MockedHwBinCtrlObj(self.__camera)
            self.__capabilities.append(binCtrlObj)

    def __del__(self):
        self.__camera.quit()

    def quit(self):
        self.__camera.quit()

    def getCapList(self):
        return [Core.HwCap(x) for x in self.__capabilities]

    def reset(self, reset_level):
        if reset_level == self.HardReset:
            self.__camera.hard_reset()

    def prepareAcq(self):
        self.__camera.prepareAcq()
        self.__syncObj.prepareAcq()
        self.__image_number = 0

    def startAcq(self):
        self.__acquisition_start_flag = True
        self.__camera.startAcq()
        self.__image_number += 1

    def stopAcq(self):
        self.__camera.stopAcq()

    def getStatus(self):
        camserverStatus = self.__camera.status
        status = Core.HwInterface.StatusType()

        if camserverStatus == MockedState.ERROR:
            status.det = Core.DetFault
            status.acq = Core.AcqFault
        else:
            if camserverStatus == MockedState.RUNNING:
                status.det = Core.DetExposure
                status.acq = Core.AcqRunning
            else:
                status.det = Core.DetIdle
                lastAcquiredFrame = self.__camera.acquiredFrames - 1
                requestNbFrame = self.__syncObj.getNbFrames()
                if not self.__acquisition_start_flag or (
                    lastAcquiredFrame >= 0 and lastAcquiredFrame == (requestNbFrame - 1)
                ):
                    status.acq = Core.AcqReady
                else:
                    status.acq = Core.AcqRunning

        status.det_mask = Core.DetExposure | Core.DetFault
        return status

    def getNbAcquiredFrames(self):
        return self.__camera.acquiredFrames

    def getNbHwAcquiredFrames(self):
        return self.getNbAcquiredFrames()

    @property
    def camera(self):
        return self.__camera
