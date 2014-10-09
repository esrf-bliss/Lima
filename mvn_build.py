#!/usr/bin/python
# -*- coding: iso-8859-15 -*-
#
# Lima compilation script
#
# Version 1.0
#
#
# Author: FL / AN (Hacked from S. Poirier)

from multiprocessing import Pool
import os
import sys
import ConfigParser
import shutil
import string
from argparse import ArgumentParser

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
def build_limadetector():
  if platform == "linux":
    if mvn_offline == True:
        rc = os.system("mvn clean install --file pom-linux.xml -o")
    else:
        rc = os.system("mvn clean install --file pom-linux.xml")
    if rc != 0:
      raise BuildError
      
  elif platform == "win32":
    if mvn_offline == True:
        rc = os.system("mvn clean install --file pom-win.xml -o")
    else:
        rc = os.system("mvn clean install --file pom-win.xml")
    if rc != 0:
	  raise BuildError  


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
  build_limadetector()
  
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
    
    # this take 2 min 45 sec on a quad core
    if multi_proc == False:
        for cam in camera_list:
            build_plugin('camera/' + cam, target_path)
    else:
        # this take 1 min 26 sec on a quad core
        pool = Pool()           
        for cam in camera_list:
            print "multi proc: Building:	" , cam, "\n" 
            pool.apply_async(build_plugin,('camera/' + cam, target_path))
        pool.close()
        pool.join()
	
#------------------------------------------------------------------------------
# Main Entry point
#------------------------------------------------------------------------------
if __name__ == "__main__":
  
  if "linux" in sys.platform: 
	platform = "linux"
	camera_list = ["adsc", "aviex", "basler", "eiger", "marccd","pilatus","prosilica","simulator","xpad"]
  if "win32" in sys.platform:
	platform = "win32"
	camera_list = ["andor", "hamamatsu", "pco","perkinelmer","roperscientific","simulator"]
  print "platform : ", platform

  # command line parsing
  parser = ArgumentParser(description="Lima compilation script")
  cams_string = ""
  for cam in camera_list:
    cams_string += cam + "|"
  help_string = "module to compile (possible values are: all|processlib|lima|cameras|"+ cams_string+ "|device||cleanall)"
  parser.add_argument("module", help=help_string)  # positional
  parser.add_argument("-o","--offline", help="mvn will be offline",action="store_true")
  parser.add_argument("-m","--multiproc", help="cameras will be compiled in multiprocessing way",action="store_true")
  parser.add_argument("-d","--directory", help="automatically install Lima binaries into the specified installation directory")
  #parser.add_argument("-q","--quiet", help="mvn will be quiet", action="store_true")
  args = parser.parse_args()
  
  # manage command line option  
  if args.directory:
    target_path = args.directory
  else:
    target_path = None
    
  if args.multiproc:
    multi_proc = True
  else:
    multi_proc = False
    
  if args.offline:
    mvn_offline = True
    maven_install = "mvn clean install -DenableCheckRelease=false -o"
  else:
    maven_install = "mvn clean install -DenableCheckRelease=false"
    mvn_offline = False
    
  # variables
  maven_clean =	"mvn clean"
  current_dir = os.getcwd()

  try:
	# Build all
	if args.module == 'all':	 
		print 'BUILD ALL\n'
		build_plugin('third-party/Processlib', target_path)
		build_lima_core(target_path)
		build_all_camera(target_path)
		build_device(target_path)
	# Build processlib
	elif args.module == 'processlib':
		print 'BUILD ProcessLib\n'
		build_plugin('third-party/Processlib', target_path)
	# Build device
	elif args.module == 'device':
		print 'BUILD Device\n'
		build_device(target_path)
	# Build lima
	elif args.module == 'lima':
		print 'BUILD Lima Core\n'
		build_lima_core(target_path)
	# Build cameras
	elif args.module == 'cameras':
		print 'BUILD All ',platform,' Cameras\n'
		build_all_camera(target_path)
	# Clean all
	elif args.module =='cleanall':
		clean_all()
	# Build cam
	else:
		for cam in camera_list:
			if args.module == cam:
				build_plugin('camera/'+cam, target_path)
				break
		
  except BuildError, e:
	sys.stderr.write("!!!   BUILD FAILED    !!!\n")
