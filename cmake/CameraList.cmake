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
IF(DEFINED ENV{COMPILE_ANDOR})
    set(COMPILE_ANDOR "$ENV{COMPILE_ANDOR}" CACHE BOOL "compile andor?" FORCE)
ELSE()
    set(COMPILE_ANDOR OFF CACHE BOOL "compile andor?")
ENDIF()
if(COMPILE_ANDOR)
   add_subdirectory(camera/andor)
endif(COMPILE_ANDOR)

#BASLER
IF(DEFINED ENV{COMPILE_BASLER})
    set(COMPILE_BASLER "$ENV{COMPILE_BASLER}" CACHE BOOL "compile basler?" FORCE)
ELSE()
    set(COMPILE_BASLER OFF CACHE BOOL "compile basler?")
ENDIF()
if(COMPILE_BASLER)
    add_subdirectory(camera/basler)
endif(COMPILE_BASLER)

#DEXELA
IF(DEFINED ENV{COMPILE_DEXELA})
   set(COMPILE_DEXELA "$ENV{COMPILE_DEXELA}" CACHE BOOL "compile dexela ?" FORCE)
ELSE()
   set(COMPILE_DEXELA OFF CACHE BOOL "compile dexela ?")
ENDIF()
if(COMPILE_DEXELA)
    add_subdirectory(camera/dexela)
endif(COMPILE_DEXELA)
	
#PCO
IF(DEFINED ENV{COMPILE_PCO})
    set(COMPILE_PCO "$ENV{COMPILE_PCO}" CACHE BOOL "compile pco?" FORCE)
ELSE()
    set(COMPILE_PCO OFF CACHE BOOL "compile pco?")
ENDIF()
if(COMPILE_PCO)
    add_subdirectory(camera/pco)
endif(COMPILE_PCO)

#SIMULATOR
IF(DEFINED ENV{COMPILE_SIMULATOR})
    set(COMPILE_SIMULATOR "$ENV{COMPILE_SIMULATOR}" CACHE BOOL "compile simulator?" FORCE)
ELSE()
    set(COMPILE_SIMULATOR ON CACHE BOOL "compile simulator?")
ENDIF()
if(COMPILE_SIMULATOR)
    add_subdirectory(camera/simulator)
endif(COMPILE_SIMULATOR)

#ROPERSCIENTIFIC
IF(DEFINED ENV{COMPILE_ROPERSCIENTIFIC})
   set(COMPILE_ROPERSCIENTIFIC "$ENV{COMPILE_ROPERSCIENTIFIC}" CACHE BOOL "compile roperscientific ?" FORCE)
ELSE()
   set(COMPILE_ROPERSCIENTIFIC OFF CACHE BOOL "compile roperscientific?")
ENDIF()
if(COMPILE_ROPERSCIENTIFIC)
    add_subdirectory(camera/roperscientific)
endif(COMPILE_ROPERSCIENTIFIC)



