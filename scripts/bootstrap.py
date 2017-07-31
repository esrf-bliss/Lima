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
import sys, os
import platform
	
def check_options(options_pass):
	classic_options=[]
	script_options=[]
	for arg in options_pass:
		if arg=="--help" or arg=="-h" or arg=="-help" or arg=="-?":
			with open("INSTALL.txt", 'r') as f:
				print f.read()
			return [0],[0]
			break
		if "--prefix=" in arg:
			script_options.append(arg)
		elif arg=="-g" or arg=="--git":
			script_options.append("git")
		elif "--python-packages=" in arg:
			script_options.append(arg)
		else:
			classic_options.append(arg)
	return(script_options,classic_options)

def GitCloneSubmodule(submodules):
	submodules.append("third-party/Processlib")
	try:
		for submodule in submodules:
			if submodule not in not_submodule:
				if submodule in camera_list:
					submodule="camera/"+str(submodule)
				if submodule=="espia":
					submodule="camera/common/"+str(submodule)
				if submodule=="pytango-server":
					submodule="applications/tango/python"
				init_check = os.system("git submodule init " +str(submodule))
				if str(init_check)!="0":
					raise Exception("Couldn't init the following submodule : "+str(submodule))
		os.system("git submodule update")
		checkout_check = os.system("git submodule foreach 'git checkout cmake'")
		if str(checkout_check)!="0":
			raise Exception("Make sure every submodule has a branch cmake.")
	except Exception as inst:
				print inst
				if str(init_check)!="0":
					sys.exit(init_check)
				else:
					sys.exit(checkout_check)

def ConfigOptions(options):
	configFile = 'scripts/config.txt'
	optionName=[]
	config = []
	#del options[0]
	for arg in options:
		if "camera/" in str(arg):
			optionName.append(str.upper(str(arg)[7:]))
		elif "third-party/" in str(arg):
			optionName.append(str.upper(str(arg)[12:]))
		elif arg=="pytango-server":
			optionName.append("PYTANGO_SERVER")
		else:
			#probably test or python options.
			optionName.append(str.upper(str(arg)))
	#return option in config.txt pass as argument and also the ones with "=1" in config.txt
	with open(configFile) as f:
		for line in f:
			line=line[:-1]
			for option in optionName:
				if option in line:
					line=line[:-1]
					line=line+str(1)
			if line.startswith('LIMA'):
				if line[len(line)-1]==str(1):
					config.append("-D"+line)
		config= " ".join([str(cmd) for cmd in config])
		return config
	f.close()

def Install_lima_linux():
	os.chdir(os.getcwd()+"/build")
	try:
		if install_path=="" and install_python_path=="":
			cmake_check = os.system("cmake -G\"Unix Makefiles\" "+source_path+" "+cmake_config)
		else:
			if install_path!="" and install_python_path=="":
				cmake_check = os.system("cmake -G\"Unix Makefiles\" "+source_path+" -DCMAKE_INSTALL_PREFIX="+str(install_path)+" "+cmake_config)
			elif install_path=="" and install_python_path!="":
				cmake_check = os.system("cmake -G\"Unix Makefiles\" "+source_path+" "+cmake_config+" -DPYTHON_SITE_PACKAGES_DIR="+str(install_python_path))
			else:
				cmake_check = os.system("cmake -G\"Unix Makefiles\" "+source_path+" -DCMAKE_INSTALL_PREFIX="+str(install_path)+" "+cmake_config+" -DPYTHON_SITE_PACKAGES_DIR="+str(install_python_path))
		if str(cmake_check)!="0":
			raise Exception("Something is wrong in your CMake environement. Make sure your configuration is good.")

		compilation_check= os.system("cmake --build . --target install")
		if str(compilation_check)!="0":
			raise Exception("CMake couldn't build or install libraries. Contact claustre@esrf.fr for informations.")
	except Exception as inst:
		print inst
		if str(cmake_check)!="0":
			sys.exit(cmake_check)
		else:
			sys.exit(compilation_check)

