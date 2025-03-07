.. _requirements:

Requirements
------------

Some tools and libraries are required to build LImA for either Windows and Linux.

.. note::
  All the dependencies, build or runtime, are available as Conda_ packages for both Windows and Linux platforms.

Build dependencies
~~~~~~~~~~~~~~~~~~

- A C++ compiler (usually GCC for Linux and Visual Studio for Windows)

- Visual Studio 2008 for x86 or x64 for python2.7.x
- Visual Studio 2008 Express for x86 only for python2.7.x
- Visual Studio 2015 or 2017 for x86 and x64 for python >= 3.5

- CMake_ >= 3.1

Python dependencies
~~~~~~~~~~~~~~~~~~~

LImA_ is compatible with python 2 and 3.

- numpy_ >= 1.1
- sip_ >= 4.19

Optional dependencies
~~~~~~~~~~~~~~~~~~~~~

Saving format dependencies
^^^^^^^^^^^^^^^^^^^^^^^^^^

- TIFF_, Tag Image File Format (TIFF), a widely used format for storing image data ;
- zlib_, a lossless data-compression library. For Windows, you can download the ESRF binary package `zlib-windows`_ and install it under ``C:\Program Files`` ;
- CBF_, a library for accessing Crystallographic Binary Files (CBF files) and Image-supporting CIF (imgCIF) files ;
- HDF5_, a data model, library, and file format for storing and managing data ;
- CCfits_, CFITSIO_, a library for reading and writing data files in FITS (Flexible Image Transport System) data format ;
- LZ4_ >= 1.9.1, a lossless compression algorithm ;
- libconfig_, a library for processing structured configuration files. For Windows, you can download the ESRF binary package `libconfig-windows`_ and install it under ``C:\Program Files``.

PyTango server dependencies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- PyTango_, the Tango python binding
- libtango_, the Tango toolkit

.. _git: https://git-scm.com
.. _CMake: https://cmake.org
.. _Conda: https://conda.io

.. _Python: https://python.org
.. _LImA: https://lima1.readthedocs.io
.. _GSL: https://www.gnu.org/software/gsl
.. _gsl-windows:  http://ftp.esrf.fr/pub/bliss/lima/gsl-windows.zip
.. _zlib-windows:  http://ftp.esrf.fr/pub/bliss/lima/zlib-windows.zip
.. _libconfig-windows:  http://ftp.esrf.fr/pub/bliss/lima/libconfig-windows.zip

.. _Tango: http://tango-control.org
.. _PyTango: http://github.com/tango-cs/pytango
.. _libtango: http://www.tango-controls.org/downloads/

.. _numpy: http://pypi.python.org/pypi/numpy
.. _sip: https://www.riverbankcomputing.com/software/sip

.. _TIFF: http://www.libtiff.org/
.. _zlib: https://zlib.net/
.. _CBF: http://www.bernstein-plus-sons.com/software/CBF
.. _HDF5: https://support.hdfgroup.org/HDF5
.. _CCfits: https://heasarc.gsfc.nasa.gov/fitsio/ccfits
.. _CFITSIO: https://heasarc.gsfc.nasa.gov/fitsio/fitsio.html
.. _LZ4: https://lz4.github.io/lz4
.. _libconfig: http://www.hyperrealm.com/libconfig
