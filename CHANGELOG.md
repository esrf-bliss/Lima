LIMA CHANGELOG


Version v1.10.2 released on February 27th 2025
----------------------------------------------

New features

  * saving: add partial saving mode and everyNFrames parameter
  * saving: added **time of frame** dataset in Hdf5 file.

Bug Fixes

  * Very fast acquisition sometimes fails due to no buffer for BS compression, has been fixed
  * CtVideo::Image buffer python binding with empty buffer segfault, has been fixed
  * saving: fixed a small compilation error with recent compiler

Camera Updates

  * Andor3 v1.12.0
    * compiled for lima-core v1.10
    * update for sdk 3.15.30092.2, only one with bitflow driver supported by ubuntu 20.04 kernel version

  * Basler v1.11.0
    * compiled for lima-core v1.10
    * upgrade Bpp10 cameras to use 2-bytes instead of only 1

  * Meta
    * compiled for lima-core v1.10

  * Iris v1.10.2
    * compiled for lima-core v1.10
    * Resolve "Camera get in Fault state if stopped during acquisition"
    * Resolve "Timeout are not properly reported"
    * Resolve "Make the acquisition timeout configurable"
    * Update missing docs

  * Lambda v1.10.1
    * compile for lima-core v1.10
    * useless code make 1.10 not working since alloc a buffer is too early
    * fix pixel size (55x55 um)

  * Maxipix v1.10.0
    * compiled for lima-core v1.10

  * PhotonicScience v1.10.0
    * recompiled for v1.10
    * first conda package with new deps on imagestar-sdk (win only)

  * Pilatus v1.10.0
    * recompiled for lima-core v1.10
    * Resolve "stopAcq() let the camera in FAULT state"

  * Prosilica v1.10.0
    * compiled for lima-core v1.10
    * Added hardware ROI Capability
 
  * RoperScientific under preparation v1.10 pvcam-rp-sdk

  * Simulator v1.10.1
    * conda recipe:  add python to run dependencies

  * Template 
    * add tango python code

  * Xh v1.10.0
    * compiled for lima-core v1.10

  * xspress3 v1.10.1
    * compiled for lima-core v1.10
    * Remove some codes which break lima buffer logic
 
PyTango server updates v1.10.1
  * new attribute saving_every_n_frames
  * remove confusing error message printed when reading attr acc_saturated_cblevel
  * readLastImage command: use a default value which allow to read the very first index
  * fixed doc mistake for image_roi attribute


Version v1.10.1 released on July 25th 2024
------------------------------------------

New features

  * Allow constructing NumaAllocator from NumaNodeMask
  * Unify CPUMask & NumaNodeMask conversion to/from hex string

Version v1.10.0 released on June 28th 2024
------------------------------------------

New features

  * Added support for per-frame binary sideband data, the binary complement to the text-only header
  * Allow reusing compressed image blob injected by the (Eiger) HW plugin for (HDF5) saving
  * Better overall performance of accumulation, now parallelize using Processlib threadpool
  * Better handling of memory buffers at various stages of the system (hardware, accumulation, saving compression)
  * Better overall performance of the saving and limit saving impact on newFrameReady that could lead to frame drop
  * Add allocator factory, available from Python
  * NumaAllocator now handles up to 128 cores/threads and can be constructed from NumaNodeMask
  * Accumulation now provides two reduction operation: SUM or MEAN. MEAN computes the arithmetic mean over the subframes.

Bug fixes

  * Remove Allocator move constructor
  * Reading the latest image with readImage(-1) now works properly in accumulation
  * Memory allocation properly handled when switching from SINGLE to ACCUMULATION or the other way around (would run out of memory)

Camera updates

  * Eiger v1.10.0
    * Add sideband compressed blob from ZMQ stream so it can be reused by the saving subsystem
    * Add dynamic_pixel_depth in order to avoid expanding data and ensure reusing compressed blob for saving

  * Hamamatsu v1.10.0
    * Remove SDK from repository due to license constrains

  * Iris v1.10.0
    * Fix dead lock when stopping acquisition and improve reliability
    * Port to Linux SDK
    * Enable Embedded Frame Metadata
    * Implement frame queue and improve performance with realtime scheduling priority (SCHED_FIFO)
    * Fix endless acquisitions
    * Improve frame timeout management

  * Simulator v1.10.0
    * Bug-fixes in test_simulator application

  * SlsDetector v1.10.0
    * Port to slsDetectorPackage 6.0.0
    * Implement passive slsReceiver mode and move geometry reconstruction to sls::FrameAssembler
    * Improve performance, reliability and tolerance to packets loss
    * Add Jungfrau detector support, including advanced img_proc like average and gain/pedestal correction