def Install_lima_windows():
	os.chdir(os.getcwd()+"/build")
	try :
		if platform.machine()=="AMD64":
			if install_path=="" and install_python_path=="":
				cmake_check = os.system("cmake -G\"Visual Studio 9 2008 Win64\" "+source_path+" "+cmake_config)
			else:
				if install_path!="" and install_python_path=="":
					cmake_check = os.system("cmake -G\"Visual Studio 9 2008 Win64\" "+source_path+" -DCMAKE_INSTALL_PREFIX="+str(install_path)+" "+cmake_config)
				elif install_path=="" and install_python_path!="":
					cmake_check = os.system("cmake -G\"Visual Studio 9 2008 Win64\" "+source_path+" "+cmake_config+" -DPYTHON_SITE_PACKAGES_DIR="+str(install_python_path))
				else:
					cmake_check = os.system("cmake -G\"Visual Studio 9 2008 Win64\" "+source_path+" -DCMAKE_INSTALL_PREFIX="+str(install_path)+" "+cmake_config+" -DPYTHON_SITE_PACKAGES_DIR="+str(install_python_path))
		else:  #platform.machine()=="x86": should be 32bits.
			if install_path=="" and install_python_path=="":
				cmake_check = os.system("cmake -G\"Visual Studio 9 2008\" "+source_path+" "+cmake_config)
			else:
				if install_path!="" and install_python_path=="":
					cmake_check = os.system("cmake -G\"Visual Studio 9 2008\" "+source_path+" -DCMAKE_INSTALL_PREFIX="+str(install_path)+" "+cmake_config)
				elif install_path=="" and install_python_path!="":
					cmake_check = os.system("cmake -G\"Visual Studio 9 2008\" "+source_path+" "+cmake_config+" -DPYTHON_SITE_PACKAGES_DIR="+str(install_python_path))
				else:
					cmake_check = os.system("cmake -G\"Visual Studio 9 2008\" "+source_path+" -DCMAKE_INSTALL_PREFIX="+str(install_path)+" "+cmake_config+" -DPYTHON_SITE_PACKAGES_DIR="+str(install_python_path))
		if str(cmake_check)!="0":
			raise Exception("Something went wrong in the CMake preparation. Make sure your configuration is good.")

		compilation_check = os.system("cmake --build . --target install --config Release")
		if str(compilation_check)!="0":
			raise Exception("CMake couldn't build or install libraries. Contact claustre@esrf.fr for informations.")
	except Exception as inst:
		print inst


if __name__ == '__main__':
	OS_TYPE=platform.system()
	del sys.argv[0]
	not_submodule=('git', 'python', 'tests', 'test', 'cbf', 'lz4', 'fits', 'gz', 'tiff', 'hdf5')
	camera_list=('adsc', 'andor3', 'basler', 'dexela', 'frelon', 'hexitec', 'marccd', 'merlin', 'mythen3', 'perkinelmer', 'pilatus', 'pointgrey', 'rayonixhs', 'ultra', 'xh', 'xspress3', 'andor', 'aviex', 'eiger', 'hamamatsu', 'imxpad', 'maxipix', 'mythen', 'pco', 'photonicscience','pixirad', 'prosilica', 'roperscientific', 'ueye', 'v4l2', 'xpad')
	install_path=""
	install_python_path=""
	print "OS TYPE : ",OS_TYPE
	source_path=os.getcwd()

	available_options,options = check_options(sys.argv)

	if available_options==0 and options==0:
		exit
	#No git option under windows for obvious reasons.
	if OS_TYPE=="Linux":
		if "git" in available_options:
                        GitCloneSubmodule(options)

	cmake_config = ConfigOptions(options)
	print cmake_config
	for script_option in available_options:
		if "--prefix=" in script_option:
			install_path=script_option[9:]
		if "--python-packages=" in script_option:
			install_python_path=script_option[18:]
	if OS_TYPE=="Linux":
		Install_lima_linux()
		
	elif OS_TYPE=="Windows":
		Install_lima_windows()

	
