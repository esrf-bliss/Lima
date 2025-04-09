from __future__ import annotations

from Lima import Core

from .mocked_camera import MockedCamera, MockedInterface


def create_ct_image(cam: MockedCamera) -> Core.CtImage:
    cam_hw = MockedInterface(cam)
    ct_control = Core.CtControl(cam_hw)
    ct_image = ct_control.image()
    assert ct_image is not None

    # Hold the control else it will be released
    cam.control = ct_control
    cam.hw_interface = cam_hw

    return ct_image


def test_default():
    cam = MockedCamera()
    ct_image = create_ct_image(cam)
    assert ct_image.getMode() == Core.CtImage.HardAndSoft
    assert ct_image.getHwImageDim() == Core.FrameDim(Core.Size(16, 8), Core.Bpp8)
    assert ct_image.getImageDim() == Core.FrameDim(Core.Size(16, 8), Core.Bpp8)

    cam.quit()


def test_binning_without_hardware_capability():
    cam = MockedCamera()
    ct_image = create_ct_image(cam)
    ct_image.setBin(Core.Bin(2, 2))

    hard = ct_image.getHard()
    assert not hard.hasBinCapability()
    assert hard.getBin() == Core.Bin(1, 1)
    assert hard.getSize() == Core.Size(16, 8)

    soft = ct_image.getSoft()
    assert soft.getBin() == Core.Bin(2, 2)
    # Final output
    assert soft.getSize() == Core.Size(8, 4)

    cam.quit()


def test_binning_with_hardware_capability():
    cam = MockedCamera(supports_sum_binning=True)
    ct_image = create_ct_image(cam)
    ct_image.setBin(Core.Bin(2, 2))

    hard = ct_image.getHard()
    assert hard.hasBinCapability()
    assert hard.getBin() == Core.Bin(2, 2)
    assert hard.getSize() == Core.Size(8, 4)

    soft = ct_image.getSoft()
    assert soft.getBin() == Core.Bin(1, 1)
    # Final output
    assert soft.getSize() == Core.Size(8, 4)

    cam.quit()
