#!/usr/bin/python
# -*- coding: iso-8859-15 -*-
#
# Lima compilation script
#
# Version 1.0
#
#
# Hacked from S. Poirier

import os
import sys
import ConfigParser
import shutil
import string

maven_install = "mvn clean install -DenableCheckRelease=false"
maven_clean =	"mvn clean"
current_dir = os.getcwd()

if "linux" in sys.platform: platform = "linux"
if "win32" in sys.platform: platform = "win32"
print "platform : ", platform

#------------------------------------------------------------------------------
# build exception
#------------------------------------------------------------------------------
class BuildError(Exception):
  pass

#------------------------------------------------------------------------------
# Project directory
#------------------------------------------------------------------------------
def set_project_dir(sub_dir):
  project_dir = os.path.join(current_dir, sub_dir)
  os.chdir(project_dir)
  
#------------------------------------------------------------------------------
# Copy library
#------------------------------------------------------------------------------
def copy_file_ext(from_path, to_path, file_ext):
  
  if not os.path.isdir(to_path):
	os.makedirs(to_path)
	
  files = os.listdir(from_path)
  for file in files:
	full_name = os.path.join(from_path, file)
	if os.path.isfile(full_name):
	  root, ext = os.path.splitext(full_name)
	  if ext == file_ext:
		shutil.copy(full_name, to_path)
		print '\n' + file + ' copied in ' + to_path

#------------------------------------------------------------------------------
# Compilation
#------------------------------------------------------------------------------
def build():
  rc = os.system(maven_install)
  if rc != 0:
	raise BuildError

#------------------------------------------------------------------------------
# Clean
#------------------------------------------------------------------------------
def clean():
  rc = os.system(maven_clean)
  if rc != 0:
	raise BuildError

#------------------------------------------------------------------------------
# Clean all modules
#------------------------------------------------------------------------------
def clean_all():
  set_project_dir('.');clean()
  set_project_dir('camera/adsc');clean()
  set_project_dir('camera/aviex');clean()
  set_project_dir('camera/basler');clean()
  set_project_dir('camera/marccd');clean()
  set_project_dir('camera/pilatus');clean()
  set_project_dir('camera/prosilica');clean()
  set_project_dir('camera/simulator');clean()
  set_project_dir('camera/xpad');clean()
  set_project_dir('applications/tango/LimaDetector');clean()

	

#------------------------------------------------------------------------------
# build the LimaDetector device
#------------------------------------------------------------------------------
def build_device(target_path):
  print 'Build Device LimaDetector\n'
  set_project_dir('applications/tango/LimaDetector')
  build()
  
  if target_path is not None:
	dest_path = os.path.join(target_path, '')

	if platform == "linux":
		src_path = './target/nar/bin/i386-Linux-g++/'
	elif platform == "win32":
		src_path = './target/nar/bin/x86-Windows-msvc/'

	copy_file_ext(src_path, dest_path, '')
	
  print '\n'

#------------------------------------------------------------------------------
# build the Lima Core
#------------------------------------------------------------------------------
def build_lima_core(target_path):
  print 'Build Lima\n'
  set_project_dir('.')
  build()
  
  if target_path is not None:
	dest_path = os.path.join(target_path, '')

	if platform == "linux":
		src_path = './target/nar/lib/i386-Linux-g++/shared/'
	elif platform == "win32":
		src_path = './target/nar/lib/x86-Windows-msvc/shared/'

	copy_file_ext(src_path, dest_path, '.so')
	
  print '\n'

#------------------------------------------------------------------------------
# build the Plugin 'plugin'
#------------------------------------------------------------------------------
def build_plugins(plugin,target_path):
  
  """Build the selected plugin"""
  
  print "Building:	" , plugin, "\n" 
  set_project_dir(plugin)
  build()
  
  if target_path is not None:
	dest_path = os.path.join(target_path, '')

	if platform == "linux":
		src_path = './target/nar/lib/i386-Linux-g++/shared/'
	elif platform == "win32":
		src_path = './target/nar/lib/x86-Windows-msvc/shared/'

	copy_file_ext(src_path, dest_path, '.so')
	
  print '\n'