#CAMERA ONLY WORKING ON LINUX
IF(UNIX)
	#ANDOR3
	IF(DEFINED ENV{COMPILE_ANDOR3})
		set(COMPILE_ANDOR3 "$ENV{COMPILE_ANDOR3}" CACHE BOOL "compile andor3?" FORCE)
	ELSE()
		set(COMPILE_ANDOR3 OFF CACHE BOOL "compile andor3?")
	ENDIF()
	if(COMPILE_ANDOR3)
		add_subdirectory(camera/andor3)
	endif(COMPILE_ANDOR3)

	#AVIEX
	IF(DEFINED ENV{COMPILE_AVIEX})
		set(COMPILE_AVIEX "$ENV{COMPILE_AVIEX}" CACHE BOOL "compile aviex?" FORCE)
	ELSE()
		set(COMPILE_AVIEX OFF CACHE BOOL "compile aviex?")
	ENDIF()
	if(COMPILE_AVIEX)
		add_subdirectory(camera/aviex)
	endif(COMPILE_AVIEX)
	
	#ADSC
	IF(DEFINED ENV{COMPILE_ADSC})
		set(COMPILE_ADSC "$ENV{COMPILE_ADSC}" CACHE BOOL "compile adsc?" FORCE)
	ELSE()
		set(COMPILE_ADSC OFF CACHE BOOL "compile adsc?")
	ENDIF()
	if(COMPILE_ADSC)
		add_subdirectory(camera/adsc)
	endif(COMPILE_ADSC)

	#ESPIA
	IF(DEFINED ENV{COMPILE_ESPIA})
		set(COMPILE_ESPIA "$ENV{COMPILE_ESPIA}" CACHE BOOL "compile espia?" FORCE)
	ELSE()
		set(COMPILE_ESPIA OFF CACHE BOOL "compile espia?")
	ENDIF()
	if(COMPILE_ESPIA)
		add_subdirectory(camera/common/espia)
	endif(COMPILE_ESPIA)

	#EIGER
	IF(DEFINED ENV{COMPILE_EIGER})
		set(COMPILE_EIGER "$ENV{COMPILE_EIGER}" CACHE BOOL "compile eiger?" FORCE)
	ELSE()
		set(COMPILE_EIGER OFF CACHE BOOL "compile eiger?")
	ENDIF()
	if(COMPILE_EIGER)
		add_subdirectory(camera/eiger)
	endif(COMPILE_EIGER)

	#FRELON
	IF(DEFINED ENV{COMPILE_FRELON})
		set(COMPILE_FRELON "$ENV{COMPILE_FRELON}" CACHE BOOL "compile frelon?" FORCE)
	ELSE()
		set(COMPILE_FRELON OFF CACHE BOOL "compile frelon?")
	ENDIF()
	if(COMPILE_FRELON)
		add_subdirectory(camera/frelon)
	endif(COMPILE_FRELON)

	#HEXITEC
	IF(DEFINED ENV{COMPILE_HEXITEC})
		set(COMPILE_HEXITEC "$ENV{COMPILE_HEXITEC}" CACHE BOOL "compile hexitec ?" FORCE)
	ELSE()
		set(COMPILE_HEXITEC OFF CACHE BOOL "compile hexitec ?")
	ENDIF()
	if(COMPILE_HEXITEC)
		add_subdirectory(camera/hexitec)
	endif(COMPILE_HEXITEC)

	#IMXPAD
	IF(DEFINED ENV{COMPILE_IMXPAD})
		set(COMPILE_IMXPAD "$ENV{COMPILE_IMXPAD}" CACHE BOOL "compile imxpad?" FORCE)
	ELSE()
		set(COMPILE_IMXPAD OFF CACHE BOOL "compile imxpad?")
	ENDIF()
	if(COMPILE_IMXPAD)
		add_subdirectory(camera/imxpad)
	endif(COMPILE_IMXPAD)

	#MARCCD
	IF(DEFINED ENV{COMPILE_MARCCD})
		set(COMPILE_MARCCD "$ENV{COMPILE_MARCCD}" CACHE BOOL "compile marccd?" FORCE)
	ELSE()
		set(COMPILE_MARCCD OFF CACHE BOOL "compile marccd?")
	ENDIF()
	if(COMPILE_MARCCD)
		add_subdirectory(camera/marccd)
	endif(COMPILE_MARCCD)

	#MAXIPIX
	IF(DEFINED ENV{COMPILE_MAXIPIX})
		set(COMPILE_MAXIPIX "$ENV{COMPILE_MAXIPIX}" CACHE BOOL "compile maxipix?" FORCE)
	ELSE()
		set(COMPILE_MAXIPIX OFF CACHE BOOL "compile maxipix?")
	ENDIF()
	if(COMPILE_MAXIPIX)
		add_subdirectory(camera/maxipix)
	endif(COMPILE_MAXIPIX)

	#MERLIN
	IF(DEFINED ENV{COMPILE_MERLIN})
		set(COMPILE_MERLIN "$ENV{COMPILE_MERLIN}" CACHE BOOL "compile merlin?" FORCE)
	ELSE()
		set(COMPILE_MERLIN OFF CACHE BOOL "compile merlin?")
	ENDIF()
	if(COMPILE_MERLIN)
		add_subdirectory(camera/merlin)
	endif(COMPILE_MERLIN)

	#MYTHEN
	IF(DEFINED ENV{COMPILE_MYTHEN})
		set(COMPILE_MYTHEN "$ENV{COMPILE_MYTHEN}" CACHE BOOL "compile mythen ?" FORCE)
	ELSE()
		set(COMPILE_MYTHEN OFF CACHE BOOL "compile mythen ?")
	ENDIF()
	if(COMPILE_MYTHEN)
		add_subdirectory(camera/mythen)
	endif(COMPILE_MYTHEN)

	#MYTHEN3
	IF(DEFINED ENV{COMPILE_MYTHEN3})
		set(COMPILE_MYTHEN3 "$ENV{COMPILE_MYTHEN3}" CACHE BOOL "compile mythen3 ?" FORCE)
	ELSE()
		set(COMPILE_MYTHEN3 OFF CACHE BOOL "compile mythen3 ?")
	ENDIF()
	if(COMPILE_MYTHEN3)
		add_subdirectory(camera/mythen3)
	endif(COMPILE_MYTHEN3)

	#PILATUS
	IF(DEFINED ENV{COMPILE_PILATUS})
		set(COMPILE_PILATUS "$ENV{COMPILE_PILATUS}" CACHE BOOL "compile pilatus?" FORCE)
	ELSE()
		set(COMPILE_PILATUS OFF CACHE BOOL "compile pilatus?")
	ENDIF()
	if(COMPILE_PILATUS)
		add_subdirectory(camera/pilatus)
	endif(COMPILE_PILATUS)


	#PIXIRAD
	IF(DEFINED ENV{COMPILE_PIXIRAD})
		set(COMPILE_PIXIRAD "$ENV{COMPILE_PIXIRAD}" CACHE BOOL "compile pixirad?" FORCE)
	ELSE()
		set(COMPILE_PIXIRAD OFF CACHE BOOL "compile pixirad?")
	ENDIF()
	if(COMPILE_PIXIRAD)
		add_subdirectory(camera/pixirad)
	endif(COMPILE_PIXIRAD)


	#POINTGREY
	IF(DEFINED ENV{COMPILE_POINTGREY})
		set(COMPILE_POINTGREY "$ENV{COMPILE_POINTGREY}" CACHE BOOL "compile pointgrey?" FORCE)
	ELSE()
		set(COMPILE_POINTGREY OFF CACHE BOOL "compile pointgrey?")
	ENDIF()
	if(COMPILE_POINTGREY)
		add_subdirectory(camera/pointgrey)
	endif(COMPILE_POINTGREY)

	#PROSILICA
	IF(DEFINED ENV{COMPILE_PROSILICA})
		set(COMPILE_PROSILICA "$ENV{COMPILE_PROSILICA}" CACHE BOOL "compile prosilica?" FORCE)
	ELSE()
		set(COMPILE_PROSILICA OFF CACHE BOOL "compile prosilica?")
	ENDIF()
	if(COMPILE_PROSILICA)
		add_subdirectory(camera/prosilica)
	endif(COMPILE_PROSILICA)
	
	#RAYONIX HS
	IF(DEFINED ENV{COMPILE_RAYONIXHS})
		set(COMPILE_RAYONIXHS "$ENV{COMPILE_RAYONIXHS}" CACHE BOOL "compile rayonix hs?" FORCE)
	ELSE()
		set(COMPILE_RAYONIXHS OFF CACHE BOOL "compile rayonix hs?")
	ENDIF()
	if(COMPILE_RAYONIXHS)
		add_subdirectory(camera/rayonixhs)
	endif(COMPILE_RAYONIXHS)

	#UEYE
	IF(DEFINED ENV{COMPILE_UEYE})
		set(COMPILE_UEYE "$ENV{COMPILE_UEYE}" CACHE BOOL "compile ueye ?" FORCE)
	ELSE()
		set(COMPILE_UEYE OFF CACHE BOOL "compile ueye ?")
	ENDIF()
	if(COMPILE_UEYE)
		add_subdirectory(camera/ueye)
	endif(COMPILE_UEYE)

	#ULTRA
	IF(DEFINED ENV{COMPILE_ULTRA})
		set(COMPILE_ULTRA "$ENV{COMPILE_ULTRA}" CACHE BOOL "compile ultra ?" FORCE)
	ELSE()
		set(COMPILE_ULTRA OFF CACHE BOOL "compile ultra ?")
	ENDIF()
	if(COMPILE_ULTRA)
		add_subdirectory(camera/ultra)
	endif(COMPILE_ULTRA)

	#V4L2
	IF(DEFINED ENV{COMPILE_V4L2})
		set(COMPILE_V4L2 "$ENV{COMPILE_V4L2}" CACHE BOOL "compile v4l2 ?" FORCE)
	ELSE()
		set(COMPILE_V4L2 OFF CACHE BOOL "compile v4l2 ?")
	ENDIF()
	if(COMPILE_V4L2)
		add_subdirectory(camera/v4l2)
	endif(COMPILE_V4L2)

	#XPAD
	IF(DEFINED ENV{COMPILE_XPAD})
		set(COMPILE_XPAD "$ENV{COMPILE_XPAD}" CACHE BOOL "compile Xpad ?" FORCE)
	ELSE()
		set(COMPILE_XPAD OFF CACHE BOOL "compile Xpad ?")
	ENDIF()
	if(COMPILE_XPAD)
		add_subdirectory(camera/xpad)
	endif(COMPILE_XPAD)

	#XH
	IF(DEFINED ENV{COMPILE_XH})
		set(COMPILE_XH "$ENV{COMPILE_XH}" CACHE BOOL "compile xh ?" FORCE)
	ELSE()
		set(COMPILE_XH OFF CACHE BOOL "compile xh ?")
	ENDIF()
	if(COMPILE_XH)
		add_subdirectory(camera/xh)
	endif(COMPILE_XH)

	#XSPRESS3
	IF(DEFINED ENV{COMPILE_XSPRESS3})
		set(COMPILE_XSPRESS3 "$ENV{COMPILE_XSPRESS3}" CACHE BOOL "compile xspress3 ?" FORCE)
	ELSE()
		set(COMPILE_XSPRESS3 OFF CACHE BOOL "compile xspress3 ?")
	ENDIF()	
	if(COMPILE_XSPRESS3)
		add_subdirectory(camera/xspress3)
	endif(COMPILE_XSPRESS3)

