import time
import numpy as np
from tempfile import gettempdir
from contextlib import nullcontext as does_not_raise

import pytest

from Lima import Core


ACQ_EXPO_TIME = 0.1
ACQ_NB_FRAMES = 5
ACC_NB_FRAMES = 10  # Number of frames per window


def wait_acq_finished(ct: Core.CtControl, timeout=5.0):
    """Wait for the end of the acquisition

    raise:
        RuntimeError if the wait timeout
    """

    SLEEP = 0.1
    retry = int(timeout / SLEEP)

    while ct.getStatus().AcquisitionStatus == Core.AcqRunning and retry > 0:
        time.sleep(SLEEP)
        retry -= 1

    if retry == 0:
        raise RuntimeError("Acquisition finished TIMEOUT")

    status = ct.getStatus()
    assert status.AcquisitionStatus == Core.AcqReady

    status = ct.getImageStatus()
    assert status.LastImageReady + 1 == ACQ_NB_FRAMES


def prepare(ct: Core.CtControl, output_type=None, threshold=None, operation=None):
    acq = ct.acquisition()
    acq.setAcqMode(Core.Accumulation)
    acq.setAcqExpoTime(ACQ_EXPO_TIME)
    # acq.setLatencyTime(1)
    acq.setAcqNbFrames(ACQ_NB_FRAMES)
    acq.setAccMaxExpoTime(ACQ_EXPO_TIME / ACC_NB_FRAMES)

    acc = ct.accumulation()
    if output_type:
        acc.setOutputType(output_type)

    if threshold:
        acc.setFilter(Core.CtAccumulation.FILTER_THRESHOLD_MIN)
        acc.setThresholdBefore(threshold)
        assert acc.getFilter() == Core.CtAccumulation.FILTER_THRESHOLD_MIN
        assert acc.getThresholdBefore() == threshold
    else:
        assert acc.getFilter() == Core.CtAccumulation.FILTER_NONE
        assert acc.getThresholdBefore() == 0.

    if operation:
        acc.setOperation(operation)
        assert acc.getOperation() == operation
    else:
        assert acc.getOperation() == Core.CtAccumulation.ACC_SUM

    class ThresholdCallback(Core.CtAccumulation.ThresholdCallback):
        def aboveMax(self, data, value):
            pass

    threshold_callback = ThresholdCallback()
    acc.registerThresholdCallback(threshold_callback)

    assert acq.getAccNbFrames() == ACC_NB_FRAMES
    assert acq.getAccExpoTime() == ACQ_EXPO_TIME / ACC_NB_FRAMES

    sav = ct.saving()
    sav.setDirectory(gettempdir())
    sav.setPrefix("test_acc")
    sav.setFormat(Core.CtSaving.FileFormat.HDF5)
    sav.setSavingMode(Core.CtSaving.SavingMode.AutoFrame)
    sav.setOverwritePolicy(Core.CtSaving.Overwrite)

    ct.prepareAcq()


def start(ct: Core.CtControl):
    ct.startAcq()
    status = ct.getStatus()
    assert status.AcquisitionStatus == Core.AcqRunning


# def is_signed_integer(image_type):
#     if image_type == Core.Bpp8S or image_type == Core.Bpp16S or image_type == Core.Bpp32S:
#         return True
#     else:
#         return False


def image_type_to_dtype(image_type: Core.ImageType):
    if image_type == Core.Bpp8:
        return np.uint8
    elif image_type == Core.Bpp8S:
        return np.int8
    if image_type == Core.Bpp16:
        return np.uint16
    elif image_type == Core.Bpp16S:
        return np.int16


@pytest.mark.parametrize(
    ("simu, output_type, expectation"),
    [
        (Core.Bpp8, None, does_not_raise()),
        (Core.Bpp8S, None, does_not_raise()),
        (Core.Bpp16, None, does_not_raise()),
        (Core.Bpp16S, None, does_not_raise()),
        (Core.Bpp32, None, does_not_raise()),
        (Core.Bpp32S, None, does_not_raise()),
        (Core.Bpp8, Core.Bpp16S, does_not_raise()),
        (Core.Bpp8S, Core.Bpp16S, does_not_raise()),
        (Core.Bpp16, Core.Bpp16S, does_not_raise()),
        (Core.Bpp16S, Core.Bpp16S, does_not_raise()),
        (Core.Bpp16S, Core.Bpp16, pytest.raises(Core.Exception)),
        (Core.Bpp32, Core.Bpp16S, pytest.raises(Core.Exception)),
        (Core.Bpp32S, Core.Bpp16S, pytest.raises(Core.Exception)),
    ],
    indirect=["simu"]
)
def test_accumulation_filter_none(simu, output_type, expectation):
    with expectation:
        prepare(simu, output_type)
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
        (Core.Bpp8, None, does_not_raise()),
        (Core.Bpp8S, None, does_not_raise()),
        (Core.Bpp16, None, does_not_raise()),
        (Core.Bpp16S, None, does_not_raise()),
        (Core.Bpp32, None, does_not_raise()),
        (Core.Bpp32S, None, does_not_raise()),
        (Core.Bpp8, Core.Bpp16S, does_not_raise()),
        (Core.Bpp8S, Core.Bpp16S, does_not_raise()),
        (Core.Bpp16, Core.Bpp16S, does_not_raise()),
        (Core.Bpp16S, Core.Bpp16S, does_not_raise()),
        (Core.Bpp16S, Core.Bpp16, pytest.raises(Core.Exception)),
        (Core.Bpp32, Core.Bpp16S, pytest.raises(Core.Exception)),
        (Core.Bpp32S, Core.Bpp16S, pytest.raises(Core.Exception)),
    ],
    indirect=["simu"]
)
def test_accumulation_filter_threshold(simu, output_type, expectation):
    threshold = 5

    with expectation:
        prepare(simu, output_type, threshold)
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
        (Core.Bpp8, None, does_not_raise()),
        (Core.Bpp8S, None, does_not_raise()),
        (Core.Bpp16, None, does_not_raise()),
        (Core.Bpp16S, None, does_not_raise()),
        (Core.Bpp32, None, does_not_raise()),
        (Core.Bpp32S, None, does_not_raise()),
        (Core.Bpp8, Core.Bpp16S, does_not_raise()),
        (Core.Bpp8S, Core.Bpp16S, does_not_raise()),
        (Core.Bpp16, Core.Bpp16, does_not_raise()),
        (Core.Bpp16, Core.Bpp16S, does_not_raise()),
        (Core.Bpp16S, Core.Bpp16S, does_not_raise()),
        (Core.Bpp16S, Core.Bpp16, pytest.raises(Core.Exception)),
        (Core.Bpp32, Core.Bpp16S, does_not_raise()),
        (Core.Bpp32S, Core.Bpp16S, pytest.raises(Core.Exception)),
    ],
    indirect=["simu"]
)
def test_accumulation_mean(simu, output_type, expectation):
    with expectation:
        prepare(simu, output_type, operation=Core.CtAccumulation.ACC_MEAN)
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
