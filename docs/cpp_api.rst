C++ API
=======

Unfortunately very limited documentation is available from the source doc...

Hello, Lima!
````````````

Let's start with a simple example of an image acquisition function using the simulator camera.

.. code-block:: c++

  // A camera instance
  Simulator::Camera simu;

  // A hardware interface and the control object
  Simulator::Interface hw(simu);
  CtControl ct = CtControl(&hw);

  // Get the saving control and set some properties
  CtSaving *save = ct.saving();
  save->setDirectory("./data");
  save->setPrefix("test_");
  save->setSuffix(".edf");
  save->setNextNumber(100);
  save->setFormat(CtSaving::EDF);
  save->setSavingMode(CtSaving::AutoFrame);
  save->setFramesPerFile(100);

  // Set the binning or any other processing
  Bin bin(2, 2);
  CtImage *image = ct.image();
  image->setBin(bin);

  // Get the acquistion control and set some properties
  CtAcquisition *acq = ct.acquisition();
  acq->setAcqMode(Single);
  acq->setAcqExpoTime(expo);
  acq->setAcqNbFrames(nframe);

  // Prepare acquistion (transfer properties to the camera)
  ct.prepareAcq();

  // Start acquistion
  ct.startAcq();
  std::cout << "SIMUTEST: acq started" << std::endl;

  //
  long frame = -1;
  while (frame < (nframe - 1))
  {
  	using namespace std::chrono;

  	high_resolution_clock::time_point begin = high_resolution_clock::now();

  	usleep(100000);

  	CtControl::ImageStatus img_status;
  	ct.getImageStatus(img_status);

  	high_resolution_clock::time_point end = high_resolution_clock::now();

  	auto duration = duration_cast<microseconds>(end - begin).count();

  	std::cout << "SIMUTEST: acq frame nr " << img_status.LastImageAcquired
  		<< " - saving frame nr " << img_status.LastImageSaved << std::endl;

  	if (frame != img_status.LastImageAcquired) {
  		unsigned int nb_frames = img_status.LastImageAcquired - frame;

  		std::cout << "  " << duration << " usec for " << nb_frames << " frames\n";
  		std::cout << "  " << 1e6 * nb_frames / duration << " fps" << std::endl;

  		frame = img_status.LastImageAcquired;
  	}
  }
  std::cout << "SIMUTEST: acq finished" << std::endl;

  // Stop acquisition ( not really necessary since all frames where acquired)
  ct.stopAcq();

  std::cout << "SIMUTEST: acq stopped" << std::endl;

Main interfaces
```````````````

The control interface is the high level interface that lets your run an acquisition.

.. doxygenclass:: lima::CtControl
  :project: control
  :members:

.. doxygenclass:: lima::CtAcquisition
  :project: control
  :members:

.. doxygenclass:: lima::HwFrameCallback
  :project: hardware
  :members:

The Hardware Interface is the low level interface that must be implemented by detector plugins.

.. doxygenclass:: lima::HwInterface
  :project: hardware
  :members:

Capabilities interfaces
```````````````````````

.. doxygenclass:: lima::HwDetInfoCtrlObj
  :project: hardware
  :members:

.. doxygenclass:: lima::HwBufferCtrlObj
  :project: hardware
  :members:

.. doxygenclass:: lima::HwSyncCtrlObj
  :project: hardware
  :members:

Implementations Helpers
```````````````````````

.. doxygenclass:: lima::SoftBufferCtrlObj
  :project: hardware
  :members:
