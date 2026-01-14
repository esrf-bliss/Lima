"""
Make sure the mocked camera properly work before testing Lima with.
"""

from __future__ import annotations

from lima import core

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
    ct_control = core.CtControl(cam_hw)
    ct_image = ct_control.image()
    hard = ct_image.getHard()
    assert hard.hasBinCapability()

    binningCtrl = cam_hw.getHwCtrlObj(core.HwCap.Type.Bin)
    assert binningCtrl.getBin() == core.Bin(1, 1)
    binningCtrl.setBin(core.Bin(2, 2))
    assert binningCtrl.getBin() == core.Bin(2, 2)
    assert binningCtrl.checkBin(core.Bin(3, 3)) == core.Bin(3, 3)
