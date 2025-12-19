from __future__ import annotations

import weakref
import enum
import threading
import numpy
import logging
import time
from Lima import Core


_logger = logging.getLogger(__name__)


class MockedState(enum.Enum):
    ERROR = enum.auto()
    READY = enum.auto()
    RUNNING = enum.auto()


class AcqThread(threading.Thread):
    def __init__(self, camera: MockedCamera):
        threading.Thread.__init__(self)
        self.camera: MockedCamera = camera

    def run(self):
        time.sleep(0.1)
        try:
            self._run()
        except Exception:
            _logger.error("Error during mocked acquisition", exc_info=True)
            raise

    def _run(self):
        if self.camera.buffer_ctrl:
            buffer_mgr = self.camera.buffer_ctrl.getBuffer()
            buffer_mgr.setStartTimestamp(Core.Timestamp.now())

        self.camera.doAcquisition()

        self.camera._stopAcq()


class MockedCamera:
    Core.DEB_CLASS(Core.DebModule.DebModCamera, "mocked.Camera")

    BPP_2_NUMPY = {
        Core.ImageType.Bpp8: numpy.uint8,
        Core.ImageType.Bpp16: numpy.uint16,
        Core.ImageType.Bpp32: numpy.uint32,
        Core.ImageType.Bpp8S: numpy.int8,
        Core.ImageType.Bpp16S: numpy.int16,
        Core.ImageType.Bpp32S: numpy.int32,
    }

    def __init__(
        self,
        trigger_multi: bool = True,
        supports_sum_binning: bool = False,
        supports_roi: bool = False,
        debug_image: bool = False,
        fill_frame_number: bool = False,
        pin_corners: bool = True,
    ):
        self.debug_image = debug_image
        self.fill_frame_number = fill_frame_number
        self.pin_corners = pin_corners

        self.name = "mocked"
        self.width = 16
        self.height = 8
        self.bpp: Core.ImageType = Core.ImageType.Bpp8
        # 55um x 55 um
        self.pixel_size = 55e-6, 55e-6

        # For hardware capability
        self.binning: Core.Bin = Core.Bin(1, 1)
        self.roi: Core.Roi = Core.Roi()

        self.trigger_mode: Core.TrigMode
        if trigger_multi:
            self.trigger_mode = Core.TrigMode.IntTrigMult
        else:
            self.trigger_mode = Core.TrigMode.IntTrig

        self.supports_sum_binning: bool = supports_sum_binning
        self.supports_roi: bool = supports_roi

        self.__prepared = False
        self.__nb_frames = 1
        self.__expo_time = 1.0
        self.__acquired_frames = 0
        self.__buffer_ctrl: Core.SoftBufferCtrlObj | None = None
        self.__status: MockedState = MockedState.READY
        self.__acq_thread = None

    def _set_buffer_ctrl(self, buffer_ctrl: Core.SoftBufferCtrlObj):
        self.__buffer_ctrl = buffer_ctrl

    def __str__(self):
        result = ""
        result += f"Name:           {self.name}\n"
        result += f"Width x Height: {self.width} x {self.height}\n"
        result += f"Bit per pixels  {self.bpp}\n"
        result += f"Pixel size:     {self.pixel_size[0] * 1e6:0.3f} x {self.pixel_size[1] * 1e6:0.3f} um\n"
        result += f"Hard binning:   {self.binning}\n"
        result += f"Trigger mode    {self.trigger_mode}\n"
        result += f"State:          {self.__status}\n"
        return result

    def hard_reset(self):
        pass

    def quit(self):
        pass

    def prepareAcq(self):
        if self.__prepared:
            _logger.warning("Already prepared")
            return

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

    def _create_frame(self, frame_id: int) -> numpy.ndarray:
        dtype = self.BPP_2_NUMPY.get(self.bpp)
        if dtype is None:
            raise ValueError(f"Unsupported BPP {self.bpp} as numpy array")

        roi = self.roi
        if roi.isEmpty():
            width = self.width // self.binning.getX()
            height = self.height // self.binning.getY()
        else:
            width = roi.getSize().getWidth()
            height = roi.getSize().getHeight()

        coef = self.binning.getX() * self.binning.getY()
        if self.fill_frame_number:
            initial_value = frame_id * coef
        else:
            initial_value = coef

        array = numpy.full((height, width), initial_value, dtype=dtype)

        if self.pin_corners:
            # Pin a corner
            array[0, 0] = 0
            array[0, width - 1] = 0

        if self.debug_image:
            print(array)
        return array

    def doAcquisition(self):
        for frame in range(self.__nb_frames):
            if self.__buffer_mgr:
                frame_id = self.__acquired_frames
                frame = self._create_frame(frame_id)

                self.__buffer_mgr.copy_data(frame_id, frame)

                frame_info = Core.HwFrameInfoType()
                frame_info.acq_frame_nb = frame_id
                frame_info.frame_timestamp = Core.Timestamp.now()
                self.__buffer_mgr.newFrameReady(frame_info)
            else:
                _logger.warning("No buffer ctrl setup")

            self.__acquired_frames += 1

        # if self.trigger_mode == Core.TrigMode.IntTrigMult:
        self.__status = MockedState.READY

    def startAcq(self):
        if self.__acquired_frames == 0:
            self.__acq_thread = AcqThread(self)
            self.__acq_thread.start()

        self.__status = MockedState.RUNNING

    def stopAcq(self):
        self._stopAcq(abort=True)

    def _stopAcq(self, abort=False):
        if abort:
            if self.__acq_thread:
                self.__acq_thread.join()
                self.__acq_thread = None
        self.__prepared = False
        self.__status = MockedState.READY

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
        return self.__buffer_ctrl


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
        raise Core.Exception(Core.Layer.Hardware, Core.ErrorType.NotSupported)

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
            raise Core.Exception(Core.Layer.Hardware, Core.ErrorType.NotSupported)

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


