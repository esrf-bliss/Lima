############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2011
# European Synchrotron Radiation Facility
# BP 220, Grenoble 38043
# FRANCE
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
import os
import glob
import sipconfig
import shutil
import numpy
import platform
from checksipexc import checksipexc

modules = [('core',		['common', 'hardware', 'control']),
	   ('simulator',	[os.path.join('camera','simulator')]),
	   ('espia',		[os.path.join('camera','common','espia')]),
	   ('frelon',		[os.path.join('camera','frelon')]),
	   ('maxipix',		[os.path.join('camera','maxipix')]),
           ('basler',           [os.path.join('camera','basler')])]

espiaModules = ['espia', 'frelon', 'maxipix']

rootDir = '..'
def rootName(fn):
    return os.path.join(rootDir, fn)
    
def findIncludes(baseDir):
    inclDirs = []
    for root, dirs, files in os.walk(baseDir):
        for dirname in dirs:
            if dirname == 'include':
                inclDirs.append(os.path.join(root,dirname))
    return inclDirs

def findModuleIncludes(name):
    for modName, modDirs in modules:
        if modName == name:
            modInclDirs = []
            for subDir in modDirs:
                modInclDirs += findIncludes(rootName(subDir))
            return modInclDirs
    return None

def main():
    excludeMods = set()

    config = sipconfig.Configuration()

    confFile = open(rootName('config.inc'))
    for line in confFile:
	if line.startswith('export') : break
        line = line.strip('\n ')
        if line.startswith('COMPILE_'):
            var, value = line.split('=')
            try:
                value = int(value)
            except ValueError:
                continue
            if not value:
                excludeMods.add(var.split('_')[-1].lower())

    for modName, modDirs in modules:
        extra_cxxflags = []
        if modName in excludeMods:
            continue
    
        if os.path.exists(modName):
            if not os.path.isdir(modName):
                raise 'Error: %s exists and is not a directory' % modName
        else:
            os.mkdir(modName)

        os.chdir('%s' % modName)

        global rootDir
        orig_rootDir = rootDir
        rootDir = os.path.join('..', rootDir)
    
        sipFileNameSrc = "lima%s.sip" % modName
        if modName != 'core':
            sipFileNameIn = os.path.join("..","limamodules.sip.in")
            f = file(sipFileNameIn)
            lines = f.read()
            f.close()
            newlines = lines.replace('%NAME',modName)
            d = file(sipFileNameSrc,'w')
            d.write(newlines)
            d.close()

        sipFileName = "lima%s_tmp.sip" % modName
        shutil.copyfile(sipFileNameSrc, sipFileName)

        initNumpy = 'lima_init_numpy.cpp'
        shutil.copyfile(os.path.join('..',initNumpy), initNumpy)


        dirProcesslib = rootName(os.path.join('third-party','Processlib'))
        sipProcesslib = os.path.join(dirProcesslib,'sip')
        extraIncludes = ['.', os.path.join('..','core'),
                         sipProcesslib, numpy.get_include(),
                         config.sip_inc_dir]

        extraIncludes += findIncludes(dirProcesslib)
        if platform.system() == 'Windows':
            extraIncludes += [os.path.join(dirProcesslib,"core","include","WindowSpecific")]
            
        coreDirs = modules[0][1]
        extraIncludes += findModuleIncludes('core')
    
        if (modName in espiaModules) and ('espia' not in excludeMods):
            espia_base = '/segfs/bliss/source/driver/linux-2.6/espia'
            espia_incl = os.path.join(espia_base,'src')
            extraIncludes += [espia_incl]

        if(modName == 'basler') :
            extraIncludes += ['/opt/pylon/include','/opt/pylon/include/genicam']
            extra_cxxflags += ['-DLESSDEPENDENCY','-DUSE_GIGE']

        extraIncludes += findModuleIncludes(modName)

        sipFile = open(sipFileName,"a")
        sipFile.write('\n')

        sipProcesslib = sipProcesslib.replace(os.sep,'/') # sip don't manage windows path
        sipFile.write('%%Import %s/processlib_tmp.sip\n' % sipProcesslib)
    
        if modName != 'core':
            sipFile.write('%Import ../core/limacore_tmp.sip\n')
        if (modName in espiaModules) and (modName != 'espia'):
            sipFile.write('%Import ../espia/limaespia_tmp.sip\n')
            extraIncludes += findModuleIncludes('espia')

        for sdir in modDirs:
            srcDir = rootName(sdir)
            for root,dirs,files in os.walk(srcDir) :
                dir2rmove = excludeMods.intersection(dirs)
                for dname in dir2rmove:
                    dirs.remove(dname)
        
                for filename in files:
                    base,ext = os.path.splitext(filename)
                    if ext != '.sip':
                        continue
                    incl = os.path.join(root,filename)
                    incl = incl.replace(os.sep,'/') # sip don't manage windows path.
                    sipFile.write('%%Include %s\n' % incl)

        sipFile.close()

        # The name of the SIP build file generated by SIP and used by the build
        # system.
        build_file = "lima%s.sbf" % modName

        # Run SIP to generate the code.
        # module's specification files using the -I flag.
        if platform.system() == 'Windows':
            plat = 'WIN32_PLATFORM'
        else:
            plat = 'POSIX_PLATFORM'
        cmd = " ".join([config.sip_bin,"-g", "-e","-c", '.','-t',plat,
                        "-b", build_file,sipFileName])
        print cmd
        os.system(cmd)

        #little HACK for adding source
        bfile = open(build_file)
        whole_line = ''
        for line in bfile :
            if 'sources' in line :
                begin,end = line.split('=')
                line = '%s = lima_init_numpy.cpp %s' % (begin,end)
            whole_line += line
        bfile.close()
        bfile = open(build_file,'w')
        bfile.write(whole_line)
        bfile.close()

        # We are going to install the SIP specification file for this module
        # and its configuration module.
        installs = []

        installs.append([sipFileNameSrc, os.path.join(config.default_sip_dir,
                                                      "lima")])

        installs.append(["limaconfig.py", config.default_mod_dir])

        # Create the Makefile.  The QtModuleMakefile class provided by the
        # pyqtconfig module takes care of all the extra preprocessor, compiler
        # and linker flags needed by the Qt library.
        makefile = sipconfig.ModuleMakefile(configuration=config,
                                            build_file=build_file,
                                            installs=installs,
                                            export_all = True)
        makefile.extra_include_dirs = extraIncludes
        if platform.system() == 'Windows':
            makefile.extra_libs = ['liblima%s' % modName,'libprocesslib']
            if modName != 'core' :
                makefile.extra_libs += ['liblimacore']
            makefile.extra_cxxflags = ['/EHsc'] + extra_cxxflags
            makefile.extra_lib_dirs = glob.glob(os.path.join(rootName('build'),'msvc','9.0','*','Release'))
        else:
            makefile.extra_libs = ['pthread','lima%s' % modName]
            makefile.extra_cxxflags = ['-pthread', '-g','-DWITH_SPS_IMAGE'] + extra_cxxflags
            makefile.extra_lib_dirs = [rootName('build')]
        makefile.extra_cxxflags.extend(['-I' + x for x in extraIncludes])
        
        # Add the library we are wrapping.  The name doesn't include any 
        # platform specific prefixes or extensions (e.g. the "lib" prefix on 
        # UNIX, or the ".dll" extension on Windows).
        # None (for me)
        
        # Generate the Makefile itself.
        makefile.generate()

        # Now we create the configuration module.  This is done by merging a
        # Python dictionary (whose values are normally determined dynamically)
        # with a (static) template.
        content = {
            # Publish where the SIP specifications for this module will be
            # installed.
            "lima_sip_dir":    config.default_sip_dir
            }

        # This creates the lima<mod>config.py module from the limaconfig.py.in
        # template and the dictionary.
        sipconfig.create_config_module("lima%sconfig.py" % modName,
                                       os.path.join("..","limaconfig.py.in"), content)

        # Fix SIP Exception handling
        for cpp_file in glob.glob('siplima*.cpp'):
            modify = checksipexc(cpp_file)
            cpp_file_out = '%s.out' % cpp_file
            if not modify:
                os.remove(cpp_file_out)
            else:
                os.remove(cpp_file)
                shutil.move(cpp_file_out,cpp_file)
            
        os.chdir('..')
        rootDir = orig_rootDir


if __name__ == '__main__':
    main()
