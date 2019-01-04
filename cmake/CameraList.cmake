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

#CAMERA WORKING ON BOTH LINUX AND WINDOWS

#ANDOR
if(DEFINED ENV{LIMACAMERA_ANDOR})
    set(LIMACAMERA_ANDOR "$ENV{LIMACAMERA_ANDOR}" CACHE BOOL "compile andor?" FORCE)
else()
    set(LIMACAMERA_ANDOR OFF CACHE BOOL "compile andor?")
endif()
if(LIMACAMERA_ANDOR)
   add_subdirectory(camera/andor)
endif(LIMACAMERA_ANDOR)

#BASLER
if(DEFINED ENV{LIMACAMERA_BASLER})
    set(LIMACAMERA_BASLER "$ENV{LIMACAMERA_BASLER}" CACHE BOOL "compile basler?" FORCE)
else()
    set(LIMACAMERA_BASLER OFF CACHE BOOL "compile basler?")
endif()
if(LIMACAMERA_BASLER)
    add_subdirectory(camera/basler)
endif(LIMACAMERA_BASLER)
	
#PCO
if(DEFINED ENV{LIMACAMERA_PCO})
    set(LIMACAMERA_PCO "$ENV{LIMACAMERA_PCO}" CACHE BOOL "compile pco?" FORCE)
else()
    set(LIMACAMERA_PCO OFF CACHE BOOL "compile pco?")
endif()
if(LIMACAMERA_PCO)
    add_subdirectory(camera/pco)
endif(LIMACAMERA_PCO)

#SIMULATOR
if(DEFINED ENV{LIMACAMERA_SIMULATOR})
    set(LIMACAMERA_SIMULATOR "$ENV{LIMACAMERA_SIMULATOR}" CACHE BOOL "compile simulator?" FORCE)
else()
    set(LIMACAMERA_SIMULATOR ON CACHE BOOL "compile simulator?")
endif()
if(LIMACAMERA_SIMULATOR)
    add_subdirectory(camera/simulator)
endif(LIMACAMERA_SIMULATOR)

#ROPERSCIENTIFIC
if(DEFINED ENV{LIMACAMERA_ROPERSCIENTIFIC})
   set(LIMACAMERA_ROPERSCIENTIFIC "$ENV{LIMACAMERA_ROPERSCIENTIFIC}" CACHE BOOL "compile roperscientific ?" FORCE)
else()
   set(LIMACAMERA_ROPERSCIENTIFIC OFF CACHE BOOL "compile roperscientific?")
endif()
if(LIMACAMERA_ROPERSCIENTIFIC)
    add_subdirectory(camera/roperscientific)
endif(LIMACAMERA_ROPERSCIENTIFIC)



