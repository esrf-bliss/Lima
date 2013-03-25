import os,shutil
import platform
from windowsSipCompilation import getModuleConfig
from optparse import OptionParser

global module2Installfiles
module2Installfiles = {}
if platform.machine() == 'AMD64':
    module2Installfiles = {
	'core' : [('__init__.py','Lima'),
		  ('common/python/Core.py','Lima'),
		  ('common/python/Debug.py','Lima'),
		  ('build/msvc/9.0/LimaCore/x64/Release/LibLimaCore.dll','Lima'),
		  ('third-party/libconfig/lib/x64/Release/libconfig++.dll','Lima'),
		  ('sip/core/limacore.pyd','Lima'),
		  ('third-party/Processlib/sip/processlib.pyd','Lima'),
		  ('third-party/Processlib/build/msvc/9.0/libprocesslib/x64/Release/libprocesslib.dll','Lima')],
	'dexela' : [('camera/dexela/python/Dexela.py','Lima'),
		    ('camera/dexela/src/DexelaConfig.cfg','Lima'),
		    ('camera/dexela/build/msvc/9.0/LibDexela/x64/Release/liblimadexela.dll','Lima'),
		    ('sip/dexela/limadexela.pyd','Lima')],
	}
else:
    module2Installfiles = {
        'core' : [('__init__.py','Lima'),
                  ('common/python/Core.py','Lima'),
                  ('common/python/Debug.py','Lima'),
                  ('build/msvc/9.0/LimaCore/Release/LibLimaCore.dll','Lima'),
                  ('third-party\libconfig\lib\libconfig++.Release\libconfig++.dll','Lima'),
                  ('sip/core/limacore.pyd','Lima'),
                  ('third-party/Processlib/sip/processlib.pyd','Lima'),
                  ('third-party/Processlib/build/msvc/9.0/libprocesslib/Release/libprocesslib.dll','Lima')],
        'pco' : [('camera/pco/python/Pco.py','Lima'),
                 ('camera/pco/build/msvc/9.0/liblimapco/Release/liblimapco.dll','Lima'),
                 ('camera/pco/sdkPco/bin/SC2_Cam.dll','Lima'),
                 ('camera/pco/sdkPco/bin/sc2_cl_me4.dll','Lima'),
                 ('applications/tango/camera/Pco.py','camera'),
                 ('sip/pco/limapco.pyd','Lima')],
        'perkinelmer' : [('camera/perkinelmer/python/PerkinElmer.py','Lima'),
                         ('camera/perkinelmer/build/msvc/9.0/LibPerkinElmer/Release/liblimaperkinelmer.dll','Lima'),
                         ('sip/perkinelmer/limaperkinelmer.pyd','Lima')],
        'photonicscience' : [('camera/photonicscience/python/PhotonicScience.py','Lima'),
                             ('camera/photonicscience/build/msvc/9.0/LibPhotonicScience/Release/liblimaphotonicscience.dll','Lima'),
                             ('camera/photonicscience/sdk/ImageStar4022_v1.7',''),
                             ('sip/photonicscience/limaphotonicscience.pyd','Lima')],
        'simulator' : [('camera/simulator/python/Simulator.py','Lima'),
                       ('camera/simulator/build/msvc/9.0/LibSimulator/Release/liblimasimulator.dll','Lima'),
                       ('sip/simulator/limasimulator.pyd','Lima')],
        'basler' : [('camera/basler/python/Basler.py','Lima'),
                    ('camera/basler/build/msvc/9.0/LibBasler/Release/liblimabasler.dll','Lima'),
                    ('sip/basler/limabasler.pyd','Lima')],
	}
#Add Src 
module2Installfiles.update({
    'tango-core' : [('applications/tango/LimaCCDs.py',''),
		    ('applications/tango/AttrHelper.py',''),
                    ('applications/tango/EnvHelper.py',''),
                    ('applications/tango/camera/__init__.py','camera'),
		    ('applications/tango/plugins','')],
    'tango-simulator' : [('applications/tango/camera/Simulator.py','camera')],
    'tango-perkinelmer' : [('applications/tango/camera/PerkinElmer.py','camera')],
    }
			   )

def copyModule(filesList,baseDestPath) :
    for src,dest in filesList:
        dst = os.path.join(baseDestPath,dest)
            
        if not os.access(dst,os.F_OK) :
            print 'Makedir',dst
            os.makedirs(dst)

        if os.path.isdir(src):
            base,srcDir = os.path.split(src)
            dst = os.path.join(dst,srcDir)
            if os.access(dst,os.F_OK) :
                shutil.rmtree(dst)
            shutil.copytree(src,dst)
            print 'Copytree',src,dst
        elif not os.access(src,os.F_OK) :
            base,srcDir = os.path.split(src)
            dstFile = os.path.join(dst,srcDir)
            f = file(dstFile,'w')
            f.close()
            print 'Create empty file',dstFile
        else:
            shutil.copy(src,dst)
            print 'Copy',src,dst

if __name__ == '__main__':
    parser = OptionParser()
    parser.add_option("--install_dir",dest="install_dir",default='install',
                      help='install directory path')
    parser.add_option("--add",dest="module_list",action="append",default=[],
                      help="Add a module to install")
    parser.add_option("--set",dest="module_set_list",action="append",
                      help="Set the module list to install, do not read config.inc file")
    parser.add_option("--available-modules",dest="get_module_list_flag",action="store_true",default=False,
                      help="List all possible modules to install")
    parser.add_option("--all",action="store_true",dest="install_all",default=False,
                      help="install all possible module")
    (option,args) = parser.parse_args()

    destPath = option.install_dir
    
    if option.get_module_list_flag:
        print 'Available Modules:'
        for moduleName in module2Installfiles.keys() :
            print '\t ->',moduleName
    elif option.module_set_list:
        for moduleName in option.module_set_list:
            filesList = module2Installfiles.get(moduleName,None)
            if filesList:
                copyModule(filesList,destPath)
            else:
                print "module : %s doesn't exist" % moduleName
    else:
        if option.install_all:
            module_list = list(module2Installfiles.keys())
        else:
            dict = getModuleConfig()
            module_list = []
            for key,value in dict.iteritems():
                if value:
                    module_list.append(key)
            module_list += option.module_list
            module_list += ["core"]
            module_list += ["tango-core"]

        for moduleName in set(module_list) :
            filesList = module2Installfiles.get(moduleName,None)
            if filesList:
                copyModule(filesList,destPath)
            else:
                print "module : %s doesn't exist" % moduleName
    
                      
