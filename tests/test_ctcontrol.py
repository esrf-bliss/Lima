from __future__ import annotations

import time
import numpy
from Lima import Core

from .mocked_camera import MockedCamera, MockedInterface


def test_mocked_control():
    cam = MockedCamera()
    cam_hw = MockedInterface(cam)
    ct_control = Core.CtControl(cam_hw)
    ct_image = ct_control.image()
    assert ct_image is not None
    print()
    print(cam)


def process_acquisition(ct_control):
    ct_control.prepareAcq()
    ct_control.startAcq()
    for _ in range(10):
        status = ct_control.getStatus()
        if status.AcquisitionStatus == Core.AcqReady:
            return
        time.sleep(1.0)
    else:
        ct_control.stopAcq()
        raise TimeoutError("Acquisition not terminated")


def test_acquisition_soft():
    cam = MockedCamera()
    cam_hw = MockedInterface(cam)
    ct_control = Core.CtControl(cam_hw)
    ct_image = ct_control.image()
    ct_image.setBin(Core.Bin(2, 2))
    process_acquisition(ct_control)

    # Check the hardware
    assert cam.binning == Core.Bin(1, 1)

    # Check the resulting frame
    data = ct_control.ReadImage(0)
    array = data.buffer
    assert array.dtype == numpy.uint8
    assert array.shape == (4, 8)
    expected_1st_row = [3, 4, 4, 4, 4, 4, 4, 3]
    numpy.testing.assert_allclose(array[0], expected_1st_row)


def test_acquisition_hard_binning():
    cam = MockedCamera(supports_sum_binning=True)
    cam_hw = MockedInterface(cam)
    ct_control = Core.CtControl(cam_hw)
    ct_image = ct_control.image()
    ct_image.setBin(Core.Bin(2, 2))
    process_acquisition(ct_control)

    # Check the hardware
    assert cam.binning == Core.Bin(2, 2)

    # Check the resulting frame
    data = ct_control.ReadImage(0)
    array = data.buffer
    assert array.dtype == numpy.uint8
    assert array.shape == (4, 8)
    expected_1st_row = [0, 4, 4, 4, 4, 4, 4, 0]
    numpy.testing.assert_allclose(array[0], expected_1st_row)


def test_acquisition_hard_roi():
    cam = MockedCamera(supports_roi=True)
    cam_hw = MockedInterface(cam)
    ct_control = Core.CtControl(cam_hw)
    ct_image = ct_control.image()
    ct_image.setRoi(Core.Roi(0, 0, 8, 4))
    process_acquisition(ct_control)

    # Check the hardware
    assert cam.roi == Core.Roi(0, 0, 8, 4)

    # Check the resulting frame
    data = ct_control.ReadImage(0)
    array = data.buffer
    assert array.dtype == numpy.uint8
    assert array.shape == (4, 8)
    expected_1st_row = [0, 1, 1, 1, 1, 1, 1, 0]
    numpy.testing.assert_allclose(array[0], expected_1st_row)
