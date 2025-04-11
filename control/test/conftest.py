import pytest

from Lima import Core, Simulator


class ConstantCamera(Simulator.Camera):

    def __init__(self, fill_value):
        super().__init__(Simulator.Camera.MODE_EXTERNAL)
        self.fill_value = fill_value

    def fillData(self, data):
        data.buffer.fill(self.fill_value)


class UniformCamera(Simulator.Camera):

    def __init__(self):
        super().__init__(Simulator.Camera.MODE_EXTERNAL)

    def fillData(self, data):
        data.buffer.fill(data.frameNumber)


@pytest.fixture(scope="module")
def debug():
    mod_flags = Core.DebParams.getModuleFlags()
    type_flags = Core.DebParams.getTypeFlags()

    Core.DebParams.setTypeFlags(Core.DebParams.AllFlags)

    yield

    Core.DebParams.setModuleFlags(mod_flags)
    Core.DebParams.setTypeFlags(type_flags)


@pytest.fixture
def simu(request):
    frame_dim = Core.FrameDim(Core.Size(10, 10), request.param)

    cam = UniformCamera()

    getter = cam.getFrameGetter()
    getter.setFrameDim(frame_dim)
    assert getter.getFrameDim() == frame_dim

    hw = Simulator.Interface(cam)
    ct = Core.CtControl(hw)

    yield ct