class MockedHwRoiCtrlObj(Core.HwRoiCtrlObj):
    def __init__(self, camera: MockedCamera):
        Core.HwRoiCtrlObj.__init__(self)
        self.__camera: MockedCamera = camera

    def setRoi(self, roi: Core.Roi):
        self.__camera.roi = roi

    def getRoi(self) -> Core.Roi:
        return self.__camera.roi

    def checkRoi(self, roi: Core.Roi) -> Core.Roi:
        return roi


class MockedInterface(Core.HwInterface):
    def __init__(self, camera: MockedCamera):
        Core.HwInterface.__init__(self)
        self.__buffer_ctrl = Core.SoftBufferCtrlObj()

        self.__camera: MockedCamera = camera
        self.__camera._set_buffer_ctrl(self.__buffer_ctrl)

        self.__acquisition_start_flag = False

        self.__detInfo = DetInfoCtrlObj(self.__camera)
        self.__syncObj = MockedSyncCtrlObj(self.__camera, self.__detInfo)
        self.__capabilities = [
            self.__detInfo,
            self.__syncObj,
            self.__buffer_ctrl,
        ]

        if self.__camera.supports_sum_binning:
            binCtrlObj = MockedHwBinCtrlObj(self.__camera)
            self.__capabilities.append(binCtrlObj)

        if self.__camera.supports_roi:
            roiCtrlObj = MockedHwRoiCtrlObj(self.__camera)
            self.__capabilities.append(roiCtrlObj)

    def __del__(self):
        if self.__camera:
            self.__camera.quit()
            self.__camera = None

    def quit(self):
        if self.__camera:
            self.__camera.quit()
            self.__camera = None

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
        status = Core.HwInterface.StatusType()

        if self.__camera is None:
            status.det = Core.DetStatus.DetFault
            status.acq = Core.AcqStatus.AcqFault
            return status

        camserverStatus = self.__camera.status
        if camserverStatus == MockedState.ERROR:
            status.det = Core.DetStatus.DetFault
            status.acq = Core.AcqStatus.AcqFault
        else:
            if camserverStatus == MockedState.RUNNING:
                status.det = Core.DetStatus.DetExposure
                status.acq = Core.AcqStatus.AcqRunning
            else:
                status.det = Core.DetStatus.DetIdle
                lastAcquiredFrame = self.__camera.acquiredFrames - 1
                requestNbFrame = self.__syncObj.getNbFrames()
                if not self.__acquisition_start_flag or (
                    lastAcquiredFrame >= 0 and lastAcquiredFrame == (requestNbFrame - 1)
                ):
                    status.acq = Core.AcqStatus.AcqReady
                else:
                    status.acq = Core.AcqStatus.AcqRunning

        #status.det_mask = Core.DetStatus.DetExposure | Core.DetStatus.DetFault
        return status

    def getNbAcquiredFrames(self):
        return self.__camera.acquiredFrames

    def getNbHwAcquiredFrames(self):
        return self.getNbAcquiredFrames()

    @property
    def camera(self):
        return self.__camera
