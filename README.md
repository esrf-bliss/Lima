# LImA

[![License](https://img.shields.io/github/license/esrf-bliss/lima.svg?style=flat)](https://opensource.org/licenses/GPL-3.0)
[![Gitter](https://img.shields.io/gitter/room/esrf-bliss/lima.svg?style=flat)](https://gitter.im/esrf-bliss/LImA)
[![Conda](https://img.shields.io/conda/dn/esrf-bcu/lima-core.svg?style=flat)](https://anaconda.org/esrf-bcu)
[![Version](https://img.shields.io/conda/vn/esrf-bcu/lima-core.svg?style=flat)](https://anaconda.org/esrf-bcu)
[![Platform](https://img.shields.io/conda/pn/esrf-bcu/lima-core.svg?style=flat)](https://anaconda.org/esrf-bcu)

Lima ( **L** ibrary for **Im** age **A** cquisition) is a project for the unified control of 2D detectors. The aim is to clearly separate hardware specific code from common software configuration and features, like setting standard acquisition parameters (exposure time, external trigger), file saving and image processing.

Lima is a C++ library which can be used with many different cameras. The library also comes with a [Python](http://python.org) binding and provides a [PyTango](http://pytango.readthedocs.io/en/stable/) device server for remote control.

## Documentation

The documentation is available [here](http://lima.blissgarden.org). Thank you to the people running Read the Docs for such an excellent service.

The source for the documentation is in the `docs` folder. Here are the instructions to built and read it locally. The documentation is built with [Doxygen](http://www.doxygen.org/) and [Sphinx](http://www.sphinx-doc.org). The sphinx template is from [ReadtheDocs](https://docs.readthedocs.io). [Breathe](https://breathe.readthedocs.io) provides a bridge between the Sphinx and Doxygen documentation systems.

    pip install --user sphinx sphinx-autobuild sphinx_rtd_theme breathe recommonmark
    apt-get doxygen

    cd docs  
    doxygen common.dox
    doxygen control.dox
    doxygen hardware.dox
    make html

The html documentation is generated in `docs/.build/html`.
