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
setlocal enabledelayedexpansion

if "%~1"=="--help" ( goto :help )
if "%~1"=="-help" ( goto :help )
if "%~1"=="-h" ( goto :help )
if "%~1"=="-?" ( goto :help )

rem We create the install and build directory, if they already exist they are deleted.
set sourcepath=%cd%
set installpath=
set pythonpath=

if exist "build" (
    rmdir /S /Q build
)
mkdir build
cd build
set buildpath=%cd%

cd ..

for %%x in (%*) do (

	if "%%x"=="--local-install" (
		if exist "install" (
    			rmdir /S /Q install
		)
		mkdir install
		set installpath=%sourcepath%\install
		cd /D %sourcepath%
	)
	
	if "%%x"=="--local-python" (
		if exist "install" (
    			cd install
			mkdir python
			cd python
			set pythonpath=%sourcepath%\install\python
		) else (
			mkdir install
			cd install
			mkdir python
			set pythonpath=%sourcepath%\install\python
		)
		cd /D %sourcepath%
	)
)


cd /D %sourcepath%\scripts

rem we create the config.txt which will be used by python script. The one users will modify as they want.
if not exist config.txt (
    copy /y config.txt_default config.txt
)

rem deleting the output of python script if it exists
if exist output_config (
	del output_config
)

cd /D %sourcepath%

call python scripts/config.py %*> scripts/output_config
set /p cmake_configs=<scripts/output_config

if exist scripts/output_config (
	del scripts/output_config
)

rem we go in the build directory
cd /D %buildpath%
rem calling cmake with arguments we need. Need to check for 64 or 32bits windows version. To do so just check if ProgramFiles(x86) exists.
if defined ProgramFiles(x86) (
	if defined installpath (
		if defined pythonpath (
 			cmake -G "Visual Studio 9 2008 Win64" -DCMAKE_INSTALL_PREFIX="%installpath%" %cmake_configs% -DPYTHON_SITE_PACKAGES_DIR="%pythonpath%" "%sourcepath%"
		) else (
			cmake -G "Visual Studio 9 2008 Win64" -DCMAKE_INSTALL_PREFIX="%installpath%" %cmake_configs% "%sourcepath%"
		)
	) else (
		if defined pythonpath (
			cmake -G "Visual Studio 9 2008 Win64" %cmake_configs% -DPYTHON_SITE_PACKAGES_DIR="%pythonpath%" "%sourcepath%"
		) else (
			cmake -G "Visual Studio 9 2008 Win64" %cmake_configs% "%sourcepath%"
		)
	)
) else (
 	if defined installpath (
		if defined pythonpath (
 			cmake -G "Visual Studio 9 2008" -DCMAKE_INSTALL_PREFIX="%installpath%" %cmake_configs% -DPYTHON_SITE_PACKAGES_DIR="%pythonpath%" "%sourcepath%"
		) else (
			cmake -G "Visual Studio 9 2008" -DCMAKE_INSTALL_PREFIX="%installpath%" %cmake_configs% "%sourcepath%"
		)
	) else (
		if defined pythonpath (
			cmake -G "Visual Studio 9 2008" %cmake_configs% -DPYTHON_SITE_PACKAGES_DIR="%pythonpath%" "%sourcepath%"
		) else (
			cmake -G "Visual Studio 9 2008" %cmake_configs% "%sourcepath%"
		)
	)
)

rem configuration of env variables for visual c++ 2008 version.
cd /D %VS90COMNTOOLS%..\..\VC
call vcvarsall.bat

rem compilation and install using cmake.
cd /D %buildpath%
cmake --build . --target install --config Release

cd ..

goto :EOF


:help
echo Description :
echo  this script build and install lima project, depending on arguments pass.
echo  install directories are set by default in the default cmake directory.
echo  Probably "c:\Program Files\lima" and "python2X/site-packages/"
echo  However you can choose to install C++ lib and python lib in lima/install/ directory.
echo  Just need to add --local-install and --local-python
echo  or just --local-install if you compile without python option.
echo  or only --local-python if you only want python libs in lima/install/ directory.

 
echo  USING scripts/config.txt FILE :
echo	once you have executed a first install.bat a config.txt file is created in scripts/ directory.
echo	If you plan to compile with the same parameters a lot of time you can edit this file and select your options.
echo 	Then you just have to run install.bat and lima will compile with what you asked in config.txt.
echo	By default camera simulator is always compile. You can change that with editing config.txt file :
echo		"LIMACAMERA_SIMULATOR=0" instead of "LIMACAMERA_SIMULATOR=1"


echo execution (with cmd.exe in lima/ directory) : 
echo  "install.bat [options]"


echo  [options] : can be any camera name or saving format.
echo  other otions are : _ python : Build python wrapping.
echo                     _ tests : compile unitest in build directory.
echo                     _ config : for the fun !
 
 
echo examples : 
echo  "install.bat basler python"
echo	- compile and install lima with camera basler with python wrapping
echo	- install directory for C++ library and python library will be in cmake default directories. 
echo	(you can check them in cmake-gui)
 	
echo	this is equivalent to change config.txt with this options :
echo	  _ LIMACAMERA_BASLER=1
echo	  _ LIMA_ENABLE_PYTHON=1

echo  "install.bat --local-install basler"
echo	- compile and install camera basler in the lima/install directory.


goto :EOF


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
