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
import os,sys
import os.path as path
from optparse import OptionParser

configFile = 'config.inc'


def getModuleConfig():
    availableModules = set()
    for dir_ in ('camera', 'sip'):
        availableModules.update(
            set([subdir for subdir in os.listdir(dir_)
                 if os.path.isdir(path.join(dir_, subdir))])
        )
    config = {}
    try:
        with open(configFile) as f:
            for line in f:
                if line.startswith('COMPILE'):
                    modName, active = line.split('=')
                    modName = '_'.join(modName.split('_')[1:])
                    modName = modName.lower()
                    if modName in availableModules:
                        config[modName] = bool(int(active))
        return config
    except IOError:
        print 'You should Read the README_WINDOW First'
        raise

def compileModule(config) :
    os.chdir('sip')
    for module,active in config.iteritems():
        if active:
            os.chdir(module)
            os.system('nmake')
            os.chdir('..')
    os.chdir('..')
    
if __name__ == '__main__':
    parser = OptionParser()
    parser.add_option("--config",
                      action="store_true",dest="config_flag",default=False,
                      help = 'Configure sip modules')
    (options,args) = parser.parse_args()
    if options.config_flag:
        sys.path.insert(0,'sip')
        import configure
        os.chdir('sip')
        configure.main()
        os.chdir('..')
        sys.path.pop(0)
    else:
        config = getModuleConfig()
        compileModule(config)
