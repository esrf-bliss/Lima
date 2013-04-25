@echo off
setlocal enabledelayedexpansion

rem repertoire courant du batch 
set CurrentPath=%cd%

rem configuration des variables d'environnement visual c++ 2008 express edition
cd /D %VS90COMNTOOLS%..\..\VC
call vcvarsall.bat

rem compilation de la librairie libprocesslib
cd /D %CurrentPath%\third-party\Processlib\build\msvc\9.0\libprocesslib
msbuild.exe libprocesslib.sln /t:build /fl /flp:logfile=MyProjectOutput.log /p:Configuration=Release;Plateform=Win32 /v:d

rem compilation de la librairie limacore
cd /D %CurrentPath%\build\msvc\9.0\LimaCore
msbuild.exe LimaCore.sln /t:build /fl /flp:logfile=MyProjectOutput.log /p:Configuration=Release;Plateform=Win32 /v:d

rem compilation des plugins des cameras actives dans le fichier config.inc
for /f "delims=:" %%i in ('type %CurrentPath%\config.inc') do (
	set ligne=%%i
	set ligne_temp=%%i
	if "!ligne:~0,7!" == "COMPILE" (
		rem call:strlen longueur !ligne! 
		rem echo longueur : !longueur!
		if "!ligne:~-1!" == "1" (
			if not "!ligne:~8,-2!" == "CORE" (
				rem compilation du plugin active
				cd /D !CurrentPath!\camera\!ligne:~8,-2!\build\msvc\9.0\lib!ligne:~8,-2!
				msbuild.exe Lib!ligne:~8,-2!.sln /t:build /fl /flp:logfile=MyProjectOutput.log /p:Configuration=Release;Plateform=Win32 /v:d
			)
		)
	)
)

rem execution du script python configure.py
cd /D %CurrentPath%\third-party\Processlib\sip
call python configure.py

rem execution du script python windowsSipCompilation --config
cd /D %CurrentPath%
call python windowsSipCompilation.py --config

cd /D %CurrentPath%\third-party\Processlib\sip
call nmake

rem execution du script python windowsSipCompilation
cd /D %CurrentPath%
call python windowsSipCompilation.py

rem execution du script python windowsInstall
cd /D %CurrentPath%

if exist "python_path.tmp" (
    del python_path.tmp
)

call python_path.py
set /p python_path= < python_path.tmp
call python windowsInstall.py --install_dir=%python_path%

if exist "python_path.tmp" (
    del python_path.tmp
)

cd /D %CurrentPath%
