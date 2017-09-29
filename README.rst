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

Requirements
------------

documentation/source/requirements.rst


How to build and install
------------------------

documentation/source/build_install.rst


Lima Team contact: lima@esrf.fr

.. |Linux Status| image:: https://travis-ci.org/esrf-bliss/Lima.svg?branch=cmake
                  :target: https://travis-ci.org/esrf-bliss/Lima
                  :alt:
.. |Windows Status| image:: https://ci.appveyor.com/api/projects/status/rk0yqwem1jqxwubu?svg=true 
                  :target: https://ci.appveyor.com/api/projects/status/rk0yqwem1jqxwubu/branch/cmake?svg=true

		     
.. _Python: http://python.org
.. _PyTango: http://github.com/tango-cs/pytango
