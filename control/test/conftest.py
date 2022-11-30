import pytest

from Lima import Core, Simulator

class UniformCamera(Simulator.Camera):
    """Derive the camera in order to custom the way to render the frame"""
    def fillData(self, data):
        data.buffer.fill(1)

@pytest.fixture(scope="module")
def debug():
    mod_flags = Core.DebParams.getModuleFlags()
    type_flags = Core.DebParams.getTypeFlags()

    Core.DebParams.setTypeFlags(Core.DebParams.AllFlags)
    
    yield

    Core.DebParams.setModuleFlags(mod_flags)
    Core.DebParams.setTypeFlags(type_flags)

@pytest.fixture(params=[Core.Bpp8, Core.Bpp8S, Core.Bpp16, Core.Bpp16S])
def simu(request):

    frame_dim = Core.FrameDim(Core.Size(800, 600), request.param)

    cam = UniformCamera()
    getter = cam.getFrameGetter()
    getter.setFrameDim(frame_dim)
    assert getter.getFrameDim() == frame_dim

    hw = Simulator.Interface(cam)
    ct = Core.CtControl(hw)

    yield ct
