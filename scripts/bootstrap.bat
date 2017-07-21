rem ##########################################################################
rem  This file is part of LImA, a Library for Image Acquisition
rem
rem   Copyright (C) : 2009-2017
rem   European Synchrotron Radiation Facility
rem   BP 220, Grenoble 38043
rem   FRANCE
rem  
rem   Contact: lima@esrf.fr
rem  
rem   This is free software; you can redistribute it and/or modify
rem   it under the terms of the GNU General Public License as published by
rem   the Free Software Foundation; either version 3 of the License, or
rem   (at your option) any later version.
rem 
rem   This software is distributed in the hope that it will be useful,
rem   but WITHOUT ANY WARRANTY; without even the implied warranty of
rem   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem   GNU General Public License for more details.
rem 
rem   You should have received a copy of the GNU General Public License
rem   along with this program; if not, see <http://www.gnu.org/licenses/>.
rem ###########################################################################
rem @echo off
rem setlocal enabledelayedexpansion

rem We create the install and build directory, if they already exist they are deleted.

set sourcepath=%cd%

if exist "cmake-build" (
    rmdir /S /Q cmake-build
)
mkdir cmake-build
cd cmake-build
set buildpath=%cd%

cd ..
if exist "cmake-install" (
    rmdir /S /Q cmake-install
)
mkdir cmake-install
cd cmake-install
mkdir python
set installpath=%cd%

cd /D %sourcepath%

rem we create the config.txt which will be used by python script. The one users will modify as they want.
if not exist "scripts/config.txt" (
    copy /y scripts/config.txt_default scripts/config.txt
)

rem deleting the output of python script if it exists
if exist "scripts/output_config" (
	del "scripts/output_config"
)
pause

call python scripts/config.py %*> scripts/output_config
set /p cmake_configs=<scripts/output_config

if exist "scripts/output_config" (
	del "scripts/output_config"
)

rem we go in the build directory
cd /D %buildpath%
rem calling cmake with arguments we need. Need to check for 64 or 32bits windows version. To do so just check if ProgramFiles(x86) exists.
 if defined ProgramFiles(x86) (
 	cmake -G "Visual Studio 9 2008 Win64" -DCMAKE_INSTALL_PREFIX="%installpath%" %cmake_configs% -DPYTHON_SITE_PACKAGES_DIR="%installpath%\python" "%~2"
) else (
 	cmake -G "Visual Studio 9 2008" -DCMAKE_INSTALL_PREFIX="%~3" %cmake_configs% -DPYTHON_SITE_PACKAGES_DIR="%installpath%\python" "%sourcepath%"
)

rem configuration of env variables for visual c++ 2008 version.
cd /D %VS90COMNTOOLS%..\..\VC
call vcvarsall.bat

rem compilation and install using cmake.
cd /D %buildpath%
cmake --build . --target install --config Release

cd ..
rem the following code might be use one day.
rem cd %~2

rem if exist "python_path.tmp" (
rem     del python_path.tmp
rem )

rem call python python_path.py
rem set /p python_path= < python_path.tmp
rem call python windowsInstall.py --install_dir=%python_path%

rem if exist "python_path.tmp" (
rem     del python_path.tmp
rem )
