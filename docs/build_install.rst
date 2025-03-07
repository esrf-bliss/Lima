.. _build_installation:

Build and Install
-----------------

Install binary packages
^^^^^^^^^^^^^^^^^^^^^^^

We provide Conda_ binary packages for some cameras. This is, by far, the easiest way to get started with LImA! For instance:

Install first lastest miniconda3 (https://docs.conda.io/en/latest/miniconda.html)

Install mamba package in your "base" environment to speed up your future installations, the default conda installer is very slow, so we prefer to use mamba:

.. code-block:: bash

  conda install mamba -c conda-forge

Install now the Lima camera package (e.g basler) at the same time you create the new environment for your Lima installation:

.. code-block:: bash

  mamba create -n basler -c conda-forge -c esrf-bcu lima-camera-basler

would install a fully loaded Lima and all its dependencies with the Basler camera plugin and SDK. The camera comes as a python module but is also  C++ development package that includes header files and `CMake package config <https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html>`_ files.

If you need to run the Python Tango device server you should install the Tango camera package:

.. code-block:: bash

  mamba create -n basler -c conda-forge -c esrf-bcu lima-camera-basler-tango

.. note:: The runtime libraries of the camera's SDK are provided as well but some cameras requires drivers or specific setups than needs to be installed manually.

Build from source
^^^^^^^^^^^^^^^^^

First, you need to :ref:`get_source`. Two methods are provided to build LImA from source:

- using our install script that aims to hide the complexity of CMake_;
- using CMake_ directly for developers who are already acquainted with the tool and need the extra flexibility.

Using scripts
"""""""""""""

The ``install`` scripts will run CMake_ to compile and/or install.

It accepts input arguments (see below) but it also uses a configuration file  ``scripts/config.txt``. Feel free to update this file for setting a permanent configuration for your own installation.

For Linux:

.. code-block:: bash

     [sudo] install.sh
     [--git]
     [--install-prefix=<desired installation path>]
     [--install-python-prefix=<desired python installation path>]
     [options]

For Windows:

.. code-block:: bash

  install.bat
  [--install-prefix=<desired installation path>]
  [--install-python-prefix=<desired python installation path>]
  [options]

The ``--git`` (Linux only) option can be used to clone the required submodules as a prerequisite. Otherwise you should install the submodules manually with git commands, for instance::

 $ git submodule init third-party/Processlib
 $ git submodule init camera/basler
 $ git submodule init applications/tango/python
 $ git submodule update

Options are ``<camera-name> <saving-format> python pytango-server``:

``<camera-name>`` can be a combination of any of the following options::

  andor|andor3|basler|prosilica|adsc|mythen3|ueye|xh|xspress3|ultra|
  xpad|mythen|pco|marccd|pointgrey|imxpad|dexela|merlin|v4l2|
  eiger|pixirad|hexitec|aviex|roperscientific|rayonixhs|espia|maxipix|frelon

``<saving-format>`` can be a combination of any of the following options::

  cbf|nxs|fits|edfgz|edflz4|tiff|hdf5

``python`` will install the python module

``pytango-server`` will install the PyTango_ server

For example, to install the Basler camera, use the TIFF output format, the python binding and the TANGO server, one would run:

.. code-block:: bash

  $ sudo install.sh --git --install-prefix=./install --install-python-prefix=./install/python tiff basler python pytango-server

Using CMake
"""""""""""

Install first the project submodules:

.. code-block:: bash

  git submodule init third-party/Processlib
  git submodule init camera/basler
  git submodule init applications/tango/python
  git submodule update

Run ``cmake`` in the build directory:

.. code-block:: bash

  mkdir build
  cd build
  cmake ..
     [-G "Visual Studio 15 2017 Win64" | -G "Visual Studio 15 2017" | -G "Unix Makefiles"]
     [-DCMAKE_INSTALL_PREFIX=<desired installation path>]
     [-DPYTHON_SITE_PACKAGES_DIR=<desired python installation path>]
     -DLIMA_ENABLE_TIFF=true
     -DLIMACAMERA_BASLER=true
     -DLIMA_ENABLE_PYTANGO_SERVER=true
     -DLIMA_ENABLE_PYTHON=true

Then compile and install:

.. code-block:: bash

 cmake --build
 sudo cmake --build --target install

Environment Setup
^^^^^^^^^^^^^^^^^

.. warning::
  
  If you are using Conda_, we advice against setting any environment variables that might affect the Conda environment (e.g. ``PATH``, ``PYTHONPATH``)as this one of the most common source of troubles.

If the install path for libraries and python modules are not the default, you need to update your environment variables as follow:

For Linux:

.. code-block:: bash

  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<my-custom-install-dir>/Lima/lib
  export PYTHONPATH=$PYTHONPATH:<my-custom-install-dir>

For Windows:

.. code-block:: bash

  set PATH=%PATH%;<my-custom-install-dir>\Lima\lib
  set PYTHONPATH=%PYTHONPATH%;<my-custom-install-dir>

or update the system wide variables ``PATH`` for the libraries and ``PYTHONPATH`` for python.

.. _CMake: https://cmake.org
.. _Conda: https://conda.io

.. _PyTango: http://github.com/tango-cs/pytango
