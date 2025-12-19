import time
import numpy as np
from contextlib import nullcontext as does_not_raise
import pytest

from Lima import Core
from .lima_helper import LimaHelper

try:
    from Lima import Simulator
except ImportError:
    Simulator = None


ACQ_EXPO_TIME = 0.1
ACQ_NB_FRAMES = 5
ACC_NB_FRAMES = 10  # Number of frames per window


def create_mock(bpp):
    from .mocked_camera import MockedCamera
    cam = MockedCamera(
        trigger_multi=True,
        fill_frame_number=True,
        pin_corners=False,
    )
    cam.bpp = bpp
    cam.height = 10
    cam.width = 10
    return cam


def create_simulator(bpp):
    if Simulator is None:
        raise RuntimeError("Lima simulator is not available")

    class UniformCamera(Simulator.Camera):

        def __init__(self):
            super().__init__(Simulator.Camera.Mode.MODE_GENERATOR)

        def fillData(self, data):
            data.buffer.fill(data.frameNumber)

    frame_dim = Core.FrameDim(Core.Size(10, 10), bpp)

    cam = UniformCamera()

    getter = cam.getFrameGetter()
    getter.setFrameDim(frame_dim)
    assert getter.getFrameDim() == frame_dim

    return cam


@pytest.fixture
def simu(request, lima_helper: LimaHelper):
    use_lima_simulator = request.config.getoption("--lima-simulator")

    bpp = request.param
    if use_lima_simulator:
        cam = create_simulator(bpp)
        hw = Simulator.Interface(cam)
        ct = Core.CtControl(hw)

    else:
        cam = create_mock(bpp)
        ct = lima_helper.control(cam)

    yield ct


def wait_acq_finished(ct: Core.CtControl, timeout=5.0):
    """Wait for the end of the acquisition

    raise:
        RuntimeError if the wait timeout
    """

    SLEEP = 0.1
    retry = int(timeout / SLEEP)

    while ct.getStatus().AcquisitionStatus == Core.AcqStatus.AcqRunning and retry > 0:
        time.sleep(SLEEP)
        retry -= 1

    if retry == 0:
        raise RuntimeError("Acquisition finished TIMEOUT")

    status = ct.getStatus()
    assert status.AcquisitionStatus == Core.AcqStatus.AcqReady

    status = ct.getImageStatus()
    assert status.LastImageReady + 1 == ACQ_NB_FRAMES


def prepare(tmp_path, ct: Core.CtControl, output_type=None, threshold=None, operation=None):
    acq = ct.acquisition()
    acq.setAcqMode(Core.AcqMode.Accumulation)
    acq.setAcqExpoTime(ACQ_EXPO_TIME)
    # acq.setLatencyTime(1)
    acq.setAcqNbFrames(ACQ_NB_FRAMES)
    acq.setAccMaxExpoTime(ACQ_EXPO_TIME / ACC_NB_FRAMES)

    acc = ct.accumulation()
    if output_type:
        acc.setOutputType(output_type)

    if threshold:
        acc.setFilter(Core.CtAccumulation.Filter.FILTER_THRESHOLD_MIN)
        acc.setThresholdBefore(threshold)
        assert acc.getFilter() == Core.CtAccumulation.Filter.FILTER_THRESHOLD_MIN
        assert acc.getThresholdBefore() == threshold
    else:
        assert acc.getFilter() == Core.CtAccumulation.Filter.FILTER_NONE
        assert acc.getThresholdBefore() == 0.

    if operation:
        acc.setOperation(operation)
        assert acc.getOperation() == operation
    else:
        assert acc.getOperation() == Core.CtAccumulation.Operation.ACC_SUM

    class ThresholdCallback(Core.CtAccumulation.ThresholdCallback):
        def aboveMax(self, data, value):
            pass

    threshold_callback = ThresholdCallback()
    acc.registerThresholdCallback(threshold_callback)

    assert acq.getAccNbFrames() == ACC_NB_FRAMES
    assert acq.getAccExpoTime() == ACQ_EXPO_TIME / ACC_NB_FRAMES

    sav = ct.saving()
    sav.setDirectory(str(tmp_path))
    sav.setPrefix("test_acc")
    sav.setFormat(Core.CtSaving.FileFormat.HDF5)
    sav.setSavingMode(Core.CtSaving.SavingMode.AutoFrame)
    sav.setOverwritePolicy(Core.CtSaving.OverwritePolicy.Overwrite)

    ct.prepareAcq()


