.. Lima documentation master file, created by
   sphinx-quickstart on Fri Feb 18 10:19:02 2011.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

====================================
LIMA : Library for Image Acquisition
====================================
Lima ( **L** ibrary for **Im** age **A** cquisition) is a project for the unified control of 2D detectors. The aim is to clearly separate hardware specific code from common software configuration and features, like setting standard acquisition parameters (exposure time, external trigger), file saving and image processing.

Lima is a C++ library  which can be used for  many different cameras. The library is also available for Python_ and it provides a PyTango_  server for remote control.

The documentation is also available in `pdf`_ and `epub`_ format.

If you want to be in touch with the LIMA community, please send an email to lima@esrf.fr. You may also want to subscribe to our mailing list by sending a message to sympa@esrf.fr with `subscribe lima` as subject.

There is no binary package for lima yet but Lima is a very active project and many developments are ongoing and available from `GitHub <https://github.com/esrf-bliss/Lima>`_. However you can find stable version releases through git branches and tags on `Github releases <https://github.com/esrf-bliss/Lima/releases>`_.

For the latest changes, refers to the :download:`Release Notes <../ReleaseNotes.txt>` (Also available under the Git repository master branch as ReleaseNotes.txt text file).

.. _compilation:

.. _installation:

.. toctree::
  :maxdepth: 3
  :caption: Installation

  requirements
  getit
  build_install
  install_tango_device_server

.. toctree::
  :maxdepth: 2
  :caption: How to contribute

  howto_contribute

.. toctree::
  :maxdepth: 2
  :caption: Cameras

  supported_cameras
  future_cameras

.. toctree::
  :maxdepth: 3
  :caption: Applications

  applications/cli/doc/index
  applications/tango/python/doc/index

.. toctree::
  :maxdepth: 3
  :caption: Camera plugin developer guide

  plugin_getting_started/index

.. toctree::
  :maxdepth: 1
  :caption: Developer Documentation

  cpp_api
  python_api

You may also want to check out the `doxygen documentation`_ extracted from the source code.

.. _`doxygen documentation`: doxygen/html/index.html

.. _pdf: http://readthedocs.org/projects/lima-doc/downloads/pdf/latest/
.. _epub: http://readthedocs.org/projects/lima-doc/downloads/epub/latest/
.. _release notes: ./ReleaseNotes.txt

.. _Python: http://python.org
.. _PyTango: http://github.com/tango-cs/pytango
