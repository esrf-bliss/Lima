.. LImA documentation master file, created by
   sphinx-quickstart on Fri Feb 18 10:19:02 2011.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

====================================
LImA : Library for Image Acquisition
====================================
LImA (stands for **L** ibrary for **Im** age **A** cquisition) is a project for the unified control of 2D detectors. It is used in production in `ESRF Beamlines <https://www.esrf.eu/about/synchrotron-science/beamline>`_ and in other places.

The architecture of the library aims at clearly separating hardware specific code from common software configuration and features, like setting standard acquisition parameters (exposure time, external trigger), file saving and image processing.

LImA is a C++ library but the library also comes with a Python_ binding. A PyTango_ device server for remote control is provided as well.

We provide Conda binary package for Windows and Linux for some cameras. Check out our `Conda channel <https://anaconda.org/esrf-bcu>`_.

LImA is a very active project and many developments are ongoing and available from `GitHub <https://github.com/esrf-bliss/LImA>`_. You can find stable version releases through git branches and tags on `Github releases <https://github.com/esrf-bliss/LImA/releases>`_.

If you want to get in touch with the LIMA community, please send an email to lima@esrf.fr. You may also want to subscribe to our mailing list by sending a message to `sympa@esrf.fr <mailto:sympa@esrf.fr?subject=subscribe%20lima>`_ with ``subscribe lima`` as subject.

For the latest changes, refers to the :download:`Release Notes <../ReleaseNotes.txt>`.

Note that this documentation is also available in `pdf`_ and `epub`_ format.

.. _compilation:

.. _installation:

.. toctree::
  :maxdepth: 3
  :caption: Installation

  requirements
  build_install
  install_tango_device_server

.. toctree::
  :maxdepth: 2
  :caption: Getting Started

  user/overview
  user/concepts
  user/tutorial

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
  :caption: Camera Plugin developer guide

  plugin/index

.. toctree::
  :maxdepth: 1
  :caption: Reference Documentation

  cpp_api
  python_api
  
.. toctree::
  :maxdepth: 2
  :caption: How to contribute

  howto_contribute

.. _pdf: http://readthedocs.org/projects/lima-doc/downloads/pdf/latest/
.. _epub: http://readthedocs.org/projects/lima-doc/downloads/epub/latest/
.. _release notes: ./ReleaseNotes.txt

.. _Python: http://python.org
.. _PyTango: http://github.com/tango-cs/pytango