#CAMERA ONLY WORKING ON LINUX
if(UNIX)
	#ANDOR3
	if(DEFINED ENV{LIMACAMERA_ANDOR3})
		set(LIMACAMERA_ANDOR3 "$ENV{LIMACAMERA_ANDOR3}" CACHE BOOL "compile andor3?" FORCE)
	else()
		set(LIMACAMERA_ANDOR3 OFF CACHE BOOL "compile andor3?")
	endif()
	if(LIMACAMERA_ANDOR3)
		add_subdirectory(camera/andor3)
	endif(LIMACAMERA_ANDOR3)

	#AVIEX
	if(DEFINED ENV{LIMACAMERA_AVIEX})
		set(LIMACAMERA_AVIEX "$ENV{COMPILE_AVIEX}" CACHE BOOL "compile aviex?" FORCE)
	else()
		set(LIMACAMERA_AVIEX OFF CACHE BOOL "compile aviex?")
	endif()
	if(LIMACAMERA_AVIEX)
		add_subdirectory(camera/aviex)
	endif(LIMACAMERA_AVIEX)
	
	#ADSC
	if(DEFINED ENV{LIMACAMERA_ADSC})
		set(LIMACAMERA_ADSC "$ENV{LIMACAMERA_ADSC}" CACHE BOOL "compile adsc?" FORCE)
	else()
		set(LIMACAMERA_ADSC OFF CACHE BOOL "compile adsc?")
	endif()
	if(LIMACAMERA_ADSC)
		add_subdirectory(camera/adsc)
	endif(LIMACAMERA_ADSC)
	
	#DEXELA
	if(DEFINED ENV{LIMACAMERA_DEXELA})
	   set(LIMACAMERA_DEXELA "$ENV{LIMACAMERA_DEXELA}" CACHE BOOL "compile dexela ?" FORCE)
	else()
	   set(LIMACAMERA_DEXELA OFF CACHE BOOL "compile dexela ?")
	endif()
	if(LIMACAMERA_DEXELA)
		add_subdirectory(camera/dexela)
	endif(LIMACAMERA_DEXELA)

	#EIGER
	if(DEFINED ENV{LIMACAMERA_EIGER})
		set(LIMACAMERA_EIGER "$ENV{LIMACAMERA_EIGER}" CACHE BOOL "compile eiger?" FORCE)
	else()
		set(LIMACAMERA_EIGER OFF CACHE BOOL "compile eiger?")
	endif()
	if(LIMACAMERA_EIGER)
		add_subdirectory(camera/eiger)
	endif(LIMACAMERA_EIGER)

	#FRELON
	if(DEFINED ENV{LIMACAMERA_FRELON})
		set(LIMACAMERA_FRELON "$ENV{LIMACAMERA_FRELON}" CACHE BOOL "compile frelon?" FORCE)
	else()
		set(LIMACAMERA_FRELON OFF CACHE BOOL "compile frelon?")
	endif()
	if(LIMACAMERA_FRELON)
		add_subdirectory(camera/frelon)
	endif(LIMACAMERA_FRELON)

	#FLI
	if(DEFINED ENV{LIMACAMERA_FLI})
		set(LIMACAMERA_FLI "$ENV{LIMACAMERA_FLI}" CACHE BOOL "compile Finger Lake Instrument camera?" FORCE)
	else()
		set(LIMACAMERA_FLI OFF CACHE BOOL "compile Finger Lake Instrument camera?")
	endif()
	if(LIMACAMERA_FLI)
		add_subdirectory(camera/fli)
	endif(LIMACAMERA_FLI)

	#HEXITEC
	if(DEFINED ENV{LIMACAMERA_HEXITEC})
		set(LIMACAMERA_HEXITEC "$ENV{LIMACAMERA_HEXITEC}" CACHE BOOL "compile hexitec ?" FORCE)
	else()
		set(LIMACAMERA_HEXITEC OFF CACHE BOOL "compile hexitec ?")
	endif()
	if(LIMACAMERA_HEXITEC)
		add_subdirectory(camera/hexitec)
	endif(LIMACAMERA_HEXITEC)

	#IMXPAD
	if(DEFINED ENV{LIMACAMERA_IMXPAD})
		set(LIMACAMERA_IMXPAD "$ENV{LIMACAMERA_IMXPAD}" CACHE BOOL "compile imxpad?" FORCE)
	else()
		set(LIMACAMERA_IMXPAD OFF CACHE BOOL "compile imxpad?")
	endif()
	if(LIMACAMERA_IMXPAD)
		add_subdirectory(camera/imxpad)
	endif(LIMACAMERA_IMXPAD)

	#LAMBDA
	if(DEFINED ENV{LIMACAMERA_LAMBDA})
		set(LIMACAMERA_LAMBDA "$ENV{LIMACAMERA_LAMBDA}" CACHE BOOL "compile lambda?" FORCE)
	else()
		set(LIMACAMERA_LAMBDA OFF CACHE BOOL "compile lambda?")
	endif()
	if(LIMACAMERA_LAMBDA)
		add_subdirectory(camera/lambda)
	endif(LIMACAMERA_LAMBDA)

	#MARCCD
	if(DEFINED ENV{LIMACAMERA_MARCCD})
		set(LIMACAMERA_MARCCD "$ENV{LIMACAMERA_MARCCD}" CACHE BOOL "compile marccd?" FORCE)
	else()
		set(LIMACAMERA_MARCCD OFF CACHE BOOL "compile marccd?")
	endif()
	if(LIMACAMERA_MARCCD)
		add_subdirectory(camera/marccd)
	endif(LIMACAMERA_MARCCD)

	#MAXIPIX
	if(DEFINED ENV{LIMACAMERA_MAXIPIX})
		set(LIMACAMERA_MAXIPIX "$ENV{LIMACAMERA_MAXIPIX}" CACHE BOOL "compile maxipix?" FORCE)
	else()
		set(LIMACAMERA_MAXIPIX OFF CACHE BOOL "compile maxipix?")
	endif()
	if(LIMACAMERA_MAXIPIX)
		add_subdirectory(camera/maxipix)
	endif(LIMACAMERA_MAXIPIX)

	#MERLIN
	if(DEFINED ENV{LIMACAMERA_MERLIN})
		set(LIMACAMERA_MERLIN "$ENV{LIMACAMERA_MERLIN}" CACHE BOOL "compile merlin?" FORCE)
	else()
		set(LIMACAMERA_MERLIN OFF CACHE BOOL "compile merlin?")
	endif()
	if(LIMACAMERA_MERLIN)
		add_subdirectory(camera/merlin)
	endif(LIMACAMERA_MERLIN)

	#MYTHEN
	if(DEFINED ENV{LIMACAMERA_MYTHEN})
		set(LIMACAMERA_MYTHEN "$ENV{LIMACAMERA_MYTHEN}" CACHE BOOL "compile mythen ?" FORCE)
	else()
		set(LIMACAMERA_MYTHEN OFF CACHE BOOL "compile mythen ?")
	endif()
	if(LIMACAMERA_MYTHEN)
		add_subdirectory(camera/mythen)
	endif(LIMACAMERA_MYTHEN)

	#MYTHEN3
	if(DEFINED ENV{LIMACAMERA_MYTHEN3})
		set(LIMACAMERA_MYTHEN3 "$ENV{LIMACAMERA_MYTHEN3}" CACHE BOOL "compile mythen3 ?" FORCE)
	else()
		set(LIMACAMERA_MYTHEN3 OFF CACHE BOOL "compile mythen3 ?")
	endif()
	if(LIMACAMERA_MYTHEN3)
		add_subdirectory(camera/mythen3)
	endif(LIMACAMERA_MYTHEN3)

	#PILATUS
	if(DEFINED ENV{LIMACAMERA_PILATUS})
		set(LIMACAMERA_PILATUS "$ENV{LIMACAMERA_PILATUS}" CACHE BOOL "compile pilatus?" FORCE)
	else()
		set(LIMACAMERA_PILATUS OFF CACHE BOOL "compile pilatus?")
	endif()
	if(LIMACAMERA_PILATUS)
		add_subdirectory(camera/pilatus)
	endif(LIMACAMERA_PILATUS)

	#PIXIRAD
	if(DEFINED ENV{LIMACAMERA_PIXIRAD})
		set(LIMACAMERA_PIXIRAD "$ENV{LIMACAMERA_PIXIRAD}" CACHE BOOL "compile pixirad?" FORCE)
	else()
		set(LIMACAMERA_PIXIRAD OFF CACHE BOOL "compile pixirad?")
	endif()
	if(LIMACAMERA_PIXIRAD)
		add_subdirectory(camera/pixirad)
	endif(LIMACAMERA_PIXIRAD)

	#POINTGREY
	if(DEFINED ENV{LIMACAMERA_POINTGREY})
		set(LIMACAMERA_POINTGREY "$ENV{LIMACAMERA_POINTGREY}" CACHE BOOL "compile pointgrey?" FORCE)
	else()
		set(LIMACAMERA_POINTGREY OFF CACHE BOOL "compile pointgrey?")
	endif()
	if(LIMACAMERA_POINTGREY)
		add_subdirectory(camera/pointgrey)
	endif(LIMACAMERA_POINTGREY)

	#PROSILICA
	if(DEFINED ENV{LIMACAMERA_PROSILICA})
		set(LIMACAMERA_PROSILICA "$ENV{LIMACAMERA_PROSILICA}" CACHE BOOL "compile prosilica?" FORCE)
	else()
		set(LIMACAMERA_PROSILICA OFF CACHE BOOL "compile prosilica?")
	endif()
	if(LIMACAMERA_PROSILICA)
		add_subdirectory(camera/prosilica)
	endif(LIMACAMERA_PROSILICA)
	
	#RAYONIX HS
	if(DEFINED ENV{LIMACAMERA_RAYONIXHS})
		set(LIMACAMERA_RAYONIXHS "$ENV{LIMACAMERA_RAYONIXHS}" CACHE BOOL "compile rayonix hs?" FORCE)
	else()
		set(LIMACAMERA_RAYONIXHS OFF CACHE BOOL "compile rayonix hs?")
	endif()
	if(LIMACAMERA_RAYONIXHS)
		add_subdirectory(camera/rayonixhs)
	endif(LIMACAMERA_RAYONIXHS)

	#SLSDETECTOR
	if(DEFINED ENV{LIMACAMERA_SLSDETECTOR})
		set(LIMACAMERA_SLSDETECTOR "$ENV{LIMACAMERA_SLSDETECTOR}" CACHE BOOL "compile slsdetector ?" FORCE)
	else()
		set(LIMACAMERA_SLSDETECTOR OFF CACHE BOOL "compile slsdetector ?")
	endif()	
	if(LIMACAMERA_SLSDETECTOR)
		add_subdirectory(camera/slsdetector)
	endif(LIMACAMERA_SLSDETECTOR)

	#UEYE
	if(DEFINED ENV{LIMACAMERA_UEYE})
		set(LIMACAMERA_UEYE "$ENV{LIMACAMERA_UEYE}" CACHE BOOL "compile ueye ?" FORCE)
	else()
		set(LIMACAMERA_UEYE OFF CACHE BOOL "compile ueye ?")
	endif()
	if(LIMACAMERA_UEYE)
		add_subdirectory(camera/ueye)
	endif(LIMACAMERA_UEYE)

	#ULTRA
	if(DEFINED ENV{LIMACAMERA_ULTRA})
		set(LIMACAMERA_ULTRA "$ENV{LIMACAMERA_ULTRA}" CACHE BOOL "compile ultra ?" FORCE)
	else()
		set(LIMACAMERA_ULTRA OFF CACHE BOOL "compile ultra ?")
	endif()
	if(LIMACAMERA_ULTRA)
		add_subdirectory(camera/ultra)
	endif(LIMACAMERA_ULTRA)

	#V4L2
	if(DEFINED ENV{LIMACAMERA_V4L2})
		set(LIMACAMERA_V4L2 "$ENV{LIMACAMERA_V4L2}" CACHE BOOL "compile v4l2 ?" FORCE)
	else()
		set(LIMACAMERA_V4L2 OFF CACHE BOOL "compile v4l2 ?")
	endif()
	if(LIMACAMERA_V4L2)
		add_subdirectory(camera/v4l2)
	endif(LIMACAMERA_V4L2)

	#XPAD
	if(DEFINED ENV{LIMACAMERA_XPAD})
		set(LIMACAMERA_XPAD "$ENV{LIMACAMERA_XPAD}" CACHE BOOL "compile Xpad ?" FORCE)
	else()
		set(LIMACAMERA_XPAD OFF CACHE BOOL "compile Xpad ?")
	endif()
	if(LIMACAMERA_XPAD)
		add_subdirectory(camera/xpad)
	endif(LIMACAMERA_XPAD)

	#XH
	if(DEFINED ENV{LIMACAMERA_XH})
		set(LIMACAMERA_XH "$ENV{LIMACAMERA_XH}" CACHE BOOL "compile xh ?" FORCE)
	else()
		set(LIMACAMERA_XH OFF CACHE BOOL "compile xh ?")
	endif()
	if(LIMACAMERA_XH)
		add_subdirectory(camera/xh)
	endif(LIMACAMERA_XH)

	#XSPRESS3
	if(DEFINED ENV{LIMACAMERA_XSPRESS3})
		set(LIMACAMERA_XSPRESS3 "$ENV{LIMACAMERA_XSPRESS3}" CACHE BOOL "compile xspress3 ?" FORCE)
	else()
		set(LIMACAMERA_XSPRESS3 OFF CACHE BOOL "compile xspress3 ?")
	endif()	
	if(LIMACAMERA_XSPRESS3)
		add_subdirectory(camera/xspress3)
	endif(LIMACAMERA_XSPRESS3)

	#ZWO
	if(DEFINED ENV{LIMACAMERA_ZWO})
		set(LIMACAMERA_ZWO "$ENV{LIMACAMERA_ZWO}" CACHE BOOL "compile zwo ?" FORCE)
	else()
		set(LIMACAMERA_ZWO OFF CACHE BOOL "compile zwo ?")
	endif()	
	if(LIMACAMERA_ZWO)
		add_subdirectory(camera/zwo)
	endif(LIMACAMERA_ZWO)

	if(LIMACAMERA_FRELON OR LIMACAMERA_MAXIPIX)
		add_subdirectory(camera/common/espia)
	endif(LIMACAMERA_FRELON OR LIMACAMERA_MAXIPIX)
