#!/bin/bash
python -V
./install.sh --install-prefix=$PREFIX --install-python-prefix=$SP_DIR --find-root-path=$PREFIX hdf5 hdf5-bs edfgz edflz4 cbf tiff python pytango-server basler pilatus v4l2 espia maxipix frelon andor andor3 prosilica

# Conda requires the library to be installed in /lib not /lib64 regardeless of the HFS standard.
mv -f $PREFIX/lib64/* $PREFIX/lib
rmdir $PREFIX/lib64