PyTango server updates v1.10.0

  * New DATA_ARRAY v4 header, including acq_tag field with the value of the corresponding R/W attribute
  * Set the default verbose (debug) level to 2 (Fatal, Error & Warning)
  * Improve doc and fix related issues


CppTango server udpate

  * Update repository with changes from ICA @ SOLEIL


Version v1.9.23 released on November 2nd 2023
---------------------------------------------

New features

  * CtAccumulation: improve (internal) soft operations' error handling (MR-249)
  * Support acq. params modify image params (like bit depth) (MR-240)

Bug fixes

  * Avoid dead-locks due to acq. of both CtControl & CtSaving mutexes (MR-248)
  * Fix BufferCtrlMgr when only image_type signedness changes  (MR-245)

Camera updates

  * Andor3 v1.11.0 
    * update for sdk 3.15.30092.2, only one with bitflow driver supported by ubuntu 20.04 kernel version
    * Update deps to lima-core>=1.9.20 which introduces a binary incompatibility with AutoMutex class
    * Support of MARANA detector
    * Change of camera interface. Andor3 parameters are not fixed enum anymore
    * serial_number can be specified to attach interface to a specific camera. This allow to have two controlled cameras on the same PC.

  * Basler from v1.10.1 to v1.10.10  
    * effective frame rate (tango attr frame_rate) now available 
    * Hw Roi for camera with increment (e.g acA1920-50gm)
    * Fix: changing latency_time (to change frame rate) raised an exception
    * getStatus() returns Ready when in soft/hw trigger mode and camera is ready to receive a new trigger.
      An optimisation to reduce delay between frame triggering.
    * used callback instead of acquisition thread.
    * Wait that camera is ready for trigger
    * Provide blank image when missing frames
    * Fix: enforce force_video_mode should be 'false' or 'true'
    * Fix: revert fixed packet_size @ 1000 and take into account the passed value in the ctor
    * Add support for ACE2 camera models
    * Temperature reading now available from PyTango device server
    * Fix: getGain() did not return percent of the gain range but the raw value instead
    * Add support for USB3 camera interface
    * Using new SDK Pylon version 6.3

  * Dhyana v1.9.6
    * IntTrig is using sequence TUCSEN mode due to synchronization the first frame is corrupted and must be through out.
    * Fixed issue with hw trigger and long delay when TUCAM_Cap_Stop() is called:
      * Update the sdk local include files to version 2.0 20220915
      * add a test program for hw trigger mode

  * Eiger v1.9.14 
    * Filewriter: at prepare delete the remaining files in DCU storage
    * HW roi support added for 9M and 16M models
    * Update deps to lima-core>=1.9.20 which introduces a binary incompatibility with AutoMutex class
    * Camera be initialize with a memory mapped file (e.g ramdisk) to use the new MmapFileBufferAllocMgr buffer manager, 
      and have a fixed amount of memory allocated during the whole life of the camera server. This will remove the long 
      latency which can be observed during start up of an acquisition with a huge amount of frames is requested.
    * Fix: reading of plugin_status (missing AttrHelper import)
    * Cache feature for "slow reading" attributes
    
  * Imxpad v1.9.5
    * Fix: Remove setNbBuffers(1) on buffer ctrl obj
    * Fix: Tango, acquisition modes spelling mistakes (capital letters mandatory)
    * Fix: G config file saving bug fixe by Frederic Bompard (RebiX ltd.)
    
  * Fli v1.9.1
    First conda package, use SDK 1.104

  * Minipix v1.9.0
    * First release, camera in production at ESRF beamline ID20

  * Pco v1.9.10
    * Fix: Dimax timeout (just ignored for now)
    * Add missing support for Edge 5.5 GL
    * Add Edge CL LUT compression support (16 -> 12 bit)

  * Pilatus v1.9.8
    * Fix: min latency time for pilatus3 S model is set to 3ms like for pilatus2
    * Fix: threshold_gain, AUTOG (pilatus3) is now managed
    * Add temperature and humidity reading

  * Prosilica v1.9.3
    * support PvApi SDK 1.28
    * support Manta camera model
    * IntTrigMulti tigger mode added
    * latency_time added to support frame rate setting
    * Fix: Fixed wrong min/max gain

  * Simulator v1.9.10
    * Rebuild to sync with processlib 1.8.6 (using external pthread-win21 library)
    * Fix: Invalid max_roi in (prefetched) loader mode
    * Fix: frame dimensions at initialization or when switching simulation mode
    * Custom pixel size
    * Fill frame in Python
    * Fix: data race condition in FrameBuilder when used with prefetch
    * Add external trigger input
    * Tango: add frame_dim device property and R/Wattribute
    * Add HW Roi to Simulator::Interface
    * Improve HW binning check and allow independent Horz/Vert binning
    * Fix: GENERATOR_PREFETCH with HW bin and/or roi

  * NEW !! Ximea v1.9.1 (since 2021) 

  * NEW !! Zwo, not yet releases, the sdk is not available.


