.. image:: http://lima.blissgarden.org/_static/lima-logo-2-40perc.png

=============================
Library for Image Acquisition
=============================

|Linux Status|
|Windows Status|

Description
-----------

Lima ( **L** ibrary for **Im** age **A** cquisition) is a project for the unified control of 2D detectors. The aim is to clearly separate hardware specific code from common software configuration and features, like setting standard acquisition parameters (exposure time, external trigger), file saving and image processing.

Lima is a C++ library  which can be used for  many different cameras. The library is also available for Python_ and it provides a PyTango_  server for remote control.

Documentation:  http://lima.blissgarden.org

.. _requirements:


Requirements
------------

You need to install some tools and libraries for building Lima for both windows and Linux.

Build dependencies:

- gcc for Linux
- Visual Studio 2008 for x86 or x64 for Python 2.7.x 
- Visual Studio 2008 Express for x86 only for Python 2.7.x 
- Visual Studio 2015 for x86 and x64 for Python >= 3.5 
- git_
- CMake_ >= 3.1
- gsl_: For windows, download the esrf binary package `gsl-windows`_ and install it under "c:\\program files\\" 
     

**Python** dependencies:

Lima_ can be compiled for python 2 and 3. The following packages must be installed, development versions with header files (include files) are mandatory.

- numpy_ >= 1.1
- sip_   <= 4.18

The following requirements are optional.

Saving format dependencies:

- tiff_
- zlib_: for windows, you can download the esrf binary package `zlib-windows`_ and install it under "c:\\program files\\"
- cbf_
- hdf5_
- ccfits_
- lz4_ = 1.7.x
- libconfig_: for windows you can download the ESRF binary package `libconfig-windows`_ and install it under "c:\\program files\\"

PyTango_ server dependencies:

The LimaCCDs PyTango server only works for Python 2, it will be updated for Python 3 soon !!

- PyTango_
- libtango_

.. _git: https://git-scm.com
.. _Python: http://python.org
.. _Lima: http://lima.blissgarden.org
.. _gsl: https://www.gnu.org/software/gsl
.. _gsl-windows:  http://ftp.esrf.fr/pub/bliss/lima/gsl-windows.zip
.. _zlib-windows:  http://ftp.esrf.fr/pub/bliss/lima/zlib-windows.zip
.. _libconfig-windows:  http://ftp.esrf.fr/pub/bliss/lima/libconfig-windows.zip
.. _CMake: https://cmake.org

.. _Tango: http://tango-control.org
.. _PyTango: http://github.com/tango-cs/pytango
.. _libtango: http://tango-controls.org/downloads/source

.. _numpy: http://pypi.python.org/pypi/numpy
.. _sip: https://www.riverbankcomputing.com/software/sip

.. _tiff: http://www.libtiff.org/
.. _zlib: https://zlib.net/
.. _cbf: http://www.bernstein-plus-sons.com/software/CBF
.. _hdf5: https://support.hdfgroup.org/HDF5
.. _ccfits: https://heasarc.gsfc.nasa.gov/fitsio/ccfits
.. _lz4: https://lz4.github.io/lz4
.. _libconfig: http://www.hyperrealm.com/libconfig


.. _build_installation:

Build and Install
-----------------

Using scripts
^^^^^^^^^^^^^
the **install** scripts will run cmake, compile and install. 

It accepts input arguments (see below) but it also uses a configuration file  **scripts/config.txt**. Feel free to update this file for setting a permanent configuration for your own installation.

Linux::

     [sudo] install.sh
     [--git]
     [--prefix=<desired installation path>]
     [--python-packages=<desired python installation path>]
     [options]

Windows::

  install.bat
  [--prefix=<desired installation path>]
  [--python-packages=<desired python installation path>]
  [options]

The **--git** (Linux only) option cab be used to clone the required submodules as a prerequisite. Otherwise you should install the submodules with git commands, for instance::

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
^^^^^^^^^^^

Install first the project submodules::

 $ git submodule init third-party/Processlib
 $ git submodule init camera/basler
 $ git submodule init applications/tango/python
 $ git submodule update

Run cmake in the build directory::

 $ mkdir build
 $ cd build
 $ cmake ..
     [-G "Visual Studio 9 2008 Win64" | -G "Visual Studio 9 2008" | -G "Unix Makefiles"] 
     [-DCMAKE_INSTALL_PREFIX=<desired installation path>]
     [-DPYTHON_SITE_PACKAGES_DIR=<desired python installation path>]     
     -DLIMA_ENABLE_TIFF=true
     -DLIMACAMERA_BASLER=true
     -DLIMA_ENABLE_PYTANGO_SERVER=true
     -DLIMA_ENABLE_PYTHON=true

Then compile and install::

 $ cmake --build
 $ sudo cmake --build --target install


May you need to update your environment?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you have changed the default destination path for both libraries and python modules you should update
your environment variables.

For Linux:

.. code-block:: sh

  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<my-new-install-dir>/Lima/lib
  
  export PYTHONPATH=$PYTHONPATH:<my-new-install-dir>

For Windows:

Update the system wide variables PATH for the libraries and PYTHONPATH for python.


.. _PyTango: http://github.com/tango-cs/pytango


Lima Team contact: lima@esrf.fr

.. |Linux Status| image:: https://travis-ci.org/esrf-bliss/Lima.svg?branch=cmake
                  :target: https://travis-ci.org/esrf-bliss/Lima
                  :alt:
.. |Windows Status| image:: https://ci.appveyor.com/api/projects/status/rk0yqwem1jqxwubu?svg=true 
                  :target: https://ci.appveyor.com/api/projects/status/rk0yqwem1jqxwubu/branch/cmake?svg=true

		     
.. _Python: http://python.org
.. _PyTango: http://github.com/tango-cs/pytango

