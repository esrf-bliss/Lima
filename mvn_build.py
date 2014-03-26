#!/usr/bin/python
# -*- coding: iso-8859-15 -*-
#
# Lima compilation script
#
# Version 1.0
#
#
# Author: FL / AN (Hacked from S. Poirier)

import os
import sys
import ConfigParser
import shutil
import string

maven_install = "mvn clean install -DenableCheckRelease=false"
maven_clean =	"mvn clean"
current_dir = os.getcwd()

if "linux" in sys.platform: 
	platform = "linux"
	camera_list = ["adsc", "aviex", "basler","marccd","pilatus","prosilica","simulator","xpad"]
if "win32" in sys.platform:
	platform = "win32"
	camera_list = ["andor", "pco","perkinelmer","roperscientific","simulator"]
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
	set_project_dir('third-party/Processlib');clean()
	for cam in camera_list:
		set_project_dir('camera/'+ cam);clean()
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
def build_plugin(plugin,target_path):
  
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
def build_all_camera(target_path):
	for cam in camera_list:
		build_plugin('camera/' + cam, target_path)
	
#------------------------------------------------------------------------------
# Usage
#------------------------------------------------------------------------------
def usage():
  print "Usage: [python] mvn_build.py <target> [<installation_folder>]"
  print "target: all|processlib|lima|cameras|", camera_list, "|device||cleanall"
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
		build_plugin('third-party/Processlib', target_path)
		build_lima_core(target_path)
		build_all_camera(target_path)
		build_device(target_path)
	#### Build processlib
	elif target == 'processlib':
		print 'BUILD ProcessLib\n'
		build_plugin('third-party/Processlib', target_path)
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
		print 'BUILD All ',platform,' Cameras\n'
		build_all_camera(target_path)
	#### Clean all
	elif target =='cleanall':
		clean_all()
	#### Build cam
	else:
		for cam in camera_list:
			if target == cam:
				build_plugin('camera/'+cam, target_path)
				break
		
		usage()
		
  except BuildError, e:
	sys.stderr.write("!!!BUILD FAILED!!!\n")