Version v1.9.22 released on July 5th 2023
------------------------------------------

New features

  * New camera plugin for Minipix camera from Advacam company
  * Accumulation bit-depth can be changed to from 32 to 16
  * Python camera plugin API enhanced with new sip method on HwBufferMgr class to copy frame data into the hw buffer.

Bug fixes

  * check expotime in accumulation, 0 can kill some cameras (pilatus)


Version v1.9.21 released on December 22nd 2022
----------------------------------------------

Documentation updates

  * Update the docs for conda use and list of supported camera conda packages

CI/CD and build changes

  * Make Lima's dependencies PRIVATE so they are not part of the link interface of downstream projects
  * Use GIT_SUBMODULE_PATHS to init bitshuffle submodule, to fix a new issue with gitlab runner and permissions


Version v1.9.20 released on October 20th 2022
---------------------------------------------

Bug fixes

  * Heap corruption corrected related to ThreadUtil seen on windows only (more sensitive !!!)


Version v1.9.19 released on September 23rd 2022
-----------------------------------------------

New features

  * recompiled with processlib for win32 using pthreadwin32 library


Version v1.9.16 released on April 7th 2022
------------------------------------------

Bug fixes

  * Pin OpenSSL to 1.1.1* to match Conda Forge global pinning
  * Add missing virtual destructors
  * Add missing LIMACORE_API macros on CtTestApp and AppPars


Version v1.9.15 released on March 30th 2022
-------------------------------------------

Conda

  Minor release improving mostly the CI and build process.

  * Update SIP to latest 4.19 version
  * Build Conda packages for Python 3.7, 3.8, 3.9


Version v1.9.14 released on March 29th 2022
-------------------------------------------

New features

  * New BufferMgr using Memory map: MmapBufferAllocMgr

  * Add generic test application framework for Hw & Control layers:
    * Import functionality from test Simulator & SlsDetector applications
    * Formalize command line parameter decoding
    * Add new functionalty at the Control and Test Application levels:
       * Implement sequences, each consisting in acqs with different acq-nb-frames
       * Allow sequential execution of runAcq (prepareAcq) on different threads, emulating a Tango server environment: test-nb-exec-threads



Version v1.9.13 released on March 10th 2022
-------------------------------------------

New features

  * Updated bitshutffle to version 0.4.2
  * Updated lz4 to version 1.9.3

Submodule updates

  * PyTango server v1.9.16
  * dhyana camera v1.9.2, new sdk 2.0.0
  * lambda lambda v1.9.2 new sdk 2.0.2
  * template camera added


Version v1.9.12 released on October 19th 2021
---------------------------------------------

Bug fixes

  * fix bug in setEnableLogStat to not fclose null fd 


Version v1.9.11 released on September 29th 2021
-----------------------------------------------

Bug fixes

  * Fix memory over-consumption:

    CtBuffer: systematically use malloc_trim to return unused memory to OS


Version v1.9.10 released on August 13th 2021
--------------------------------------------

Conda

To install the conda packages and since lima 1.9.8 the conda-forge channel must be set as the main one:
  * https://lima1.readthedocs.io/en/latest/build_install.html#install-binary-packages

New features

  * RoiCollection new processlib sinktask now available
  * RoiCollection and RoiCounter accept a overflow threshold value to cut off randomly defective pixel

Bug fixes

  * CtBuffer: An overflow on the max number of buffer

Camera updates

  * Dhyana v1.9.1
    * Hw Roi do not check supported camera Roi
      The Dhyana 95 camera only support X offset as multiple of 4 and width (X) as multiple of 8
    * export Tucam trigger mode and trigger edge
    * The camera supports as default mode a roller shutter and can be switch to global mode or synchronous mode. In addition the input trigger can be detected on rising or falling edge:
      * Few fixes as well, the pixelsize in meter now, the firmware version is a string.
      * Manage the cold start and take a fake acquisition, otherwise the detector does not start on the first trigger.
      * set the trigger mode in prepareAcq
    * prepareAcq must not start the acquisition
    * Some refactoring needed to move the start of the acquisition to startAcq and only prepare buffer or acquisition parameters like trigger mode in prepareAcq
    * Add ExtTrigSingle
  * Eiger v1.9.8
    * workaround for bug in restful api of simplon 1.8 with filewriter "files" command returning a null string instead of a empty string list when there is no file
    * HW saving: fix setCommonHeader() for eiger2 api pass double value
    * add threshold_energy2
    * add threshold_diff_mode
    * tango:
      * add detector_ip attribute (ro) to get the ip address of the detector DCU, useful to send curl command to the detector DCU
      * add threshold_energy2 attribute (rw)
      * add threshold_diff_mode attribute (rw)
  * Pilatus
    * v1.9.5
      * nb frames max 65535 only for IntTrig trigger mode
    * v1.9.4
      * support for live mode (frames = 0)
      * increase TmpfsBuffer to 95% of the file-system provided (typically a ramdisk)
    * v1.9.3
      * Manage S series where hw-roi is not supported: add a camera_s_serie keywork in camera.def

