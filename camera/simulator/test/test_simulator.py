from Lima import Core
from Lima import Simulator
import time


def test_mode_generator(cam, nb_frames_prefetched = 0):
    if nb_frames_prefetched:
        cam.setMode(Simulator.Camera.MODE_GENERATOR_PREFETCH)
        fb = cam.getFrameGetter()
        fb.setNbPrefetchedFrames(nb_frames_prefetched);
    else:
        cam.setMode(Simulator.Camera.MODE_GENERATOR)
        fb = cam.getFrameGetter()    
    
    # Add a peak
    p1 = Simulator.GaussPeak(10,10,23, 1000) # peak at 10,10 fwhm=23 and max=1000
    fb.setPeaks([p1])


def test_mode_loader(cam, nb_frames_prefetched = 0):
    if nb_frames_prefetched:
        cam.setMode(Simulator.Camera.MODE_LOADER_PREFETCH)
        fb = cam.getFrameGetter()
        fb.setNbPrefetchedFrames(nb_frames_prefetched);
    else:
        cam.setMode(Simulator.Camera.MODE_LOADER)
        fb = cam.getFrameGetter()
    
    # Set file pattern
    fb.setFilePattern(b'input\\test_*.edf')

cam = Simulator.Camera()

#test_mode_generator(cam)
#test_mode_generator(cam, 10)
#test_mode_loader(cam)
test_mode_loader(cam, 100)

hwint = Simulator.Interface(cam)
control = Core.CtControl(hwint)

# setting new file parameters and autosaving mode
saving=control.saving()

pars=saving.getParameters()
pars.directory=b'output'
pars.prefix=b'testsimul_'
pars.suffix=b'.edf'
pars.fileFormat=Core.CtSaving.EDF
pars.savingMode=Core.CtSaving.AutoFrame
saving.setParameters(pars)

acq = control.acquisition()

# now ask for 2 sec. exposure and 10 frames
acq.setAcqExpoTime(0.1)
acq.setAcqNbFrames(10)

control.prepareAcq()
control.startAcq()

# wait for last image (#9) ready
status = control.getStatus()
lastimg = status.ImageCounters.LastImageReady
while lastimg !=9:
  time.sleep(0.1)
  lastimg = control.getStatus().ImageCounters.LastImageReady
  status = control.getStatus()
  lastimg = status.ImageCounters.LastImageReady

# read the first image
im0 = control.ReadImage(0)