#------------------------------------------------------------------------------
# build all linux cameras
#------------------------------------------------------------------------------
def build_linux_plugins(target_path):
	build_plugins('camera/adsc', target_path)
	build_plugins('camera/aviex', target_path)
	build_plugins('camera/basler', target_path)
	build_plugins('camera/marccd', target_path)
	build_plugins('camera/pilatus', target_path)
	build_plugins('camera/prosilica', target_path)
	build_plugins('camera/simulator', target_path)
	build_plugins('camera/xpad', target_path)

#------------------------------------------------------------------------------
# build all win32 cameras
#------------------------------------------------------------------------------
def build_win32_plugins(target_path):
	build_plugins('camera/pco', target_path)
	build_plugins('camera/perkinelmer', target_path)
	build_plugins('camera/roperscientific', target_path)
	build_plugins('camera/simulator', target_path)

#------------------------------------------------------------------------------
# Usage
#------------------------------------------------------------------------------
def usage():
  print "Usage: [python] mvn_build.py <target> [<installation_folder>]"
  print "target: all|processlib|lima|cameras|a specific camera name|device||cleanall"
  sys.exit(1)

#------------------------------------------------------------------------------
# Main Entry point
#------------------------------------------------------------------------------
if __name__ == "__main__":
  
  if len(sys.argv) < 2:
	usage()
  
  target = sys.argv[1]

  target_path = None
  
  if len(sys.argv) == 3:
	target_path = sys.argv[2]

  try:
	#### Build all
	if target == 'all':	 
		print 'BUILD ALL\n'
		build_plugins('third-party/Processlib', target_path)
		build_lima_core(target_path)
		if platform == "linux":
			build_linux_plugins(target_path)
		elif platform == "win32":
			build_win32_plugins(target_path)
		build_device(target_path)
	#### Build processlib
	elif target == 'processlib':
		print 'BUILD ProcessLib\n'
		build_plugins('third-party/Processlib', target_path)
	#### Build device
	elif target == 'device':
		print 'BUILD Device\n'
		build_device(target_path)
	#### Build lima
	elif target == 'lima':
		print 'BUILD Lima Core\n'
		build_lima_core(target_path)
	#### Build cameras
	elif target == 'cameras':
		if platform == "linux":
			print 'BUILD All Linux Cameras\n'
			build_linux_plugins(target_path)
		elif platform == "win32":
			print 'BUILD All Win32 Cameras\n'
			build_win32_plugins(target_path)
	#### Build adsc
	elif target =='adsc':
		build_plugins('camera/adsc', target_path)
	#### Build aviex
	elif target =='aviex':
		build_plugins('camera/aviex', target_path)
	#### Build basler
	elif target =='basler':
		build_plugins('camera/basler', target_path)	
	#### Build marccd
	elif target =='marccd':
		build_plugins('camera/marccd', target_path)	
	#### Build pilatus
	elif target =='pilatus':
		build_plugins('camera/pilatus', target_path)
	#### Build prosilica
	elif target =='prosilica':
		build_plugins('camera/prosilica', target_path)
	#### Build xpad
	elif target =='xpad':
		build_plugins('camera/xpad', target_path)
	#### Build pco
	elif target =='pco':
		build_plugins('camera/pco', target_path)
	#### Build perkinelmer
	elif target =='perkinelmer':
		build_plugins('camera/perkinelmer', target_path)
	#### Build roperscientific
	elif target =='roperscientific':
		build_plugins('camera/roperscientific', target_path)
	#### Build simulator
	elif target =='simulator':
		build_plugins('camera/simulator', target_path)
	#### Clean all
	elif target =='cleanall':
		clean_all()
	#### Print Help usage Message
	else:
	  usage()
  except BuildError, e:
	sys.stderr.write("!!!BUILD FAILED!!!\n")