Version v1.9.9 released on June 1st 2021
----------------------------------------

Bug fixes

  * fix a bug introduced in version 1.9.7: Do not clear frame/common headers in CtSaving::clear(), user can use explicit commands instead
  * conda CI: conda-forge channel added for windows gitlab CI

Version v1.9.8 released on March 18th 2021
------------------------------------------

Bug fixes

  * Can compile over processlib v1.7.5 (roicollection)
  * Fix libconfig find for both windows and linux and using conda-forge new package

New features

  * Pin processlib to 1.7* version 1.8* is sip incompatible and will be used from lima-core 1.10
  * CtSaving refactoring which manages now frames per file and last file with less frames when total number of frames is not a modulo of the nb_frame_per_file


Version v1.9.7 released on January 15th 2021
-------------------------------------------

New features

  * Conda: stop support for python 2.7 and 3.6

Bug fixes

  * Check whether the acquisition is still running when detructing  CtControl and stop the aquistion accordingly
  * Call H5garbage_collect at the begin of SaveContainer_Hdf5::_prepare
  * Fix CtSaving data race conditions
    * Fix data race in CtSaving::Stream::m_cnt_status
    * Fix HwBufferMgr when nb of available buffers decreases (e.g. to_alloc is negative)
    * Fixes some artihmetic overflows, uninitialized variables and throws where code should be noexcept
  * Fix CtImage (more missing SwapDimIfRotated for binning)
  * Add missing Fault status to python hardware interface

Camera updates

  * Eiger v1.9.5
    * http_port property added for Tango device
    * Add retrigger command
    * Add high voltage reading (measured and target)
      add high voltage state reading and reset command
    * Add Warning message when Command data does not fit in buffer
    * Use better way to read DETECTOR_READOUT_TIME among different API vesions: 
      * First try reading min value, use main value if negative
    * Limit ZMQ poll time to 2 sec after an abort:
    * Acquisition does not dead-lock if dseries_end header is not received
     * Correct plugin_status attributes which conflict with standard tango status
    * Deal with trigger command issues due timeout in lengthy acquisitions (> 5 min)
    * Identify HTTP response codes in Commands; report error for codes 4xx and 5xx
    * Fix issues in IntTrigMult:    
      * Bug incrementing m_frames_triggered
      * Fix Camera and Interface status when Armed
      * Remove unnecessary check in Interface::startAcq (improved in CtControl)

  * Pilatus v1.9.2
    * In Tango plugin fix a host_port misinterpreted type, now accept port number as an integer

  * Lambda v1.9.0
    * use now X-Spectrum SDK version 1.3.2. A conda package is available for the SDK xspectrum-sdk on esrf-bcu channel

PyTango server updates

 v1.9.9
  * Bug fixes
    * shared_memory_ attributes silently ignored in read or write if the Display module is not available
    * Unused code clean up and python2 backward compatibility restored
    * acc_saturated_cblevel attribute ignores read/write if no module is loaded
    * Tango event of last-image-changed fixed to not lost last events before the acquisition finishes

  * New features
    * RoiCounter and RoiSpectrum mask now support SILX 0.14 file format
    * BackgroundSubtraction: takeNextAcquisitionAsBackground() command cam be called during an acquisition

 v1.9.8
  * New features for RoiCounters
    * makes BufferSize and MaskFile always accessible
    * added MaskFile Property and Attribute on RoiCounter
      * raise ValueError in case mask file cannot be read

Version v1.9.6 released on September 25th 2020
----------------------------------------------

New features

  * lima_conda_build script: local Conda compilation with optimization for native architecture 

