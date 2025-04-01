from __future__ import annotations

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
