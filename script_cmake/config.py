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

configFile = 'config.txt'

def getModuleConfig():
    config = []
    try:
        with open(configFile) as f:
            for line in f:
				if line.startswith('LIMA'):
					if line[len(line)-2]==str(1):
						config.append("-D"+line[:-1])
				elif line.startswith('CMAKE') or line.startswith('PYTHON'):
					config.append("-D"+line[:-1])
        config= " ".join([str(cmd) for cmd in config])
        return config
    except IOError:
        print 'Error'
        raise

		
if __name__ == '__main__':
	config = getModuleConfig()
	print config

