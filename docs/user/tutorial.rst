Tutorial
--------

In this tutorial, we are going to write a program that prepares the camera and run a simple acquisition. We will be using the simulator, but every cameras should work in the same way. The program is in C++, the python binding being similar or simpler.

First some headers needs to be included :
  
- The ``simulator/camera.h`` that defines the ``Camera`` class for this specific cameras
- The ``lima/ctcontrol.h`` that defines the ``CtControl`` class which is the main user interface of LImA

If the library and plugin were not installed in the default locations, make sure to adjust the include search paths of your compiler.

.. code-block:: c++

  #include <simulator/camera.h>
  #include <lima/ctcontrol.h>

Then, the camera object is instantiated and the corresponding interface is constructed:

.. code-block:: c++

  // A camera instance
  simulator::Camera simu(/* some cameras have specific settings here, e.g. IP address */);

  // A hardware interface
  simulator::Interface hw(simu);
  
At this point, the code specific to the camera code is over and we can instantiate the :cpp:class:`lima::CtControl` object:
  
.. code-block:: c++
  
  // The main control object
  CtControl ct = lima::CtControl(&hw);


:cpp:class:`lima::CtControl` is a class that aggregates many aspects of the configuration and the control of the cameras. Here is a non exhaustive lists of controls:

+-------------+--------------------------------------------------------------------------------------------------------------+
| Control     | Description                                                                                                  |
+=============+==============================================================================================================+
| Acquisition | Controls exposure time, number of frames, trigger mode, etc...                                               |
+-------------+--------------------------------------------------------------------------------------------------------------+
| Image       | Controls cropping (ROI), binning, rotation and other processing applied either on hardware or by software... |
+-------------+--------------------------------------------------------------------------------------------------------------+
| Saving      | Controls the file format, compression, metadata...                                                           |
+-------------+--------------------------------------------------------------------------------------------------------------+
| Shutter     | Controls the shutter mode and open and closed times...                                                       |
+-------------+--------------------------------------------------------------------------------------------------------------+
| Buffer      | Controls the number of buffer, the maximum memory to use...                                                  |
+-------------+--------------------------------------------------------------------------------------------------------------+

These specific controls are accessible form the main :cpp:class:`lima::CtControl` object.

.. code-block:: c++
  
  // Get the acquisition, saving and image controls
  CtAcquisition *acq = ct.acquisition();
  CtSaving *save = ct.saving();
  CtImage *image = ct.image();

All these control objects have member functions to set their parameters, either directly or using a the :cpp:class:`Parameter` object, such as :cpp:class:`lima::CtSaving::Parameter` (nested class). Here is how we could set the saving properties of our acquisition:

.. code-block:: c++

  save->setDirectory("./data");
  save->setPrefix("test_");
  save->setSuffix(".edf");
  save->setNextNumber(100);
  save->setFormat(CtSaving::EDF);
  save->setSavingMode(CtSaving::AutoFrame);
  save->setFramesPerFile(100);

In the same way, image processing can configured to use a 2 x 2 binning:

.. code-block:: c++

  image->setBin(Bin(2, 2));

Or acquisition parameters to get 10 frames with a 0.1 sec exposure:

.. code-block:: c++

  acq->setAcqMode(Single);
  acq->setAcqExpoTime(0.1);
  acq->setAcqNbFrames(10);

Once we are happy with our settings, it's time to prepare the acquisition which perform multiple tasks such as buffer allocation, folder creation or applying the camera settings through the camera plugin and SDK.

.. code-block:: c++

  // Prepare acquisition (transfer properties to the camera)
  ct.prepareAcq();

If the preparation is successful, the acquisition can be started anytime with:

.. code-block:: c++

  // Start acquisition
  ct.startAcq();

That's all for now, have good fun with LImA!
