LIMA
=======
Lima ( **L** ibrary for **Im** age **A** cquisition) is a project for the unified control of 2D detectors. The aim is to clearly separate hardware specific code from common software configuration and features, like setting standard acquisition parameters (exposure time, external trigger), file saving and image processing.


For a full documentation please refer to the online documentation at:

      http://lima.blissgarden.org

How to build and install
------------------------
Using scripts
``````````````
Linux:
[sudo] install.sh
Windows:
 install.bat
 [--prefix=<desired installation path>]
 [--python-path=<desired python installation path>]
 [options]

options: <camera-name> | <saving-format> | python
camera-name: andor|andor3|basler|prosilica|adsc|mythen3|ueye|xh|xspress3|ultra|xpad|mythen|pco|marccd|pointgrey|imxpad|dexela|merlin|v4l2|eiger|pixirad|hexitec|aviex|roperscientific|rayonixhs|espia|maxipix|frelon
saving-format:cbf|nxs|fits|edfgz|edflz4|tiff|hdf5
python: to compile python module

Example:
 sudo install.sh --prefix=./install --python-path=./install basler python tiff

Using cmake
`````````````
 git submodule init third-party/Processlib
 git submodule init camera/basler
 git submodule update

 mkdir build;
 cd build;
 cmake ..;
 [-DCMAKE_INSTALL_PREFIX=<desired installation path>]
 [-DLIMA_ENABLE_PYTHON=true]
 [-DPYTHON_SITE_PACKAGES_DIR=<desired python installation path>]
 -DLIMACAMERA_BASLER=true
 -DLIMA_ENABLE_TIFF=true
 make;
 [sudo] make install


Lima Team
mailto: lima@esrf.fr

