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

rem We create the install and build directory, if they already exist they are deleted.
set sourcepath=%cd%

cd ..\..
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

cd /D %sourcepath%\scripts

rem we call the main script, with paths as arguments.
call bootstrap.bat %buildpath% %sourcepath% %installpath%