Bug fixes

  * MR-131: InvalidValue exception, roi-out-of-limits when applying rotation with an image_roi 
    already defined
    fix computation of the max_roi according to the current binning and rotation
    Add equality operators for Size and their missing python bindings for Point, Size and FrameDim
    Add test for ROI computation for every bin / flip / rot 
  * MR-130: continuous acquisition and hdf5 saving broken
    fix deadlock resulting in saving task stall and consequently AcqStatus being AcqRunning
    indefinitely
    fix _calcAcqStatus when acq_nb_frames == 0
  * MR-120: stopAcq() destroying non empty allocator
    Do not destroy default Allocator until it's empty
 
Camera updates

  * Maxipix 1.9.1: fixed latency_time calculation when in hardware trigger mode
  * Pilatus 1.9.1:
     * Bug Fix: IntTrigMult trigger mode, mode to software trig each image exposing, 
                has been repaired, and can be use now to have faster software acquisition sequence
     * New Features:
       Tango: new properties to run the server on a slave computer, not only on the DCU computer:
         - tmpfs_path. default = /lima_data
         - host_name, default = localhost
         - host_port, default = 41234
         - config_path, default = /home/det/p2_det/config/cam_data/camera.def

  * Simulator 1.9.2: 
      * Fix EDF Parser (allow smaller header block of 512Kib)
      * Fix Prefetch mode (segfault and/or black images with some use cases)


New cameras

  * Arinax: The OAV B-ZOOM is an On-Axis Video-microscope with a hybrid zoom system used for
    parallax free observation of micrometer sized crystals. It is optimized for MX crystallography
    beamlines and high throughput stations in Synchrotrons.
  * QHYccd:  high-performance scientific grade CMOS and CCD cameras
   

Version v1.9.5 released on July 23th 2020
----------------------------------------

New features
 
 * HDF5 container:
   * new attributes "file_name" and "file_time" to keep track of the original
     file path and creation date.
   * adjust the image dataset size to the number of acquired frames
   * in manual saving, keep a contiguous frame dataset
   *  Avoid Tango dead-lock timeout due to lengthy memory allocations

Bug fixes
  * fix cmake find_package_handle_standard_args warning
  * fix conversion compiler warning when calling import_array macro.
  * HDF5 container:
    * Fix parallel saving:
      * Refactor SaveContainerHdf5::_File:
      * Add debug information when writing HDF5 Dataset chunk fails
      * Remove unused variables
      * Use AutoPtr for dynamically-allocated objects
      * Simplify initialization at _open
    * fixed bug with number of frames for last file.
    * adjust the image dataset to the real number of acquired frames
    
  * Fix bug in CtControl::unregisterImageStatusCallback and improve code:
    * Allow unregisterImageStatusCallback if acq is not Ready
      * Do not throw exception: it is called from cb destructor
      * Status can be AcqFault, which is perfectly legal to unregister
    * Cosmetics
    * ThreadUtils: remove obsolete declarations in ReadWriteLock

Version v1.9.4 released on May 25th 2020
----------------------------------------

New features
 
 * IntTrigMult: fix check of extra calls to CtControl::startAcq()
 * Protect if detector is Ready but has not injected yet the last image into Lima
 * HDF5: NXmeasurement does not exist, use NXcollection instead

Bug fixes

 * remove (gcc) compiler warnings 
 * memory leak in ImageZCompression (HDF5GZ)
 * CtSaving: fix thread synchronization issues
 * CtBuffer: wait for processing to finish before releasing all mapped buffers
 * CtSaving_Compression: refactoring of the ZBufferHelper class

Camera news
 
 * Eiger (v1.9.4): Dectris Eiger camera submodule now support setting for http and 
   stream port.

Version v1.9.3 released on February 21th 2020
---------------------------------------------

New features

 * HDF5 schema has been simplified and now fits better with Nexus standard.

Bug fixes

 * Fix HW saving when acq_nb_frames is not a multiple of FramesPerFile.
 * When stopping the acquisition, update the number of expected frame to save.

camera news

 * Eiger (v1.9.3): Dectris Eiger2 now supported by the eiger camera submodule
 * Basler (v1.9.3): Fixed segfault with color models, Scout model does not support 
   AcquisitionFrameCount.
 * Basler (v1.9.2): Fixed a segmentation fault error when Roi is set and camera 
   is running in continuous mode (live mode)/

Version v1.9.2 released on February 17th 2020
---------------------------------------------

New features
 
 * CI pipeline on gitlab.esrf.fr has now downstream test on simulator camera submodule

Bug fixes

 * check if hdf5 attribute exists before creating it
 * Resolve "saving statistic missed"
 * Flush statistic log file
 * Use more explicit variable name in CtControl::newImageSaved


Version v1.9.1 released on September 13th 2019
----------------------------------------------

