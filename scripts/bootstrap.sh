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
help() {
cat <<EOTEXT
Description: 
	This script will build and install lima project, with arguments pass as explained. 
	the install directory can be change by adding as argument --prefix=path_you_want.
	Same for the python install directory, add --python-packages=path_you_want.
	
	USING scripts/config.txt FILE :
		once you have executed a first ./install.sh a config.txt file is created in scripts/ directory.
		If you plan to compile with the same parameters a lot of time you can edit this file and select your options.
		Then you just have to run ./install.sh and lima will compile with what you asked in config.txt. 
		By default camera simulator is always compile. You can change that with editing config.txt file : "LIMACAMERA_SIMULATOR=0" instead of "LIMACAMERA_SIMULATOR=1"
	
execution : 
	"./install.sh [--prefix=path] [--python-packages=path] [options]"
			
	[options] : can be any camera name or saving format.
	saving format available : cbf, tiff, lz4, gz, hdf5, fits.
	other otions are : _ python : Build python wrapping.
	                   _ tests : compile unitest in build directory.
	                   _ config, sps, gldisplay : for the fun !
			                   
examples : 
	./install.sh basler python cbf
		-> compile and install lima with camera basler with python wrapping and cbf saving format.
		-> install directory for C library and python library will be in default directory. 
		
	this is equivalent to change config.txt with this options :
		_ LIMACAMERA_BASLER=1
		_ LIMA_ENABLE_CBF=1
		_ LIMA_ENABLE_PYTHON=1

	./install.sh --prefix=${HOME} tests
		-> compile and install lima only with camera simulator, also compiling simulators tests.
		-> the install directory is set in the home directory (${HOME})
		install directory can't be put in the config.txt file for the moment.
		
	./install.sh
EOTEXT
}

if [ "$1" == "-h" ] || [ "$1" == "--help" ] || [ "$1" == "-help" ] || [ "$1" == "-?" ]; then
	help
else
	
	script_path=$(pwd)/scripts
	source_path=$(pwd)

	#we create build and install directory in lima/ directory
	if [ -d "build/" ]; then
		cd build/
		#We test if the build directory is empty or not
		empty_dir=$(ls -l)
		if [ "$build_dir" != "total 0" ]; then
			rm -rf *
		fi
	else
		mkdir build/
		cd build/
	fi
	build_path=$(pwd)
	
	for arg in "$@"
	do 
		if [[ "$arg" == --prefix* ]];then
			install_path=$(echo ${arg##*=})
			
		fi
	done
	
	for arg in "$@"
	do 
		if [[ "$arg" == --python-packages* ]];then
			install_python_path=$(echo ${arg##*=})
		fi
	done

	cd $script_path
	if [ ! -f "config.txt" ]; then
		cp config.txt_default config.txt
	fi

	cd $source_path
	#Python script getting compile options, return it -DOPTION=1, so in CMakeLists.txt every options need to be at OFF because otherwise it will still compile it.
	compileoptions=$(python scripts/config.py $@)
	echo $compileoptions
	
	cd $build_path
	#Launching CMake, building unix Makefiles, from the source in /lima.
	if [ -z "$install_path" ] && [ -z "$install_python_path" ]; then
		#the user didn't specified in which directory he wanted to intall C and python library. will use path by default.
		cmake -G"Unix Makefiles" $source_path $compileoptions
	else
		if [ ! -z "$install_path" ] && [ ! -z "$install_python_path" ]; then
			#Specified both directory.
			cmake -G"Unix Makefiles" $source_path -DCMAKE_INSTALL_PREFIX="$install_path" $compileoptions -DPYTHON_SITE_PACKAGES_DIR="$install_python_path"
		else
			if [ -z "$install_path" ] && [ ! -z "$install_python_path" ]; then
				#Only specified the install directory for python library.
				cmake -G"Unix Makefiles" $source_path $compileoptions -DPYTHON_SITE_PACKAGES_DIR="$install_python_path"
			fi
			if [ -z "$install_python_path" ] && [ ! -z "$install_path" ]; then
				#Only specified the install directory for C library.
				cmake -G"Unix Makefiles" $source_path -DCMAKE_INSTALL_PREFIX="$install_path" $compileoptions
			fi
		fi
	fi

	#speed of compilation depend on number of processors.
	numberpr=$(nproc | bc)
	numberpr=$(($numberpr + 1))
	make -j$numberpr

	#Install libraries and everything in the directory selected by CMAKE_INSTALL_PREFIXE and PYTHON_SITE_PACKAGES_DIR for python modules.
	make install
fi

