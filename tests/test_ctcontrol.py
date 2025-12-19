from __future__ import annotations

import pytest
import numpy
from Lima import Core

from .mocked_camera import MockedCamera
from .lima_helper import LimaHelper


def test_mocked_control(lima_helper: LimaHelper):
    cam = MockedCamera()
    ct_image = lima_helper.image(cam)
    assert ct_image is not None
    print()
    print(cam)


def test_acquisition_soft(lima_helper: LimaHelper):
    cam = MockedCamera()
    ct_control = lima_helper.control(cam)
    ct_image = lima_helper.image(cam)
    ct_image.setBin(Core.Bin(2, 2))
    lima_helper.process_acquisition(ct_control)

    # Check the hardware
    assert cam.binning == Core.Bin(1, 1)

    # Check the resulting frame
    data = ct_control.ReadImage(0)
    array = data.buffer
    assert array.dtype == numpy.uint8
    assert array.shape == (4, 8)
    expected_1st_row = [3, 4, 4, 4, 4, 4, 4, 3]
    numpy.testing.assert_allclose(array[0], expected_1st_row)


def test_acquisition_hard_binning(lima_helper: LimaHelper):
    cam = MockedCamera(supports_sum_binning=True)
    ct_control = lima_helper.control(cam)
    ct_image = lima_helper.image(cam)
    ct_image.setBin(Core.Bin(2, 2))
    lima_helper.process_acquisition(ct_control)

    # Check the hardware
    assert cam.binning == Core.Bin(2, 2)

    # Check the resulting frame
    data = ct_control.ReadImage(0)
    array = data.buffer
    assert array.dtype == numpy.uint8
    assert array.shape == (4, 8)
    expected_1st_row = [0, 4, 4, 4, 4, 4, 4, 0]
    numpy.testing.assert_allclose(array[0], expected_1st_row)


def test_acquisition_hard_roi(lima_helper: LimaHelper):
    cam = MockedCamera(supports_roi=True)
    ct_control = lima_helper.control(cam)
    ct_image = lima_helper.image(cam)
    ct_image.setRoi(Core.Roi(0, 0, 8, 4))
    lima_helper.process_acquisition(ct_control)

    # Check the hardware
    assert cam.roi == Core.Roi(0, 0, 8, 4)

    # Check the resulting frame
    data = ct_control.ReadImage(0)
    array = data.buffer
    assert array.dtype == numpy.uint8
    assert array.shape == (4, 8)
    expected_1st_row = [0, 1, 1, 1, 1, 1, 1, 0]
    numpy.testing.assert_allclose(array[0], expected_1st_row)


def test_readimage_after_changing_image_dim(lima_helper: LimaHelper):
    cam = MockedCamera()
    ct_control = lima_helper.control(cam)
    ct_image = lima_helper.image(cam)
    ct_image = ct_control.image()
    lima_helper.process_acquisition(ct_control)

    data = ct_control.ReadImage(0)
    assert data.buffer.shape == (8, 16)

    ct_image.setRoi(Core.Roi(0, 0, 8, 4))

    # The frame was already aquired, it should not change
    data = ct_control.ReadImage(0)
    assert data.buffer.shape == (8, 16)


def test_readimage_after_prepare(lima_helper: LimaHelper):
    cam = MockedCamera()
    ct_control = lima_helper.control(cam)

    # Do an acquisition so what a frame exists
    lima_helper.process_acquisition(ct_control)

    # Prepare a new acquisition
    ct_control.prepareAcq()

    # We are not allowed to read any image
    with pytest.raises(Core.Exception):
        ct_control.ReadImage(0)


def test_acquisition_mean_bin_with_binroi_capability(lima_helper: LimaHelper):
    """
    Setup a camera with bin and roi capability.

    Use MEAN bin mode, as result the bin hardware capability can't be used.

    We expect a hardware roi and a software binning.
    """
    cam = MockedCamera(
        supports_sum_binning=True,
        supports_roi=True,
        # debug_image=True,
    )
    ct_control = lima_helper.control(cam)
    ct_image = lima_helper.image(cam)
    ct_image.setBinMode(Core.BinMode.Bin_Mean)
    ct_image.setBin(Core.Bin(2, 2))
    ct_image.setRoi(Core.Roi(0, 0, 8, 2))  # in binned coord
    lima_helper.process_acquisition(ct_control)

    # Check the hardware
    assert cam.roi == Core.Roi(0, 0, 16, 4)
    assert cam.binning == Core.Bin(1, 1)

    # Check the resulting frame
    data = ct_control.ReadImage(0)
    array = data.buffer
    assert array.dtype == numpy.uint8
    assert array.shape == (2, 8)
    expected_1st_row = [0, 1, 1, 1, 1, 1, 1, 0]
    numpy.testing.assert_allclose(array[0], expected_1st_row)