ENDIF()

#CAMERA ONLY WORKING ON WINDOWS
IF(WIN32)
	#HAMAMATSU
	IF(DEFINED ENV{COMPILE_HAMAMATSU})
		set(COMPILE_HAMAMATSU "$ENV{COMPILE_HAMAMATSU}" CACHE BOOL "compile hamamatsu ?" FORCE)
	ELSE()
		set(COMPILE_HAMAMATSU OFF CACHE BOOL "compile hamamatsu ?")
	ENDIF()
	if(COMPILE_HAMAMATSU)
        	add_subdirectory(camera/hamamatsu)
	endif(COMPILE_HAMAMATSU)

	#PERKIN ELMER
	IF(DEFINED ENV{COMPILE_PERKINELMER})
		set(COMPILE_PERKINELMER "$ENV{COMPILE_PERKINELMER}" CACHE BOOL "compile perkin elmer ?" FORCE)
	ELSE()
		set(COMPILE_PERKINELMER OFF CACHE BOOL "compile perkin elmer ?")
	ENDIF()
	#PERKIN ELMER
	if(COMPILE_PERKINELMER)
		add_subdirectory(camera/perkinelmer)
	endif(COMPILE_PERKINELMER)
		
	#PHOTONICSCIENCE
	IF(DEFINED ENV{COMPILE_PHOTONICSCIENCE})
		set(COMPILE_PHOTONICSCIENCE "$ENV{COMPILE_PHOTONICSCIENCE}" CACHE BOOL "compile photonicscience ?" FORCE)
	ELSE()
		set(COMPILE_PHOTONICSCIENCE OFF CACHE BOOL "compile photonicscience ?")
	ENDIF()
	if(COMPILE_PHOTONICSCIENCE)
		add_subdirectory(camera/photonicscience)
	endif(COMPILE_PHOTONICSCIENCE)
ENDIF()