def start(ct: Core.CtControl):
    ct.startAcq()
    status = ct.getStatus()
    assert status.AcquisitionStatus == Core.AcqStatus.AcqRunning


# def is_signed_integer(image_type):
#     if image_type == Core.ImageType.Bpp8S or image_type == Core.ImageType.Bpp16S or image_type == Core.ImageType.Bpp32S:
#         return True
#     else:
#         return False


def image_type_to_dtype(image_type: Core.ImageType):
    if image_type == Core.ImageType.Bpp8:
        return np.uint8
    elif image_type == Core.ImageType.Bpp8S:
        return np.int8
    elif image_type == Core.ImageType.Bpp16:
        return np.uint16
    elif image_type == Core.ImageType.Bpp16S:
        return np.int16
    elif image_type == Core.ImageType.Bpp32:
        return np.uint32
    elif image_type == Core.ImageType.Bpp32S:
        return np.int32
    raise ValueError(f"image_type {image_type} unsupported")


@pytest.mark.parametrize(
    ("simu, output_type, expectation"),
    [
        (Core.ImageType.Bpp8, None, does_not_raise()),
        (Core.ImageType.Bpp8S, None, does_not_raise()),
        (Core.ImageType.Bpp16, None, does_not_raise()),
        (Core.ImageType.Bpp16S, None, does_not_raise()),
        (Core.ImageType.Bpp32, None, does_not_raise()),
        (Core.ImageType.Bpp32S, None, does_not_raise()),
        (Core.ImageType.Bpp8, Core.ImageType.Bpp16S, does_not_raise()),
        (Core.ImageType.Bpp8S, Core.ImageType.Bpp16S, does_not_raise()),
        (Core.ImageType.Bpp16, Core.ImageType.Bpp16S, does_not_raise()),
        (Core.ImageType.Bpp16S, Core.ImageType.Bpp16S, does_not_raise()),
        (Core.ImageType.Bpp16S, Core.ImageType.Bpp16, pytest.raises(Core.Exception)),
        (Core.ImageType.Bpp32, Core.ImageType.Bpp16S, pytest.raises(Core.Exception)),
        (Core.ImageType.Bpp32S, Core.ImageType.Bpp16S, pytest.raises(Core.Exception)),
    ],
    indirect=["simu"]
)
def test_accumulation_filter_none(tmp_path, simu, output_type, expectation):
    with expectation:
        prepare(tmp_path, simu, output_type)
        start(simu)
        wait_acq_finished(simu, timeout=ACQ_EXPO_TIME * ACQ_NB_FRAMES + 1)

        img_size = simu.image().getImageDim().getSize()

        for i in range(0, ACQ_NB_FRAMES):
            frm = simu.ReadImage(i)
            assert frm.buffer.shape == (img_size.getHeight(), img_size.getWidth())
            if output_type is None:
                assert frm.buffer.dtype == np.int32
            else:
                assert frm.buffer.dtype == image_type_to_dtype(output_type)

            # Check upper left corner pixel
            begin = i * ACC_NB_FRAMES
            r = np.arange(begin, begin + ACC_NB_FRAMES, dtype=frm.buffer.dtype)
            expected = r.sum()
            assert frm.buffer[0, 0] == expected

            # Check all pixels
            comparison = frm.buffer == np.full(frm.buffer.shape, expected)
            assert comparison.all()


