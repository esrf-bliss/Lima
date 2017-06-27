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
if(DEFINED ENV{COMPILE_CAMERA_ANDOR})
    set(COMPILE_CAMERA_ANDOR "$ENV{COMPILE_CAMERA_ANDOR}" CACHE BOOL "compile andor?" FORCE)
else()
    set(COMPILE_CAMERA_ANDOR OFF CACHE BOOL "compile andor?")
endif()
if(COMPILE_CAMERA_ANDOR)
   add_subdirectory(camera/andor)
endif(COMPILE_CAMERA_ANDOR)

#BASLER
if(DEFINED ENV{COMPILE_CAMERA_BASLER})
    set(COMPILE_CAMERA_BASLER "$ENV{COMPILE_CAMERA_BASLER}" CACHE BOOL "compile basler?" FORCE)
else()
    set(COMPILE_CAMERA_BASLER OFF CACHE BOOL "compile basler?")
endif()
if(COMPILE_CAMERA_BASLER)
    add_subdirectory(camera/basler)
endif(COMPILE_CAMERA_BASLER)

#DEXELA
if(DEFINED ENV{COMPILE_CAMERA_DEXELA})
   set(COMPILE_CAMERA_DEXELA "$ENV{COMPILE_CAMERA_DEXELA}" CACHE BOOL "compile dexela ?" FORCE)
else()
   set(COMPILE_CAMERA_DEXELA OFF CACHE BOOL "compile dexela ?")
endif()
if(COMPILE_CAMERA_DEXELA)
    add_subdirectory(camera/dexela)
endif(COMPILE_CAMERA_DEXELA)
	
#PCO
if(DEFINED ENV{COMPILE_CAMERA_PCO})
    set(COMPILE_CAMERA_PCO "$ENV{COMPILE_CAMERA_PCO}" CACHE BOOL "compile pco?" FORCE)
else()
    set(COMPILE_CAMERA_PCO OFF CACHE BOOL "compile pco?")
endif()
if(COMPILE_CAMERA_PCO)
    add_subdirectory(camera/pco)
endif(COMPILE_CAMERA_PCO)

#SIMULATOR
if(DEFINED ENV{COMPILE_SIMULATOR})
    set(COMPILE_SIMULATOR "$ENV{COMPILE_SIMULATOR}" CACHE BOOL "compile simulator?" FORCE)
else()
    set(COMPILE_SIMULATOR ON CACHE BOOL "compile simulator?")
endif()
if(COMPILE_SIMULATOR)
    add_subdirectory(camera/simulator)
endif(COMPILE_SIMULATOR)

#ROPERSCIENTIFIC
if(DEFINED ENV{COMPILE_CAMERA_ROPERSCIENTIFIC})
   set(COMPILE_CAMERA_ROPERSCIENTIFIC "$ENV{COMPILE_CAMERA_ROPERSCIENTIFIC}" CACHE BOOL "compile roperscientific ?" FORCE)
else()
   set(COMPILE_CAMERA_ROPERSCIENTIFIC OFF CACHE BOOL "compile roperscientific?")
endif()
if(COMPILE_CAMERA_ROPERSCIENTIFIC)
    add_subdirectory(camera/roperscientific)
endif(COMPILE_CAMERA_ROPERSCIENTIFIC)



