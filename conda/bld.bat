REM ./install.bat --install-prefix=%LIBRARY_PREFIX% --install-python-prefix=%SP_DIR% --find-root-path=%PREFIX% --build-type=Release hdf5 hdf5-bs edfgz edflz4 tiff python pytango-server pco

cmake -Bbuild -H. -G "%CMAKE_GENERATOR%" -DLIMA_BUILD_SUBMODULES=0 -DLIMA_ENABLE_PYTHON=1 -DLIMA_ENABLE_TESTS=1 -DLIMA_ENABLE_CONFIG=1 -DLIMA_ENABLE_TIFF=1 -DLIMA_ENABLE_HDF5=1 -DLIMA_ENABLE_HDF5_BS=1 -DLIMA_ENABLE_EDFGZ=1 -DLIMA_ENABLE_EDFLZ4=1 -DCMAKE_INSTALL_PREFIX=%LIBRARY_PREFIX% -DPYTHON_SITE_PACKAGES_DIR=%SP_DIR% -DCMAKE_FIND_ROOT_PATH=%LIBRARY_PREFIX%
cmake --build build --config Release --target install
