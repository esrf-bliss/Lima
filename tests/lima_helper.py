from __future__ import annotations

from Lima import Core
from .mocked_camera import MockedCamera, MockedInterface


class LimaHelper:
    def __init__(self) -> None:
        self._hold_objects: dict[MockedCamera, tuple[Core.HwInterface, Core.CtImage, Core.CtControl]] = {}

    def register(self, cam: MockedCamera):
        cam_hw = MockedInterface(cam)
        ct_control = Core.CtControl(cam_hw)
        ct_image = ct_control.image()
        self._hold_objects[cam] = cam_hw, ct_control, ct_image

    def _get(self, cam: MockedCamera) -> tuple[Core.HwInterface, Core.CtImage, Core.CtControl]:
        if cam not in self._hold_objects:
            self.register(cam)
        return self._hold_objects[cam]

    def interface(self, cam: MockedCamera) -> Core.HwInterface:
        cached = self._get(cam)
        return cached[0]

    def control(self, cam: MockedCamera) -> Core.CtControl:
        cached = self._get(cam)
        return cached[1]

    def image(self, cam: MockedCamera) -> Core.CtImage:
        cached = self._get(cam)
        return cached[2]

    def release_all(self):
        for k in self._hold_objects:
            interface = self.interface(k)
            interface.quit()
        self._hold_objects = {}
