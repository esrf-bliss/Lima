from lima import core
import time

from .mocked_camera import MockedCamera
from .lima_helper import LimaHelper


def test_readimage(lima_helper: LimaHelper):
    cam = MockedCamera()
    cam.width = 800
    cam.height = 600
    cam.bpp = core.ImageType.Bpp32

    control = lima_helper.control(cam)

    acq = control.acquisition()
    acq.setAcqNbFrames(1)

    img = control.image()
    roi = core.Roi(300, 50, 450, 300)
    img.setRoi(roi)
    # img.setBin(core.Bin(2, 2))
    img.setRotation(core.RotationMode.Rotation_90)

    control.prepareAcq()
    control.startAcq()

    while control.getStatus().AcquisitionStatus == core.AcqStatus.AcqRunning:
        time.sleep(0.1)

    control.stopAcq()

    frame = control.ReadImage(0)

    assert frame.buffer.shape == (450, 300)
