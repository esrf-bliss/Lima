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
from optparse import OptionParser

configFile = 'config.inc'

def getModuleConfig() :
    try:
        f = file(configFile)
    except IOError:
        print 'You should Read the README_WINDOW First'
        raise
    else:
        os.chdir('sip')
        availableModule = set([x for x in os.listdir('.') if os.path.isdir(x)])
        config = {}
        for line in f:
            if line.startswith('COMPILE') :
                print "----- line", line
                modName,active = line.split('=')
                modName = '_'.join(modName.split('_')[1:])
                modName = modName.lower()
                if modName in availableModule:
                    config[modName] = bool(int(active))
                    print "-------- config", config

        os.chdir('..')
        print "-------- config", config
        return config

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
        print "----------- sys.path.insert"
        sys.path.insert(0,'sip')
        print "----------- import sip\configure.py"
        import configure
        os.chdir('sip')
        print "----------- sip\configure.py -> main"
        configure.main()
        print "----------- sip\configure.py -> main - returned"
        os.chdir('..')
        sys.path.pop(0)
    else:
        print "----------- getModuleConfig()"
        config = getModuleConfig()
        print "----------- compileModule()"
        compileModule(config)
        print "----------- end"
