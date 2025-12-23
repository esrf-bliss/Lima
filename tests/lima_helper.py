from __future__ import annotations

import time
from lima import core
from .mocked_camera import MockedCamera, MockedInterface


class LimaHelper:
    def __init__(self) -> None:
        self._hold_objects: dict[MockedCamera, tuple[core.HwInterface, core.CtImage, core.CtControl]] = {}

    def register(self, cam: MockedCamera):
        cam_hw = MockedInterface(cam)
        ct_control = core.CtControl(cam_hw)
        ct_image = ct_control.image()
        self._hold_objects[cam] = cam_hw, ct_control, ct_image

    def _get(self, cam: MockedCamera) -> tuple[core.HwInterface, core.CtImage, core.CtControl]:
        if cam not in self._hold_objects:
            self.register(cam)
        return self._hold_objects[cam]

    def interface(self, cam: MockedCamera) -> core.HwInterface:
        cached = self._get(cam)
        return cached[0]

    def control(self, cam: MockedCamera) -> core.CtControl:
        cached = self._get(cam)
        return cached[1]

    def image(self, cam: MockedCamera) -> core.CtImage:
        cached = self._get(cam)
        return cached[2]

    def release_all(self):
        for k in self._hold_objects:
            interface = self.interface(k)
            interface.quit()
        self._hold_objects = {}

    def process_acquisition(self, ct_control: core.CtControl):
        ct_control.prepareAcq()
        ct_control.startAcq()
        for _ in range(10):
            status = ct_control.getStatus()
            if status.AcquisitionStatus == core.AcqStatus.AcqReady:
                return
            time.sleep(1.0)
        else:
            ct_control.stopAcq()
            raise TimeoutError("Acquisition not terminated")