New features

 * More conda packages for cameras are now supported:
   andor3, andor, basler, dexela, eiger, frelon,
   frelon, imxpad, maxipix, marccd, merlin,
   mythen3, pco, pilatus, pixirad, pointgrey,
   prosilica, simulator, slsdetector, ueye,
   v4l2, xh
 * Some refactoring and optimisation of the software buffer classes
 * Support added for Numa (non-uniform memory access) memory management
 
Bug Fixes

 * CBF saving format now fits with miniCBF spec for Pilatus detectors
 * Video: fixed issue when taking 8/16-bit images and then changing to Accumulation
 
New cameras
 * PCO usb
 * Frelon 16

Version v1.8.0 released on February 2nd 2019
--------------------------------------------

With this release we introduced the following new features:

 * Build system has been refactored to use CMake, a cross-platform build system (Windows and Linux)
 * Building camera either standalone or using Lima as master project
 * Better support for C++ developers (with CMake packages)
 * Install.(sh|bat) to ease build and installation for CMake averse :-)
 * Conda binary packages for processlib, LImA core and some cameras and pytango device server
 * Support for Python 3
 * CI running on ESRF's Gitlab instance
 * Documentation improvements and migrated to ReadTheDocs (https://lima1.readthedocs.io)
 * Use semantic versionning
 * New HDF5 layout, direct chunk and bitshuffle LZ4 / gzip compression support
 * New BPM pytango plugin, Beam Position Monitor
 * Simulator improvements (new prefetch and read from files modes)

## New camera plugins

 * ZWO camera plugin (thanks to Jens Krueger)
 * SlsDetector (PSI Eiger) camera plugin (thanks to Alejandro Homs)
 * FLI (Finger Lake Instrument) camera plugin (thanks to Laurent Claustre)
 * Lambda (Quantum) camera plugin (thanks to Teresa Nunez)

Stable branch core-1.7
----------------------

core-1.7.2 2017-02-03
Bug Fixes
 * video: Fixed an inconsistency for scientific (monochrome) camera between
   getSupportedVideoMode() and getMode().

core-1.7.1 2017-01-06
Features
  * new tag 1.7.0 for cameras: andor/andor3/basler/dexela/espia/frelon/
    maxipix/perkinelmer/pixirad/pointgrey/prosilica/roperscientific/simulator/
    ueye/v4l2/xh

Bug Fixes
  * maxipix: maxipix-1.7.1, Fixed for setFillMode, 2 reconstructionTask objects
    created one passed to HwInterface the other kept internally in Camera object
  * basler: basler-1.7.2,  fixed monochrome vs. color mode
  * basler: basler.-1.7.1, fixed bug introduced in 1.7.0 for Roi increments
  * saving cbf: fixed Makefile + bison parser generation
    New patch version just for CBF compilation

core-1.7.0 2017-04-01
Features
  * Makefile: Improve Makefile library dependency definitions for parallel build
  * tango: server with new saving statistics and with abort command
  * basler: for color camera added yuv422packed video format
  
  * saving header: add acquisiton and images parameters
  
  * saving: added statistics.    
    We can now have saving, compression and incoming speed statistics
    changing cbf,edf,fits,hdf5,nxs and tiff to manage statistics
    
  * software operation: add possibility to force a copy for the first operation 
    internals operation are normally done in place but it's not always
    safe with some type of memory.
    i.e: You can not memove on a memory map, this cause a crash.

  * saving: we can now write severals frames in parallel.
    To control saving concurrency, you have to change the number of writing task
    with method get/setMaxConcurrentWritingTask.
    This commit contains also a small fix for write statistics:
    As we recreate the container when changing saving type, thoses parameters
    were not keeped.
    We now set event callback in case of error when writing.
    
  * saving cbf: compession optimisation with CBFMiniHeader format.    
    For now is only available for INT 32 images.
  * control: added abort acquisition method
  * basler: Pylon5 support and find proper pylon link flags

Bug Fixes
  * video: fixed synchro to image when changing video mode.
      - WARNING: only applied when live is started on already running
  * saving cbf: fixed a bug in cbf header
  * image: fixed roi when software operation is activated (rotation,flip...)
  * image: fixed maximum roi


Stable branch core-1.6
----------------------

core-1.6.2 2016-09-29
Features
 * tango python: added support for edf.lz4
 * basler: Merge pull request for new initialisation with serial number, user name or IP address.
 * pixirad: documentation updated, final cleanup of the code.

Bug fixes
 * image: fixed getRoi, returns now full frame roi if no roi was set.

core-1.6.1 2016-08-04
Features
 * maxipix: added initialization sequence in PriamSerial contructor in case of cold start.
 * andor: added enum type for BaselineClamp() 
 * tango python andor: updated properties vs_speed/p_gain/adc_speed to DevString type as for the attributes 
   Changed baseline_clamp property/attribute to DevString type
   Managed UNSUPPORTED enum for baseline_clamp/high_capacity/fan_mode property/attributes 
   Removes label for all attributes (default is now label = attribute name) * Updated the doc/index.rst documentation 
   for new/changed properties/attributes 

Bug fixes
 * core: buildfix on arm
 * andor: fixed inverted enum value for BaselineClamp
 * tango python: fix in LimaCCDs configStore() command, now it accepts a list of module names
 * andor: fixed a bug in getImageType() throwing exception in some conditions

core-1.6.0 2016-05-23
Features
 * marccd: merged marccd from esrf
 * merlin: new plugin for Merlin camera family
 * pco:  windows 64 version working
 * xapd: refactoring and new version delivered
 * sps: move Sps project to github.com
 * maxipix: c++ version delivered !!
 * spec: added a new hook for user macros (user2_) for add and remove of roicoutner, for further use like for zap pseudo counter.
 * core: accumulation, use sse register to sum 16 bits images (optimization)
 * core: prepare optimization, for linux just retrieved memory pages instead of clearing all memory.
 * pixirad: new camera plugin for Pixirad camera family
 * rayonixhs: a new readout mode been added, HDR16, thanks to the latest rayonix craydl sdk update.
 * dexela: linux version available
 * saving: add lz4 compression for edf format
 * saving: changed the buffer write size to optimize GPFS throughput,
   this optimization was done for cbf and edf
 * perkinelmer: new version to manage XRD_1611_CP3
 * tango python: added tiff as saving format
 * core: Synchronize with ImageStatusCallbacks at the beginning of prepareAcq
   Added CtControl::set/getPrepareTimeout methods (default 2 sec)
   Include Simulator test_prepare_timeout.py script
 * frelon: added Frelon::Camera::DeadTimeChangedCallback to notify Lima (Sync)   about changes 
   in the detector-defined min_lat_time (valid_ranges)
   Added Frelon TimeCalc and Acc. Mode test Python scripts
   Added Frelon Readout & Charge-Transfer times by means of float registers * Frelon::SyncCtrlObj 
   latency time now includes the detector dead time
 * core: support dynamic min_latency_time with Accumulation in CtAcquisition
 * andor3: compressed data (Mono12Packed) speed up acquisition and IntTrigMult trigger mode added
 * imxpad: stable version working with XpadServer version 3.0.X
 * background subtraction: add constant offset before subtraction to avoid data truncated to 0
 * eiger: new version with stream and hardware saving tested
 * eiger: added camera module (hardware saving + stream)
 * saving: Could now disable the directory event feature
   add frames per file parameters in hardware saving
 * tango: split python and cpp server into two submodules
 * v4l2:f ixed pb when calling prepareAcq() several times
   now start in the native camera video mode. fixed setMode   
 * ueye: patched for new sdk (4.61)
 * prosilica: new version with monitor mode
 * core: Manage monitor mode for cameras like prosilica or basler
 * imxpad: for server version 3.X
 * accumulation: enhanced Accumulation and acc_mode with new mode for threshold and/or offset correction. 
   Updated Spec and Tango for that purpose too.
 * spec: add new accumulation mode for threshold and/or offset correction
 * simulator: add IntTrigMulti capability
 * saving: add timestamp in Nexus file
 * v4l2: new camera plugin v4l2 now managed.
 * CCfits and cfitsio: configure script will be called only once
 * accumulation: extend accumulation algorithm to be able to ignore noise lower than a threshold
 * added extr module type CameraPlugin for exception
 * added extra path for future maxipix_c++ in configure.py

Bug Fixes
 * core: accumulation, fixed a memory allocation.
 * basler: external trigger fixed for ace,scout and pilot cameras
 * imxpad: camWaitAcqEnd remove from prepareAcq and startAcq, waiting time move to 1s for camWaitAcqEnd()
   SetModuleMask function added allowing to acquire smaller images, Abort fixed when images are transfered via TCP
 * marccd: Add Mutex to setImagePath/getImagePath/..., removed useless getNbAcquiredFrames()
 * merlin:check status for detector idle before startAcq, added image resizing & (image stacking - Soleil request)
 * pco: a lot of fixes !!
 * pilatus: extra delay removed due to wrong test in internal trigger
 * ueye: fixed image bytes depth for mono 12bits camera
 * spec:fixed pb when removing a roicounter, now update properly LIMA_ROI_SRV * Set the RunLevel 
   to 5 to allow other tasks (mask/bgd/ff/..) to be applied before roicounter
 * rayonixhs: added missing callback function call for readout mode change in image_type (bpp16<-->bpp32)
 * core: ThreadUtils, moved thread id to protected
 * tango python: windows, fixed path issue
 * processlib: windows, fixed compilation issue
 * core: fixed CPU overload bug in ImageStatusThread after registering multiple ImageStatusCallbacks
 * core: fixed bug crashing when SyncCtrlObj is deleted before CtAcquisition
 * image: disable roi when equal to full size (optimization)
 * andor: fix for mt-safe in error exception handling
 * saving: fixed checkValid() missing #ifdef for TIFF, only 1 frame per file supported
 * image: fixed a infinite loop in  checkDirectoryAccess() when dir is an empty string
 * image: fixed in reevalutation with _setHSRoi
 * andor3: fixed bugs with Bin/Roi, Bin can change the hw_roi with extra stride columns
   fixed bug with Bin enum. and bug with getBin
 * buffer: memory management, avoid allocating memory if hardware saving is activated (optimization)
 * image: reevaluate software roi when binning change
 * image: fixed bug with new monitor mode
 * sip: replaced tabs with blanks, otherwise will not work with python 3.
 * saving: take into account the frame id in the saving callback
 * xh:use int32_t instead of long
   remove polluting debug message
   correct sip for getMaxFrames (use str::string instead of string)
   added getMaxFrames
 * windows: fix export in ValidRangesCallback
 * espia: added Espia::BufferMgr::CamMultiFrameXferMode enumeration type
 * maxipix: fixed bug in Maxipix priam port number, now range from 1 to 5
 * tango python: fixed again wizard mode
 * processlib: windows compilation + roi counters mask patch
 * prosilica: fixed a bug with non-color camera continuous acquisition
 * spec: fixed bug with getNames() call when server is stopped.
 * CCfits: fixed installation path
 * control: fixed deadlock in unregisterImageStatusCallback
 * video: fixing dead lock when stopping and changing expo time.
 * spec: limaroi_server_initarr: use only rois that are defined for the actual  lima ccdname
 * accumulation: fixed memory allocation.


Stable branch core-1.5
----------------------

core-1.5.6 2015-12-21
Features
 * New tags for submodules: frelon-1.5.0/andor-1.5.0/pilatus-1.5.0
 * Include Simulator test_prepare_timeout.py script
 * Include COMPILE_MYTHEN3 into list of exported vars in config.inc_default
 * frelon: Added Frelon native min_latency_time (Readout & Transfer) support
 * control: Support dynamic min_latency_time with Accumulation in CtAcquisition

Bug Fixes
 * control: Synchronize with ImageStatusCallbacks at the beginning of prepareAcq
 * control: Added set/getPrepareTimeout methods (default 2 sec)

core-1.5.5 2015-12-11
Features
 * New tags for submodules: spec-1.4.7/andor3.1.5.1/tango-camera-andor3-1.5.1
 * andor3:  Compressed data (Mono12Packed) and IntTrigMult trigger mode
 * tango python: andor3 updaded, no more destride attr
 * spec: andor3 updaded, no more destride option

Bug Fixes
 * image: fixed checkValid() missing #ifdef for TIFF, only 1 frame per file supported
 * saving: Fixed a infinite loop in  checkDirectoryAccess() when dir is an empty string
 * image: fixed in reevalutation with _setHSRoi
 * basler: external trigger fixes
 * image: reevaluate software roi when binning change


core-1.5.4 2015-08-14
Features
 * New tags for submodules: Espia-1.5.1/maxipix-1.5.2/v4l2-1.5.3/ueye-1.5.0/tango-camera-andor3-1.5.0
Bug Fixes
 * hdf5: revert install in Makefile

core.1.5.3 2015-08-12
Features
 * control: accumalation mode enhanced again

core-1.5.2 2015-07-27
Features
 * New tag for andor3 needs core-1.5 for using accumulation mode
 * New tag for prosilica under core-1.5
 * simulator: add IntTrigMulti capability
 * Updated camera/prosilica
Bug Fixes
 * control: fixed deadlock in unregisterImageStatusCallback
 * video: fixing dead lock when stopping and changing expo time.

core-1.5.1 2015-06-19
Features
 * New tags: v4l2, basler, tango/camera/v4l2. basler-1.5.1/tango-camera-v4l2-1.5.1/v4l2-1.5.1 
 * Enhanced Accumulation and acc_mode with new mode for threshold and/or offset correction. Updated Spec and Tango for that purpose too.
 * accumulation: extend accumulation algorithm to be able to ignore noise lower than a threshold

