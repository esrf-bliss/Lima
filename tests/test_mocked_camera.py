"""
Make sure the mocked camera properly work before testing Lima with.
"""

from __future__ import annotations

from Lima import Core

from .mocked_camera import MockedCamera, MockedInterface


def test_default_capability():
    cam = MockedCamera()
    cam_hw = MockedInterface(cam)
    caps = cam_hw.getCapList()
    assert len(caps) == 3


def test_bin_capability():
    cam = MockedCamera(supports_sum_binning=True)
    cam_hw = MockedInterface(cam)
    caps = cam_hw.getCapList()
    assert len(caps) == 4
    ct_control = Core.CtControl(cam_hw)
    ct_image = ct_control.image()
    hard = ct_image.getHard()
    assert hard.hasBinCapability()

    binningCtrl = cam_hw.getHwCtrlObj(Core.HwCap.Bin)
    assert binningCtrl.getBin() == Core.Bin(1, 1)
    binningCtrl.setBin(Core.Bin(2, 2))
    assert binningCtrl.getBin() == Core.Bin(2, 2)
    assert binningCtrl.checkBin(Core.Bin(3, 3)) == Core.Bin(3, 3)
