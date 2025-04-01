from __future__ import annotations

import pytest
from Lima import Core
from .mocked_camera import MockedCamera
from .lima_helper import LimaHelper


def test_default(lima_helper: LimaHelper):
    cam = MockedCamera()
    ct_image = lima_helper.image(cam)
    assert ct_image.getMode() == Core.CtImage.HardAndSoft
    assert ct_image.getHwImageDim() == Core.FrameDim(Core.Size(16, 8), Core.Bpp8)
    assert ct_image.getImageDim() == Core.FrameDim(Core.Size(16, 8), Core.Bpp8)


@pytest.mark.parametrize(
    "mode",
    [
        pytest.param(
            Core.CtImage.SoftOnly, id="soft"
        ),
        pytest.param(
            Core.CtImage.HardOnly, id="hard"
        ),
        pytest.param(
            Core.CtImage.HardAndSoft, id="hard_soft"
        ),
    ],
)
def test_binning_and_roi_changes(lima_helper: LimaHelper, mode):
    """
    Check changes between binning and ROIs
    """
    cam = MockedCamera(
        supports_sum_binning=True,
        supports_roi=True,
    )
    ct_image = lima_helper.image(cam)
    ct_image.setMode(mode)
    soft = ct_image.getSoft()

    ct_image.setBin(Core.Bin(2, 2))
    assert soft.getSize() == Core.Size(8, 4)

    ct_image.setRoi(Core.Roi(0, 0, 4, 4))
    assert soft.getSize() == Core.Size(4, 4)

    ct_image.setBin(Core.Bin(4, 4))
    assert soft.getSize() == Core.Size(2, 2)

    ct_image.setRoi(Core.Roi(0, 0, 4, 2))
    assert soft.getSize() == Core.Size(4, 2)

    ct_image.setBin(Core.Bin(1, 1))
    assert soft.getSize() == Core.Size(16, 8)


def test_binning_without_hardware_capability(lima_helper: LimaHelper):
    cam = MockedCamera()
    ct_image = lima_helper.image(cam)
    ct_image.setBin(Core.Bin(2, 2))

    hard = ct_image.getHard()
    assert not hard.hasBinCapability()
    assert hard.getBin() == Core.Bin(1, 1)
    assert hard.getSize() == Core.Size(16, 8)

    soft = ct_image.getSoft()
    assert soft.getBin() == Core.Bin(2, 2)
    # Final output
    assert soft.getSize() == Core.Size(8, 4)


def test_binning_with_hardware_capability(lima_helper: LimaHelper):
    cam = MockedCamera(supports_sum_binning=True)
    ct_image = lima_helper.image(cam)
    ct_image.setBin(Core.Bin(2, 2))

    hard = ct_image.getHard()
    assert hard.hasBinCapability()
    assert hard.getBin() == Core.Bin(2, 2)
    assert hard.getSize() == Core.Size(8, 4)

    soft = ct_image.getSoft()
    assert soft.getBin() == Core.Bin(1, 1)
    # Final output
    assert soft.getSize() == Core.Size(8, 4)


def test_mean_binning_with_hardware_capability_1(lima_helper: LimaHelper):
    """
    Set bin mode, then binning
    """
    cam = MockedCamera(supports_sum_binning=True)
    ct_image = lima_helper.image(cam)
    ct_image.setBinMode(Core.Bin_Mean)
    ct_image.setBin(Core.Bin(2, 2))

    hard = ct_image.getHard()
    assert hard.hasBinCapability()
    assert hard.getBin() == Core.Bin(1, 1)
    assert hard.getSize() == Core.Size(16, 8)

    soft = ct_image.getSoft()
    assert soft.getBin() == Core.Bin(2, 2)
    # Final output
    assert soft.getSize() == Core.Size(8, 4)


def test_mean_binning_with_hardware_capability_2(lima_helper: LimaHelper):
    """
    Set binning, then bin mode
    """
    cam = MockedCamera(supports_sum_binning=True)
    ct_image = lima_helper.image(cam)
    hard = ct_image.getHard()
    soft = ct_image.getSoft()
    assert hard.hasBinCapability()

    ct_image.setBin(Core.Bin(2, 2))
    assert hard.getBin() == Core.Bin(2, 2)
    assert hard.getSize() == Core.Size(8, 4)
    assert soft.getRoi() == Core.Roi(0, 0, 8, 4)
    assert soft.getBin() == Core.Bin(1, 1)
    assert soft.getSize() == Core.Size(8, 4)

    ct_image.setBinMode(Core.Bin_Mean)
    assert hard.getBin() == Core.Bin(1, 1)
    assert hard.getSize() == Core.Size(16, 8)
    assert soft.getRoi() == Core.Roi(0, 0, 8, 4)
    assert soft.getBin() == Core.Bin(2, 2)
    # Final output
    assert soft.getSize() == Core.Size(8, 4)


def test_mean_bin_with_binroi_capability(lima_helper: LimaHelper):
    """
    Setup a camera with bin and roi capability.

    Use MEAN bin mode, as result the bin hardware capability can't be used.

    We expect a hardware roi and Bin software binning.
    """
    cam = MockedCamera(
        supports_sum_binning=True,
        supports_roi=True,
    )
    ct_image = lima_helper.image(cam)
    ct_image.setRoi(Core.Roi(0, 0, 16, 4))
    ct_image.setBin(Core.Bin(2, 2))
    ct_image.setBinMode(Core.Bin_Mean)

    hard = ct_image.getHard()
    assert hard.hasBinCapability()
    assert hard.hasRoiCapability()
    assert hard.getBin() == Core.Bin(1, 1)
    assert hard.getSetRoi() == Core.Roi(0, 0, 16, 4)
    assert hard.getRealRoi() == Core.Roi(0, 0, 16, 4)
    assert hard.getSize() == Core.Size(16, 4)

    soft = ct_image.getSoft()
    assert soft.getBin() == Core.Bin(2, 2)
    assert soft.getBinMode() == Core.Bin_Mean
    # Final output
    assert soft.getSize() == Core.Size(8, 2)
