@echo off
setlocal enabledelayedexpansion

rem variables d'environnement pour CMake
call python set_win_cmake_env.py

rem repertoire courant du batch 
if exist "_build" (
    rmdir /S /Q _build
)
mkdir _build
cd _build
set CurrentPath=%cd%

rem execution de cmake pour créer les fichiers projets et solution
cmake -G "Visual Studio 9 2008" -D PYLON_ROOT="C:\Program Files\Basler\pylon 3.2\pylon" -D GSL_INCLUDE_DIR="C:\Program Files (x86)\GnuWin32\include" -D GSL_LIB_DIR="C:\Program Files (x86)\GnuWin32\lib" -D NUMPY_INCLUDE_DIR="C:\Anaconda2\Lib\site-packages\numpy\core\include" -D NUMPY_LIB_DIR="C:\Anaconda2\Lib\site-packages\numpy\core\lib" -D COMPILE_SIP=1 ..

rem configuration des variables d'environnement visual c++ 2008 express edition
cd /D %VS90COMNTOOLS%..\..\VC
call vcvarsall.bat

rem compilation
cd /D %CurrentPath%
msbuild.exe lima.sln /t:build /fl /flp:logfile=limaOutput.log /p:Configuration=Release;Plateform=Win32 /v:d

rem execution du script python windowsInstall
cd ..

if exist "python_path.tmp" (
    del python_path.tmp
)

call python python_path.py
set /p python_path= < python_path.tmp
call python windowsInstall.py --install_dir=%python_path%

if exist "python_path.tmp" (
    del python_path.tmp
)
