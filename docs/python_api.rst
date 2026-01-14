Python API
==========

Most of the previous sections about the user interface routines applies to the Python binding. Naturally, some specifics concerning Python come into play.

This documentation is very much a work in progress. Stay tuned!

Hello, pyLima!
``````````````

Let's start with a simple example of an image acquisition function using the simulator camera.

.. code-block:: python

  from lima import core
  from lima import simulator
  import time


  def test_mode_generator(cam, nb_frames_prefetched=0):
      if nb_frames_prefetched:
          cam.setMode(simulator.Camera.MODE_GENERATOR_PREFETCH)
          fb = cam.getFrameGetter()
          fb.setNbPrefetchedFrames(nb_frames_prefetched)
          test = fb.getNbPrefetchedFrames()
      else:
          cam.setMode(simulator.Camera.MODE_GENERATOR)
          fb = cam.getFrameGetter()

      # Add a peak
      p1 = simulator.GaussPeak(10, 10, 23, 1000) # peak at 10,10 fwhm=23 and max=1000
      fb.setPeaks([p1])


  def test_mode_loader(cam, nb_frames_prefetched=0):
      if nb_frames_prefetched:
          cam.setMode(simulator.Camera.MODE_LOADER_PREFETCH)
          fb = cam.getFrameGetter()
          fb.setNbPrefetchedFrames(nb_frames_prefetched)
          test = fb.getNbPrefetchedFrames()
      else:
          cam.setMode(simulator.Camera.MODE_LOADER)
          fb = cam.getFrameGetter()

      # Set file pattern
      fb.setFilePattern(b'input\\test_*.edf')

  cam = simulator.Camera()

  #test_mode_generator(cam)
  #test_mode_generator(cam, 10)
  #test_mode_loader(cam)
  test_mode_loader(cam, 100)

  # Get the hardware interface
  hwint = simulator.Interface(cam)

  # Get the control interface
  control = core.CtControl(hwint)

  # Get the acquisition control
  acq = control.acquisition()

  # Set new file parameters and autosaving mode
  saving = control.saving()

  pars=saving.getParameters()
  pars.directory = b'output'
  pars.prefix = b'testsimul_'
  pars.suffix = b'.edf'
  pars.fileFormat = core.CtSaving.EDF
  pars.savingMode = core.CtSaving.AutoFrame
  saving.setParameters(pars)

  acq = control.acquisition()

  # now ask for 2 sec.  exposure and 10 frames
  acq.setAcqExpoTime(0.1)
  acq.setAcqNbFrames(10)

  control.prepareAcq()
  control.startAcq()

  # wait for last image (#9) ready
  status = control.getStatus()
  lastimg = status.ImageCounters.LastImageReady
  while lastimg != 9:
    time.sleep(0.1)
    lastimg = control.getStatus().ImageCounters.LastImageReady
    status = control.getStatus()
    lastimg = status.ImageCounters.LastImageReady

  # read the first image
  im0 = control.ReadImage(0)
