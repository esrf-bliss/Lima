from __future__ import annotations

import pytest
from lima import core
from .mocked_camera import MockedCamera
from .lima_helper import LimaHelper


def test_default(lima_helper: LimaHelper):
    cam = MockedCamera()
    ct_image = lima_helper.image(cam)
    assert ct_image.getMode() == core.CtImage.ImageOpMode.HardAndSoft
    assert ct_image.getHwImageDim() == core.FrameDim(core.Size(16, 8), core.ImageType.Bpp8)
    assert ct_image.getImageDim() == core.FrameDim(core.Size(16, 8), core.ImageType.Bpp8)


@pytest.mark.parametrize(
    "mode",
    [
        pytest.param(
            core.CtImage.ImageOpMode.SoftOnly, id="soft"
        ),
        pytest.param(
            core.CtImage.ImageOpMode.HardOnly, id="hard"
        ),
        pytest.param(
            core.CtImage.ImageOpMode.HardAndSoft, id="hard_soft"
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

    ct_image.setBin(core.Bin(2, 2))
    assert soft.getSize() == core.Size(8, 4)

    ct_image.setRoi(core.Roi(0, 0, 4, 4))
    assert soft.getSize() == core.Size(4, 4)

    ct_image.setBin(core.Bin(4, 4))
    assert soft.getSize() == core.Size(2, 2)

    ct_image.setRoi(core.Roi(0, 0, 4, 2))
    assert soft.getSize() == core.Size(4, 2)

    ct_image.setBin(core.Bin(1, 1))
    assert soft.getSize() == core.Size(16, 8)


def test_binning_without_hardware_capability(lima_helper: LimaHelper):
    cam = MockedCamera()
    ct_image = lima_helper.image(cam)
    ct_image.setBin(core.Bin(2, 2))

    hard = ct_image.getHard()
    assert not hard.hasBinCapability()
    assert hard.getBin() == core.Bin(1, 1)
    assert hard.getSize() == core.Size(16, 8)

    soft = ct_image.getSoft()
    assert soft.getBin() == core.Bin(2, 2)
    # Final output
    assert soft.getSize() == core.Size(8, 4)


def test_binning_with_binning_capability(lima_helper: LimaHelper):
    cam = MockedCamera(supports_sum_binning=True)
    ct_image = lima_helper.image(cam)
    ct_image.setBin(core.Bin(2, 2))

    hard = ct_image.getHard()
    assert hard.hasBinCapability()
    assert hard.getBin() == core.Bin(2, 2)
    assert hard.getSize() == core.Size(8, 4)

    soft = ct_image.getSoft()
    assert soft.getBin() == core.Bin(1, 1)
    # Final output
    assert soft.getSize() == core.Size(8, 4)


def test_binroi_with_roi_capability(lima_helper: LimaHelper):
    """
    Test a binning and ROI when the hardware ony have ROI capability.

    It's like we are about to test the forst part of the `MEAN` binning.
    """
    cam = MockedCamera(
        supports_sum_binning=False,
        supports_roi=True,
    )

    ct_image = lima_helper.image(cam)
    ct_image.setRoi(core.Roi(4, 0, 8, 8))
    ct_image.setBin(core.Bin(2, 2))

    hard = ct_image.getHard()
    assert hard.getBin() == core.Bin(1, 1)
    assert hard.getSize() == core.Size(8, 8)
    assert hard.getRealRoi() == core.Roi(4, 0, 8, 8)

    soft = ct_image.getSoft()
    assert soft.getBin() == core.Bin(2, 2)
    # Final output
    assert soft.getSize() == core.Size(4, 4)


def test_mean_binning_with_hardware_capability_1(lima_helper: LimaHelper):
    """
    Set bin mode, then binning
    """
    cam = MockedCamera(supports_sum_binning=True)
    ct_image = lima_helper.image(cam)
    ct_image.setBinMode(core.BinMode.Bin_Mean)
    ct_image.setBin(core.Bin(2, 2))

    hard = ct_image.getHard()
    assert hard.hasBinCapability()
    assert hard.getBin() == core.Bin(1, 1)
    assert hard.getSize() == core.Size(16, 8)

    soft = ct_image.getSoft()
    assert soft.getBin() == core.Bin(2, 2)
    # Final output
    assert soft.getSize() == core.Size(8, 4)


def test_mean_binning_with_hardware_capability_2(lima_helper: LimaHelper):
    """
    Set binning, then bin mode
    """
    cam = MockedCamera(supports_sum_binning=True)
    ct_image = lima_helper.image(cam)
    hard = ct_image.getHard()
    soft = ct_image.getSoft()
    assert hard.hasBinCapability()

    ct_image.setBin(core.Bin(2, 2))
    assert hard.getBin() == core.Bin(2, 2)
    assert hard.getSize() == core.Size(8, 4)
    # It's the full frame
    assert soft.getRoi() in [core.Roi(0, 0, 8, 4), core.Roi(0, 0, 0, 0)]
    assert soft.getBin() == core.Bin(1, 1)
    assert soft.getSize() == core.Size(8, 4)

    ct_image.setBinMode(core.BinMode.Bin_Mean)
    assert hard.getBin() == core.Bin(1, 1)
    assert hard.getSize() == core.Size(16, 8)
    # It's the full frame
    assert soft.getRoi() in [core.Roi(0, 0, 8, 4), core.Roi(0, 0, 0, 0)]
    assert soft.getBin() == core.Bin(2, 2)
    # Final output
    assert soft.getSize() == core.Size(8, 4)


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
    ct_image.setRoi(core.Roi(0, 0, 16, 4))
    ct_image.setBin(core.Bin(2, 2))
    ct_image.setBinMode(core.BinMode.Bin_Mean)

    hard = ct_image.getHard()
    assert hard.hasBinCapability()
    assert hard.hasRoiCapability()
    assert hard.getBin() == core.Bin(1, 1)
    assert hard.getSetRoi() == core.Roi(0, 0, 16, 4)
    assert hard.getRealRoi() == core.Roi(0, 0, 16, 4)
    assert hard.getSize() == core.Size(16, 4)

    soft = ct_image.getSoft()
    assert soft.getBin() == core.Bin(2, 2)
    assert soft.getBinMode() == core.BinMode.Bin_Mean
    # Final output
    assert soft.getSize() == core.Size(8, 2)


def test_mean_binning_with_roi(lima_helper: LimaHelper):
    cam = MockedCamera(
        supports_sum_binning=True,
        supports_roi=True,
    )
    cam.width = 16
    cam.height = 8

    ct_image = lima_helper.image(cam)

    ct_image.setRoi(core.Roi(4, 0, 8, 8))
    ct_image.setBin(core.Bin(2, 2))
    ct_image.setBinMode(core.BinMode.Bin_Mean)


def test_mean_binning_with_roi_and_flip(lima_helper: LimaHelper):
    cam = MockedCamera(
        supports_sum_binning=True,
        supports_roi=True,
    )
    cam.width = 16
    cam.height = 8

    ct_image = lima_helper.image(cam)

    ct_image.setRoi(core.Roi(4, 0, 8, 8))
    ct_image.setBin(core.Bin(2, 2))
    ct_image.setFlip(core.Flip(False, True))
    ct_image.setBinMode(core.BinMode.Bin_Mean)
