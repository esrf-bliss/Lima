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

HDF5 JPEG 2000 saving
`````````````````````

When Lima is built with ``LIMA_ENABLE_HDF5`` and ``LIMA_ENABLE_HDF5_JP2K``, the
``HDF5JP2K`` saving format stores each HDF5 chunk as a JPEG 2000 codestream.
The default compression ratio is ``10.0`` and the default encoder is OpenJPEG.

.. code-block:: python

  saving = control.saving()

  saving.setDirectory(b"output")
  saving.setPrefix(b"jp2k_")
  saving.setFormat(core.CtSaving.FileFormat.HDF5JP2K)
  saving.setFormatSuffix()
  saving.setSavingMode(core.CtSaving.SavingMode.AutoFrame)

  # Optional: change the target lossy compression ratio.
  saving.setJp2kCompressionRatio(10.0)

  # Optional: select Kakadu when Lima was built with LIMA_ENABLE_KAKADU_JP2K.
  saving.setJp2kCompressionCodec(
      core.CtSaving.Jp2kCompressionCodec.JP2KKakadu
  )

OpenJPEG remains available explicitly:

.. code-block:: python

  saving.setJp2kCompressionCodec(
      core.CtSaving.Jp2kCompressionCodec.JP2KOpenJPEG
  )

Reading these files from an external process requires the matching HDF5 filter
plugin to be available to HDF5, for example through ``HDF5_PLUGIN_PATH``.
