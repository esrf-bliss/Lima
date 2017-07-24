#!/usr/bin/env bash
###########################################################################
# This file is part of LImA, a Library for Image Acquisition
#
#  Copyright (C) : 2009-2017
#  European Synchrotron Radiation Facility
#  BP 220, Grenoble 38043
#  FRANCE
# 
#  Contact: lima@esrf.fr
# 
#  This is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
# 
#  This software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
script_path=$(pwd)/scripts
source_path=$(pwd)

#we create build and install directory in user's home

if [ -d "cmake-build/" ]; then
	cd cmake-build
	#We test if the build directory is empty or not
	empty_dir=$(ls -l)
	if [ "$build_dir" != "total 0" ]; then
		rm -rf *
	fi
else
	mkdir cmake-build
	cd cmake-build
fi
cmake_build_path=$(pwd)

cd ..

if [ -d "cmake-install/" ]; then
	cd cmake-install
	#We test if the install directory is empty or not
	empty_dir=$(ls -l)
	if [ "$build_dir" != "total 0" ]; then
		rm -rf *
	fi
else
	mkdir cmake-install
	cd cmake-install
fi
mkdir python
cmake_install_path=$(pwd)


cd $script_path
if [ ! -f "config.txt" ]; then
	cp config.txt_default config.txt
fi

cd $source_path
#Python script getting compile options, return it -DOPTION=1, so in CMakeLists.txt every options need to be at OFF because otherwise it will still compile it.
compileoptions=$(python scripts/config.py $@)
echo $compileoptions
cd $cmake_build_path
#Launching CMake, building unix Makefiles, from the source in git_test/lima
cmake -G"Unix Makefiles" $source_path -DCMAKE_INSTALL_PREFIX="$cmake_install_path" $compileoptions -DPYTHON_SITE_PACKAGES_DIR="$cmake_install_path/python"

#speed of compilation depend on number of processors.
numberpr=$(nproc | bc)
numberpr=$(($numberpr + 1))
make -j$numberpr

#Install libraries and everything in the directory selected by CMAKE_INSTALL_PREFIXE and PYTHON_SITE_PACKAGES_DIR for python modules.
make install