endif()

#CAMERA ONLY WORKING ON WINDOWS
if(WIN32)
	#HAMAMATSU
	if(DEFINED ENV{LIMACAMERA_HAMAMATSU})
		set(LIMACAMERA_HAMAMATSU "$ENV{LIMACAMERA_HAMAMATSU}" CACHE BOOL "compile hamamatsu ?" FORCE)
	else()
		set(LIMACAMERA_HAMAMATSU OFF CACHE BOOL "compile hamamatsu ?")
	endif()
	if(LIMACAMERA_HAMAMATSU)
        	add_subdirectory(camera/hamamatsu)
	endif(LIMACAMERA_HAMAMATSU)

	#PERKIN ELMER
	if(DEFINED ENV{LIMACAMERA_PERKINELMER})
		set(LIMACAMERA_PERKINELMER "$ENV{LIMACAMERA_PERKINELMER}" CACHE BOOL "compile perkin elmer ?" FORCE)
	else()
		set(LIMACAMERA_PERKINELMER OFF CACHE BOOL "compile perkin elmer ?")
	endif()
	#PERKIN ELMER
	if(LIMACAMERA_PERKINELMER)
		add_subdirectory(camera/perkinelmer)
	endif(LIMACAMERA_PERKINELMER)
		
	#PHOTONICSCIENCE
	if(DEFINED ENV{LIMACAMERA_PHOTONICSCIENCE})
		set(LIMACAMERA_PHOTONICSCIENCE "$ENV{LIMACAMERA_PHOTONICSCIENCE}" CACHE BOOL "compile photonicscience ?" FORCE)
	else()
		set(LIMACAMERA_PHOTONICSCIENCE OFF CACHE BOOL "compile photonicscience ?")
	endif()
	if(LIMACAMERA_PHOTONICSCIENCE)
		add_subdirectory(camera/photonicscience)
	endif(LIMACAMERA_PHOTONICSCIENCE)
endif()