#CAMERA ONLY WORKING ON LINUX
if(UNIX)
	#ANDOR3
	if(DEFINED ENV{COMPILE_CAMERA_ANDOR3})
		set(COMPILE_CAMERA_ANDOR3 "$ENV{COMPILE_CAMERA_ANDOR3}" CACHE BOOL "compile andor3?" FORCE)
	else()
		set(COMPILE_CAMERA_ANDOR3 OFF CACHE BOOL "compile andor3?")
	endif()
	if(COMPILE_CAMERA_ANDOR3)
		add_subdirectory(camera/andor3)
	endif(COMPILE_CAMERA_ANDOR3)

	#AVIEX
	if(DEFINED ENV{COMPILE_CAMERA_AVIEX})
		set(COMPILE_CAMERA_AVIEX "$ENV{COMPILE_AVIEX}" CACHE BOOL "compile aviex?" FORCE)
	else()
		set(COMPILE_CAMERA_AVIEX OFF CACHE BOOL "compile aviex?")
	endif()
	if(COMPILE_CAMERA_AVIEX)
		add_subdirectory(camera/aviex)
	endif(COMPILE_CAMERA_AVIEX)
	
	#ADSC
	if(DEFINED ENV{COMPILE_CAMERA_ADSC})
		set(COMPILE_CAMERA_ADSC "$ENV{COMPILE_CAMERA_ADSC}" CACHE BOOL "compile adsc?" FORCE)
	else()
		set(COMPILE_CAMERA_ADSC OFF CACHE BOOL "compile adsc?")
	endif()
	if(COMPILE_CAMERA_ADSC)
		add_subdirectory(camera/adsc)
	endif(COMPILE_CAMERA_ADSC)

	#ESPIA
	if(DEFINED ENV{COMPILE_CAMERA_ESPIA})
		set(COMPILE_CAMERA_ESPIA "$ENV{COMPILE_CAMERA_ESPIA}" CACHE BOOL "compile espia?" FORCE)
	else()
		set(COMPILE_CAMERA_ESPIA OFF CACHE BOOL "compile espia?")
	endif()
	if(COMPILE_CAMERA_ESPIA)
		add_subdirectory(camera/common/espia)
	endif(COMPILE_CAMERA_ESPIA)

	#EIGER
	if(DEFINED ENV{COMPILE_CAMERA_EIGER})
		set(COMPILE_CAMERA_EIGER "$ENV{COMPILE_CAMERA_EIGER}" CACHE BOOL "compile eiger?" FORCE)
	else()
		set(COMPILE_CAMERA_EIGER OFF CACHE BOOL "compile eiger?")
	endif()
	if(COMPILE_CAMERA_EIGER)
		add_subdirectory(camera/eiger)
	endif(COMPILE_CAMERA_EIGER)

	#FRELON
	if(DEFINED ENV{COMPILE_CAMERA_FRELON})
		set(COMPILE_CAMERA_FRELON "$ENV{COMPILE_CAMERA_FRELON}" CACHE BOOL "compile frelon?" FORCE)
	else()
		set(COMPILE_CAMERA_FRELON OFF CACHE BOOL "compile frelon?")
	endif()
	if(COMPILE_CAMERA_FRELON)
		add_subdirectory(camera/frelon)
	endif(COMPILE_CAMERA_FRELON)

	#HEXITEC
	if(DEFINED ENV{COMPILE_CAMERA_HEXITEC})
		set(COMPILE_CAMERA_HEXITEC "$ENV{COMPILE_CAMERA_HEXITEC}" CACHE BOOL "compile hexitec ?" FORCE)
	else()
		set(COMPILE_CAMERA_HEXITEC OFF CACHE BOOL "compile hexitec ?")
	endif()
	if(COMPILE_CAMERA_HEXITEC)
		add_subdirectory(camera/hexitec)
	endif(COMPILE_CAMERA_HEXITEC)

	#IMXPAD
	if(DEFINED ENV{COMPILE_CAMERA_IMXPAD})
		set(COMPILE_CAMERA_IMXPAD "$ENV{COMPILE_CAMERA_IMXPAD}" CACHE BOOL "compile imxpad?" FORCE)
	else()
		set(COMPILE_CAMERA_IMXPAD OFF CACHE BOOL "compile imxpad?")
	endif()
	if(COMPILE_CAMERA_IMXPAD)
		add_subdirectory(camera/imxpad)
	endif(COMPILE_CAMERA_IMXPAD)

	#MARCCD
	if(DEFINED ENV{COMPILE_CAMERA_MARCCD})
		set(COMPILE_CAMERA_MARCCD "$ENV{COMPILE_CAMERA_MARCCD}" CACHE BOOL "compile marccd?" FORCE)
	else()
		set(COMPILE_CAMERA_MARCCD OFF CACHE BOOL "compile marccd?")
	endif()
	if(COMPILE_CAMERA_MARCCD)
		add_subdirectory(camera/marccd)
	endif(COMPILE_CAMERA_MARCCD)

	#MAXIPIX
	if(DEFINED ENV{COMPILE_CAMERA_MAXIPIX})
		set(COMPILE_CAMERA_MAXIPIX "$ENV{COMPILE_CAMERA_MAXIPIX}" CACHE BOOL "compile maxipix?" FORCE)
	else()
		set(COMPILE_CAMERA_MAXIPIX OFF CACHE BOOL "compile maxipix?")
	endif()
	if(COMPILE_CAMERA_MAXIPIX)
		add_subdirectory(camera/maxipix)
	endif(COMPILE_CAMERA_MAXIPIX)

	#MERLIN
	if(DEFINED ENV{COMPILE_CAMERA_MERLIN})
		set(COMPILE_CAMERA_MERLIN "$ENV{COMPILE_CAMERA_MERLIN}" CACHE BOOL "compile merlin?" FORCE)
	else()
		set(COMPILE_CAMERA_MERLIN OFF CACHE BOOL "compile merlin?")
	endif()
	if(COMPILE_CAMERA_MERLIN)
		add_subdirectory(camera/merlin)
	endif(COMPILE_CAMERA_MERLIN)

	#MYTHEN
	if(DEFINED ENV{COMPILE_CAMERA_MYTHEN})
		set(COMPILE_CAMERA_MYTHEN "$ENV{COMPILE_CAMERA_MYTHEN}" CACHE BOOL "compile mythen ?" FORCE)
	else()
		set(COMPILE_CAMERA_MYTHEN OFF CACHE BOOL "compile mythen ?")
	endif()
	if(COMPILE_CAMERA_MYTHEN)
		add_subdirectory(camera/mythen)
	endif(COMPILE_CAMERA_MYTHEN)

	#MYTHEN3
	if(DEFINED ENV{COMPILE_CAMERA_MYTHEN3})
		set(COMPILE_CAMERA_MYTHEN3 "$ENV{COMPILE_CAMERA_MYTHEN3}" CACHE BOOL "compile mythen3 ?" FORCE)
	else()
		set(COMPILE_CAMERA_MYTHEN3 OFF CACHE BOOL "compile mythen3 ?")
	endif()
	if(COMPILE_CAMERA_MYTHEN3)
		add_subdirectory(camera/mythen3)
	endif(COMPILE_CAMERA_MYTHEN3)

	#PILATUS
	if(DEFINED ENV{COMPILE_CAMERA_PILATUS})
		set(COMPILE_CAMERA_PILATUS "$ENV{COMPILE_CAMERA_PILATUS}" CACHE BOOL "compile pilatus?" FORCE)
	else()
		set(COMPILE_CAMERA_PILATUS OFF CACHE BOOL "compile pilatus?")
	endif()
	if(COMPILE_CAMERA_PILATUS)
		add_subdirectory(camera/pilatus)
	endif(COMPILE_CAMERA_PILATUS)


	#PIXIRAD
	if(DEFINED ENV{COMPILE_CAMERA_PIXIRAD})
		set(COMPILE_CAMERA_PIXIRAD "$ENV{COMPILE_CAMERA_PIXIRAD}" CACHE BOOL "compile pixirad?" FORCE)
	else()
		set(COMPILE_CAMERA_PIXIRAD OFF CACHE BOOL "compile pixirad?")
	endif()
	if(COMPILE_CAMERA_PIXIRAD)
		add_subdirectory(camera/pixirad)
	endif(COMPILE_CAMERA_PIXIRAD)


	#POINTGREY
	if(DEFINED ENV{COMPILE_CAMERA_POINTGREY})
		set(COMPILE_CAMERA_POINTGREY "$ENV{COMPILE_CAMERA_POINTGREY}" CACHE BOOL "compile pointgrey?" FORCE)
	else()
		set(COMPILE_CAMERA_POINTGREY OFF CACHE BOOL "compile pointgrey?")
	endif()
	if(COMPILE_CAMERA_POINTGREY)
		add_subdirectory(camera/pointgrey)
	endif(COMPILE_CAMERA_POINTGREY)

	#PROSILICA
	if(DEFINED ENV{COMPILE_CAMERA_PROSILICA})
		set(COMPILE_CAMERA_PROSILICA "$ENV{COMPILE_CAMERA_PROSILICA}" CACHE BOOL "compile prosilica?" FORCE)
	else()
		set(COMPILE_CAMERA_PROSILICA OFF CACHE BOOL "compile prosilica?")
	endif()
	if(COMPILE_CAMERA_PROSILICA)
		add_subdirectory(camera/prosilica)
	endif(COMPILE_CAMERA_PROSILICA)
	
	#RAYONIX HS
	if(DEFINED ENV{COMPILE_CAMERA_RAYONIXHS})
		set(COMPILE_CAMERA_RAYONIXHS "$ENV{COMPILE_CAMERA_RAYONIXHS}" CACHE BOOL "compile rayonix hs?" FORCE)
	else()
		set(COMPILE_CAMERA_RAYONIXHS OFF CACHE BOOL "compile rayonix hs?")
	endif()
	if(COMPILE_CAMERA_RAYONIXHS)
		add_subdirectory(camera/rayonixhs)
	endif(COMPILE_CAMERA_RAYONIXHS)

	#UEYE
	if(DEFINED ENV{COMPILE_CAMERA_UEYE})
		set(COMPILE_CAMERA_UEYE "$ENV{COMPILE_CAMERA_UEYE}" CACHE BOOL "compile ueye ?" FORCE)
	else()
		set(COMPILE_CAMERA_UEYE OFF CACHE BOOL "compile ueye ?")
	endif()
	if(COMPILE_CAMERA_UEYE)
		add_subdirectory(camera/ueye)
	endif(COMPILE_CAMERA_UEYE)

	#ULTRA
	if(DEFINED ENV{COMPILE_CAMERA_ULTRA})
		set(COMPILE_CAMERA_ULTRA "$ENV{COMPILE_CAMERA_ULTRA}" CACHE BOOL "compile ultra ?" FORCE)
	else()
		set(COMPILE_CAMERA_ULTRA OFF CACHE BOOL "compile ultra ?")
	endif()
	if(COMPILE_CAMERA_ULTRA)
		add_subdirectory(camera/ultra)
	endif(COMPILE_CAMERA_ULTRA)

	#V4L2
	if(DEFINED ENV{COMPILE_CAMERA_V4L2})
		set(COMPILE_CAMERA_V4L2 "$ENV{COMPILE_CAMERA_V4L2}" CACHE BOOL "compile v4l2 ?" FORCE)
	else()
		set(COMPILE_CAMERA_V4L2 OFF CACHE BOOL "compile v4l2 ?")
	endif()
	if(COMPILE_CAMERA_V4L2)
		add_subdirectory(camera/v4l2)
	endif(COMPILE_CAMERA_V4L2)

	#XPAD
	if(DEFINED ENV{COMPILE_CAMERA_XPAD})
		set(COMPILE_CAMERA_XPAD "$ENV{COMPILE_CAMERA_XPAD}" CACHE BOOL "compile Xpad ?" FORCE)
	else()
		set(COMPILE_CAMERA_XPAD OFF CACHE BOOL "compile Xpad ?")
	endif()
	if(COMPILE_CAMERA_XPAD)
		add_subdirectory(camera/xpad)
	endif(COMPILE_CAMERA_XPAD)

	#XH
	if(DEFINED ENV{COMPILE_CAMERA_XH})
		set(COMPILE_CAMERA_XH "$ENV{COMPILE_CAMERA_XH}" CACHE BOOL "compile xh ?" FORCE)
	else()
		set(COMPILE_CAMERA_XH OFF CACHE BOOL "compile xh ?")
	endif()
	if(COMPILE_CAMERA_XH)
		add_subdirectory(camera/xh)
	endif(COMPILE_CAMERA_XH)

	#XSPRESS3
	if(DEFINED ENV{COMPILE_CAMERA_XSPRESS3})
		set(COMPILE_CAMERA_XSPRESS3 "$ENV{COMPILE_CAMERA_XSPRESS3}" CACHE BOOL "compile xspress3 ?" FORCE)
	else()
		set(COMPILE_CAMERA_XSPRESS3 OFF CACHE BOOL "compile xspress3 ?")
	endif()	
	if(COMPILE_CAMERA_XSPRESS3)
		add_subdirectory(camera/xspress3)
	endif(COMPILE_CAMERA_XSPRESS3)

