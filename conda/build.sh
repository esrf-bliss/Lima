#!/bin/bash
#./install.sh --install-prefix=$PREFIX --install-python-prefix=$SP_DIR --find-root-path=$PREFIX hdf5 hdf5-bs edfgz edflz4 cbf tiff python pytango-server basler pilatus v4l2 espia maxipix frelon andor andor3 prosilica

cmake -Bbuild -H. -DLIMA_BUILD_SUBMODULES=0 -DLIMA_ENABLE_PYTHON=1 -DLIMA_ENABLE_TESTS=1 -DLIMA_ENABLE_CONFIG=1 -DLIMA_ENABLE_TIFF=1 -DLIMA_ENABLE_HDF5=1 -DLIMA_ENABLE_HDF5_BS=1 -DLIMA_ENABLE_EDFGZ=1 -DLIMA_ENABLE_EDFLZ4=1 -DCMAKE_INSTALL_PREFIX=$PREFIX -DPYTHON_SITE_PACKAGES_DIR=$SP_DIR -DCMAKE_FIND_ROOT_PATH=$PREFIX
cmake --build build --target install
