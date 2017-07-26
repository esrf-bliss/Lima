.. image:: http://lima.blissgarden.org/_static/lima-logo-2-40perc.png

 

Library for Image Acquisition

Description
-----------

Lima ( **L** ibrary for **Im** age **A** cquisition) is a project for the unified control of 2D detectors. The aim is to clearly separate hardware specific code from common software configuration and features, like setting standard acquisition parameters (exposure time, external trigger), file saving and image processing.

Lima is a C++ library  which can be used for  many different cameras. The library is also available for Python_ and it provides a PyTango_  server for remote control.

Documentation:  http://lima.blissgarden.org

Requirements
------------

Lima_ is compatible with python 2 and 3

Build dependencies:

- gcc
- cmake_ >= 3
- gsl_

Python dependencies:

- numpy_ >= 1.1
- sip_ >= 4.2


Optional dependencies:

- tiff_
- libz_
- cbf_
- hdf5_
- ccfits_
- lz4_

Tango_ server dependencies:

- PyTango_
- libtango_
   


How to build and install
------------------------

Using scripts
``````````````
the **install** script will run cmake, compile and install. 

It accepts input arguments (see below) but it also use the configuration file  **scripts/config.txt**. Feel free to update this file for setting a permanent configuration for your own installation.

Linux::

     [sudo] install.sh
     [--git]
     [--prefix=<desired installation path>]
     [--python-packages=<desired python installation path>]
     [options]

Windows::

  install.bat
  [--git]
  [--prefix=<desired installation path>]
  [--python-packages=<desired python installation path>]
  [options]

The **--git** option will tell the script to clone the required submodules as a prerequisite. Otherwise you should install the submodules with git commands, for instance::

 $ git submodule init third-party/Processlib
 $ git submodule init camera/basler
 $ git submodule init applications/tango/python
 $ git submodule update
 
Options are <camera-name> <saving-format> python pytango-server:

- camera-name::

  andor|andor3|basler|prosilica|adsc|mythen3|ueye|xh|xspress3|ultra|
  xpad|mythen|pco|marccd|pointgrey|imxpad|dexela|merlin|v4l2|
  eiger|pixirad|hexitec|aviex|roperscientific|rayonixhs|espia|maxipix|frelon

- saving-format::

 cbf|nxs|fits|edfgz|edflz4|tiff|hdf5

- python: to compile python module

- pytango-server: to install the PyTango_ server

Example::

 $ sudo install.sh --git 
                 --prefix=./install 
                 --python-packages=./install/python
                 tiff basler python pytango-server

Using cmake
`````````````

Install first the project submodules::

 $ git submodule init third-party/Processlib
 $ git submodule init camera/basler
 $ git submodule init applications/tango/python
 $ git submodule update

Run cmake in the build directory::

 $ mkdir build;
 $ cd build/;
 $ cmake ..
     [-DCMAKE_INSTALL_PREFIX=<desired installation path>]
     [-DPYTHON_SITE_PACKAGES_DIR=<desired python installation path>]
     -DLIMA_ENABLE_TIFF=true
     -DLIMACAMERA_BASLER=true
     -DLIMA_ENABLE_PYTANGO_SERVER=true
     -DLIMA_ENABLE_TANGO=true

Then compile and install::

 $ make
 $ sudo make install


Lima Team contact: lima@esrf.fr

.. _Python: http://python.org
.. _Lima: http://lima.blissgarden.org
.. _gsl: https://www.gnu.org/software/gsl
.. _cmake: https://cmake.org

.. _Tango: http://tango-control.org
.. _PyTango: http://github.com/tango-cs/pytango
.. _libtango: http://tango-controls.org/downloads/source

.. _numpy: http://pypi.python.org/pypi/numpy
.. _sip: https://www.riverbankcomputing.com/software/sip

.. _tiff: http://www.libtiff.org/
.. _libz: https://zlib.net/
.. _cbf: http://www.bernstein-plus-sons.com/software/CBF
.. _hdf5: https://support.hdfgroup.org/HDF5
.. _ccfits: https://heasarc.gsfc.nasa.gov/fitsio/ccfits
.. _lz4: https://lz4.github.io/lz4