endif()

#CAMERA ONLY WORKING ON WINDOWS
if(WIN32)
	#HAMAMATSU
	if(DEFINED ENV{COMPILE_CAMERA_HAMAMATSU})
		set(COMPILE_CAMERA_HAMAMATSU "$ENV{COMPILE_CAMERA_HAMAMATSU}" CACHE BOOL "compile hamamatsu ?" FORCE)
	else()
		set(COMPILE_CAMERA_HAMAMATSU OFF CACHE BOOL "compile hamamatsu ?")
	endif()
	if(COMPILE_CAMERA_HAMAMATSU)
        	add_subdirectory(camera/hamamatsu)
	endif(COMPILE_CAMERA_HAMAMATSU)

	#PERKIN ELMER
	if(DEFINED ENV{COMPILE_CAMERA_PERKINELMER})
		set(COMPILE_CAMERA_PERKINELMER "$ENV{COMPILE_CAMERA_PERKINELMER}" CACHE BOOL "compile perkin elmer ?" FORCE)
	else()
		set(COMPILE_CAMERA_PERKINELMER OFF CACHE BOOL "compile perkin elmer ?")
	endif()
	#PERKIN ELMER
	if(COMPILE_CAMERA_PERKINELMER)
		add_subdirectory(camera/perkinelmer)
	endif(COMPILE_CAMERA_PERKINELMER)
		
	#PHOTONICSCIENCE
	if(DEFINED ENV{COMPILE_CAMERA_PHOTONICSCIENCE})
		set(COMPILE_CAMERA_PHOTONICSCIENCE "$ENV{COMPILE_CAMERA_PHOTONICSCIENCE}" CACHE BOOL "compile photonicscience ?" FORCE)
	else()
		set(COMPILE_CAMERA_PHOTONICSCIENCE OFF CACHE BOOL "compile photonicscience ?")
	endif()
	if(COMPILE_CAMERA_PHOTONICSCIENCE)
		add_subdirectory(camera/photonicscience)
	endif(COMPILE_CAMERA_PHOTONICSCIENCE)
endif()