@pytest.mark.parametrize(
    ("simu, output_type, expectation"),
    [
        (Core.ImageType.Bpp8, None, does_not_raise()),
        (Core.ImageType.Bpp8S, None, does_not_raise()),
        (Core.ImageType.Bpp16, None, does_not_raise()),
        (Core.ImageType.Bpp16S, None, does_not_raise()),
        (Core.ImageType.Bpp32, None, does_not_raise()),
        (Core.ImageType.Bpp32S, None, does_not_raise()),
        (Core.ImageType.Bpp8, Core.ImageType.Bpp16S, does_not_raise()),
        (Core.ImageType.Bpp8S, Core.ImageType.Bpp16S, does_not_raise()),
        (Core.ImageType.Bpp16, Core.ImageType.Bpp16S, does_not_raise()),
        (Core.ImageType.Bpp16S, Core.ImageType.Bpp16S, does_not_raise()),
        (Core.ImageType.Bpp16S, Core.ImageType.Bpp16, pytest.raises(Core.Exception)),
        (Core.ImageType.Bpp32, Core.ImageType.Bpp16S, pytest.raises(Core.Exception)),
        (Core.ImageType.Bpp32S, Core.ImageType.Bpp16S, pytest.raises(Core.Exception)),
    ],
    indirect=["simu"]
)
def test_accumulation_filter_threshold(tmp_path, simu, output_type, expectation):
    threshold = 5

    with expectation:
        prepare(tmp_path, simu, output_type, threshold)
        start(simu)
        wait_acq_finished(simu, timeout=ACQ_EXPO_TIME * ACQ_NB_FRAMES + 1)

        img_size = simu.image().getImageDim().getSize()

        frm = simu.ReadImage(0)
        assert frm.buffer.shape == (img_size.getHeight(), img_size.getWidth())
        if output_type is None:
            assert frm.buffer.dtype == np.int32
        else:
            assert frm.buffer.dtype == image_type_to_dtype(output_type)

        # Check upper left corner pixel
        r = np.arange(0, ACC_NB_FRAMES, dtype=frm.buffer.dtype)
        acc = r[r > threshold].sum()
        assert frm.buffer[0, 0] == acc

        # Check all pixels
        comparison = frm.buffer == np.full(frm.buffer.shape, acc)
        assert comparison.all()

        for i in range(0, ACQ_NB_FRAMES):
            frm = simu.ReadImage(i)
            assert frm.buffer.shape == (img_size.getHeight(), img_size.getWidth())
            if output_type is None:
                assert frm.buffer.dtype == np.int32
            else:
                assert frm.buffer.dtype == image_type_to_dtype(output_type)

            # Check upper left corner pixel
            begin = i * ACC_NB_FRAMES
            r = np.arange(begin, begin + ACC_NB_FRAMES, dtype=frm.buffer.dtype)
            expected = r[r > threshold].sum()
            assert frm.buffer[0, 0] == expected

            # Check all pixels
            comparison = frm.buffer == np.full(frm.buffer.shape, expected)
            assert comparison.all()


@pytest.mark.parametrize(
    ("simu, output_type, expectation"),
    [
        (Core.ImageType.Bpp8, None, does_not_raise()),
        (Core.ImageType.Bpp8S, None, does_not_raise()),
        (Core.ImageType.Bpp16, None, does_not_raise()),
        (Core.ImageType.Bpp16S, None, does_not_raise()),
        (Core.ImageType.Bpp32, None, does_not_raise()),
        (Core.ImageType.Bpp32S, None, does_not_raise()),
        (Core.ImageType.Bpp8, Core.ImageType.Bpp16S, does_not_raise()),
        (Core.ImageType.Bpp8S, Core.ImageType.Bpp16S, does_not_raise()),
        (Core.ImageType.Bpp16, Core.ImageType.Bpp16, does_not_raise()),
        (Core.ImageType.Bpp16, Core.ImageType.Bpp16S, does_not_raise()),
        (Core.ImageType.Bpp16S, Core.ImageType.Bpp16S, does_not_raise()),
        (Core.ImageType.Bpp16S, Core.ImageType.Bpp16, pytest.raises(Core.Exception)),
        (Core.ImageType.Bpp32, Core.ImageType.Bpp16S, pytest.raises(Core.Exception)),
        (Core.ImageType.Bpp32S, Core.ImageType.Bpp16S, pytest.raises(Core.Exception)),
    ],
    indirect=["simu"]
)
def test_accumulation_mean(tmp_path, simu, output_type, expectation):
    with expectation:
        prepare(tmp_path, simu, output_type, operation=Core.CtAccumulation.Operation.ACC_MEAN)
        start(simu)
        wait_acq_finished(simu, timeout=ACQ_EXPO_TIME * 1.5 * (ACQ_NB_FRAMES + 1))

        img_size = simu.image().getImageDim().getSize()
        
        for i in range(0, ACQ_NB_FRAMES):
            frm = simu.ReadImage(i)
            assert frm.buffer.shape == (img_size.getHeight(), img_size.getWidth())
            if output_type is None:
                assert frm.buffer.dtype == np.int32
            else:
                assert frm.buffer.dtype == image_type_to_dtype(output_type)

            # Check upper left corner pixel
            begin = i * ACC_NB_FRAMES
            r = np.arange(begin, begin + ACC_NB_FRAMES, dtype=frm.buffer.dtype)
            expected = int(r.sum() / ACC_NB_FRAMES)
            assert frm.buffer[0, 0] == expected

            # Check all pixels
            # comparison = frm.buffer == np.full(frm.buffer.shape, expected)
            # assert comparison.all()
