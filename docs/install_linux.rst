.. _linux:

Linux
=====

Prerequisite
````````````
Before installing Lima on Linux platform, you need to install the following packages :

	- Python 2.6 or more recent
	- GCC
	- Git
	- Gnu Scientific Library, libgsl is available within most of the linux flavour
	- sip tool (version >=4.2), used for generating the python binding

GetIt
`````
As Lima is not packaged,the only way for now is to retreive it from the git repository.

**Command to get all sources:**

.. code-block:: bash 

  seb@pcbliss01:~/$ git clone --recursive git://github.com/esrf-bliss/Lima.git

**Commands for a minimum checkout to get all source needed for a particular camera:**

.. code-block:: bash

  seb@pcbliss01:~/$ git clone git://github.com/esrf-bliss/Lima.git
  seb@pcbliss01:~/$ cd Lima
  seb@pcbliss01:~/Lima$ git submodule init third-party/Processlib third-party/Sps third-party/libconfig
  seb@pcbliss01:~/Lima$ git submodule init camera/CAMERA_YOU_WANT_TO_COMPILE
  seb@pcbliss01:~/Lima$ git submodule update

In addition (but optional) you can get the **TANGO** python and/or C++ device servers, so update your git clone again:

 .. code-block:: bash

  seb@pcbliss01:~/Lima$ git submodule init application/tango/python
  seb@pcbliss01:~/Lima$ git submodule update

  OR

  seb@pcbliss01:~/Lima$ git submodule init application/tango/cpp
  seb@pcbliss01:~/Lima$ git submodule update


Particular version
``````````````````
Stable versions of lima are tracked via Git branches and Git tags. So you can retrieve any particular version using git tools.
Please refer to the release notes document `release notes`_ , for more information of the latest release and tags.

For instance if you want to get version 1.6.1 of Lima core, do:

.. code-block:: bash

  seb@pcbliss01:~/Lima$ git checkout core-1.6.1
  seb@pcbliss01:~/Lima$ git submodule init
  seb@pcbliss01:~/Lima$ git submodule update

.. _git: http://git-scm.com/

.. _linux_compilation:

Compilation
```````````
Everything is managed by the root Makefile. 

* So first generate the config.inc file.

.. code-block:: sh

  make

* Edit the configuration file **config.inc** 

.. code-block:: sh

  COMPILE_CORE=1
  COMPILE_SIMULATOR=1
  COMPILE_SPS_IMAGE=1
  COMPILE_ESPIA=0
  COMPILE_FRELON=0
  COMPILE_MAXIPIX=0
  COMPILE_PILATUS=0
  COMPILE_CBF_SAVING=0
  COMPILE_CONFIG=1
  export COMPILE_CORE COMPILE_SPS_IMAGE COMPILE_SIMULATOR \
         COMPILE_ESPIA COMPILE_FRELON COMPILE_MAXIPIX COMPILE_PILATUS \
         COMPILE_CBF_SAVING

* Configure all python modules (don't need if you just want C++ libraries)

.. code-block:: sh

  make config

* Finally compile all C++ libraries

.. code-block:: sh

  make

* If you need Python modules

.. code-block:: sh

  make -C sip -j3

**That's all folks ;)**
  
.. _linux_installation:

Installation
````````````
Installation on Linux is pretty easy because it's managed by Makefile's.
But those Makefile's can only be used if you have compiled everything including Python modules. Otherwise It'll fail. See :ref:`linux_compilation`

.. code-block:: sh

  make install

you can specify the destination path with this variable **INSTALL_DIR**

With your new installation you may need to update your environment for both python and library paths:

.. code-block:: sh

  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<my-new-install-dir>/Lima/lib
  
  export PYTHONPATH=$PYTHONPATH:<my-new-install-dir>



**WARNING**: *"make install"* only installed C++ libs and python modules, the application like the python Tango server (LimaCCDs) code remains under applications/tango. Please go to :ref:`tango_installation` for further instructions.

.. _release notes: ../../../ReleaseNotes.txt
