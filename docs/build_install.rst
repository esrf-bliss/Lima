.. _build_installation:

Build and Install
-----------------

Two methods are provided to build Lima from source.

Using scripts
^^^^^^^^^^^^^
The ``install`` scripts will run cmake, compile and install.

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

For example, to install the basler camera, use the TIFF output format, the python binding and the TANGO server, one would run:

.. code-block:: bash

  $ sudo install.sh --git --install-prefix=./install --install-python-prefix=./install/python tiff basler python pytango-server

Using CMake
^^^^^^^^^^^

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

If you have changed the default destination path for both libraries and python modules you should update your environment variables.

For Linux:

.. code-block:: bash

  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<my-new-install-dir>/Lima/lib
  export PYTHONPATH=$PYTHONPATH:<my-new-install-dir>

For Windows:

.. code-block:: bash

  set PATH=%PATH%;<my-new-install-dir>\Lima\lib
  set PYTHONPATH=%PYTHONPATH%;<my-new-install-dir>

or update the system wide variables ``PATH`` for the libraries and ``PYTHONPATH`` for python.


.. _PyTango: http://github.com/tango-cs/pytango
