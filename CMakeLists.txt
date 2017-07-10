############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2017
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

cmake_minimum_required(VERSION 2.8)
project (lima)

IF(UNIX)
	SET(COMMON_PATH "/usr/local" CACHE PATH "path for lib and include used for several cameras")
	SET(YAT_ROOT "/usr/local/yat" CACHE PATH "temporary path for yat include and lib used by few cameras")
	SET(OPT_ROOT "/opt" CACHE PATH "path for /opt directory")
ENDIF()

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
FIND_PACKAGE(PythonInterp 2.7)
FIND_PACKAGE(PythonLibs 2.7)
IF (${PYTHONINTERP_FOUND})
    # python site-packages folder
    execute_process(
       COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print get_python_lib()"
       OUTPUT_VARIABLE _PYTHON_SITE_PACKAGES_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
ENDIF()
FIND_PACKAGE(SIP)
INCLUDE(SIPMacros)

SET(PYTHON_SITE_PACKAGES_DIR ${_PYTHON_SITE_PACKAGES_DIR} CACHE PATH "where should python modules be installed?")

IF (WIN32) #Paths based on wid00limande and wid15perkin2
	IF("${PYTHON_VERSION_MINOR}" STREQUAL "6")
    	SET(NUMPY_INCLUDE_DIR "C:/Python26/Lib/site-packages/numpy/core/include" CACHE PATH "Path to NumPy include folder")
    	SET(NUMPY_LIB_DIR "C:/Python26/Lib/site-packages/numpy/core/lib" CACHE PATH "Path to NumPy lib folder")
    	include_directories(${NUMPY_INCLUDE_DIR})
    	link_directories(${NUMPY_LIB_DIR})
    ELSE() #assuming other version is 2.7
    	SET(NUMPY_INCLUDE_DIR "C:/Python27/Lib/site-packages/numpy/core/include" CACHE PATH "Path to NumPy include folder")
		SET(NUMPY_LIB_DIR "C:/Python27/Lib/site-packages/numpy/lib" CACHE PATH "Path to NumPy lib folder")
		include_directories(${NUMPY_INCLUDE_DIR})
		link_directories(${NUMPY_LIB_DIR})
    ENDIF()
ENDIF()

find_package(Threads REQUIRED)

IF(UNIX)
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
ENDIF(UNIX)

IF(DEFINED ENV{COMPILE_CORE})
    set(COMPILE_CORE "$ENV{COMPILE_CORE}" CACHE BOOL "compile core?" FORCE)
ELSE()
    set(COMPILE_CORE ON CACHE BOOL "compile core?")
ENDIF()
IF(DEFINED ENV{COMPILE_SIMULATOR})
    set(COMPILE_SIMULATOR "$ENV{COMPILE_SIMULATOR}" CACHE BOOL "compile simulator?" FORCE)
ELSE()
    set(COMPILE_SIMULATOR ON CACHE BOOL "compile simulator?")
ENDIF()
IF(DEFINED ENV{COMPILE_SPS_IMAGE})
    set(COMPILE_SPS_IMAGE "$ENV{COMPILE_SPS_IMAGE}" CACHE BOOL "compile sps_image?" FORCE)
ELSE()
    IF (WIN32)
        set(COMPILE_SPS_IMAGE ON CACHE BOOL "compile sps_image?")
    ELSE()
        set(COMPILE_SPS_IMAGE ON CACHE BOOL "compile sps_image?")
    ENDIF()
ENDIF()

#CAMERA WORKING ON LINUX AND WINDOWS
#BASLER
IF(DEFINED ENV{COMPILE_BASLER})
    set(COMPILE_BASLER "$ENV{COMPILE_BASLER}" CACHE BOOL "compile basler?" FORCE)
ELSE()
    set(COMPILE_BASLER OFF CACHE BOOL "compile basler?")
ENDIF()
####
#ANDOR
IF(DEFINED ENV{COMPILE_ANDOR})
	set(COMPILE_ANDOR "$ENV{COMPILE_ANDOR}" CACHE BOOL "compile andor?" FORCE)
ELSE()
	set(COMPILE_ANDOR OFF CACHE BOOL "compile andor?")
ENDIF()
####
#ROPERSCIENTIFIC
IF(DEFINED ENV{COMPILE_ROPERSCIENTIFIC})
	set(COMPILE_ROPERSCIENTIFIC "$ENV{COMPILE_ROPERSCIENTIFIC}" CACHE BOOL "compile roperscientific ?" FORCE)
ELSE()
	set(COMPILE_ROPERSCIENTIFIC OFF CACHE BOOL "compile roperscientific?")
ENDIF()
####
#PCO
IF(DEFINED ENV{COMPILE_PCO})
	set(COMPILE_PCO "$ENV{COMPILE_PCO}" CACHE BOOL "compile pco?" FORCE)
ELSE()
	set(COMPILE_PCO OFF CACHE BOOL "compile pco?")
ENDIF()
####

#CAMERA ONLY WORKING UNDER LINUX
IF(UNIX)
		#PROSILICA
		IF(DEFINED ENV{COMPILE_PROSILICA})
			set(COMPILE_PROSILICA "$ENV{COMPILE_PROSILICA}" CACHE BOOL "compile prosilica?" FORCE)
		ELSE()
			set(COMPILE_PROSILICA OFF CACHE BOOL "compile prosilica?")
		ENDIF()
		####
		
		#ANDOR3
		IF(DEFINED ENV{COMPILE_ANDOR3})
			set(COMPILE_ANDOR3 "$ENV{COMPILE_ANDOR3}" CACHE BOOL "compile andor3?" FORCE)
		ELSE()
			set(COMPILE_ANDOR3 OFF CACHE BOOL "compile andor3?")
		ENDIF()
		####

		#ADSC
		IF(DEFINED ENV{COMPILE_ADSC})
			set(COMPILE_ADSC "$ENV{COMPILE_ADSC}" CACHE BOOL "compile adsc?" FORCE)
		ELSE()
			set(COMPILE_ADSC OFF CACHE BOOL "compile adsc?")
		ENDIF()
		####

		#FRELON
		IF(DEFINED ENV{COMPILE_FRELON})
			set(COMPILE_FRELON "$ENV{COMPILE_FRELON}" CACHE BOOL "compile frelon?" FORCE)
		ELSE()
			set(COMPILE_FRELON OFF CACHE BOOL "compile frelon?")
		ENDIF()
		####

		#ESPIA
		IF(DEFINED ENV{COMPILE_ESPIA})
			set(COMPILE_ESPIA "$ENV{COMPILE_ESPIA}" CACHE BOOL "compile espia?" FORCE)
		ELSE()
			set(COMPILE_ESPIA OFF CACHE BOOL "compile espia?")
		ENDIF()
		####

		#EIGER
		IF(DEFINED ENV{COMPILE_EIGER})
			set(COMPILE_EIGER "$ENV{COMPILE_EIGER}" CACHE BOOL "compile eiger?" FORCE)
		ELSE()
			set(COMPILE_EIGER OFF CACHE BOOL "compile eiger?")
		ENDIF()
		####

		#PILATUS
		IF(DEFINED ENV{COMPILE_PILATUS})
			set(COMPILE_PILATUS "$ENV{COMPILE_PILATUS}" CACHE BOOL "compile pilatus?" FORCE)
		ELSE()
			set(COMPILE_PILATUS OFF CACHE BOOL "compile pilatus?")
		ENDIF()
		####

		#IMXPAD
		IF(DEFINED ENV{COMPILE_IMXPAD})
			set(COMPILE_IMXPAD "$ENV{COMPILE_IMXPAD}" CACHE BOOL "compile imxpad?" FORCE)
		ELSE()
			set(COMPILE_IMXPAD OFF CACHE BOOL "compile imxpad?")
		ENDIF()
		####

		#MERLIN
		IF(DEFINED ENV{COMPILE_MERLIN})
			set(COMPILE_MERLIN "$ENV{COMPILE_MERLIN}" CACHE BOOL "compile merlin?" FORCE)
		ELSE()
			set(COMPILE_MERLIN OFF CACHE BOOL "compile merlin?")
		ENDIF()
		####

		#PIXIRAD
		IF(DEFINED ENV{COMPILE_PIXIRAD})
			set(COMPILE_PIXIRAD "$ENV{COMPILE_PIXIRAD}" CACHE BOOL "compile pixirad?" FORCE)
		ELSE()
			set(COMPILE_PIXIRAD OFF CACHE BOOL "compile pixirad?")
		ENDIF()
		####

		#MARCCD
		IF(DEFINED ENV{COMPILE_MARCCD})
			set(COMPILE_MARCCD "$ENV{COMPILE_MARCCD}" CACHE BOOL "compile marccd?" FORCE)
		ELSE()
			set(COMPILE_MARCCD OFF CACHE BOOL "compile marccd?")
		ENDIF()
		####

		#POINTGREY
		IF(DEFINED ENV{COMPILE_POINTGREY})
			set(COMPILE_POINTGREY "$ENV{COMPILE_POINTGREY}" CACHE BOOL "compile pointgrey?" FORCE)
		ELSE()
			set(COMPILE_POINTGREY OFF CACHE BOOL "compile pointgrey?")
		ENDIF()
		####

		#RAYONIX HS
		IF(DEFINED ENV{COMPILE_RAYONIXHS})
			set(COMPILE_RAYONIXHS "$ENV{COMPILE_RAYONIXHS}" CACHE BOOL "compile rayonix hs?" FORCE)
		ELSE()
			set(COMPILE_RAYONIXHS OFF CACHE BOOL "compile rayonix hs?")
		ENDIF()
		####COMPILE_UEYE

		#UEYE
		IF(DEFINED ENV{COMPILE_UEYE})
			set(COMPILE_UEYE "$ENV{COMPILE_UEYE}" CACHE BOOL "compile ueye ?" FORCE)
		ELSE()
			set(COMPILE_UEYE OFF CACHE BOOL "compile ueye ?")
		ENDIF()
		####

		#ULTRA
		IF(DEFINED ENV{COMPILE_ULTRA})
			set(COMPILE_ULTRA "$ENV{COMPILE_ULTRA}" CACHE BOOL "compile ultra ?" FORCE)
		ELSE()
			set(COMPILE_ULTRA OFF CACHE BOOL "compile ultra ?")
		ENDIF()
		####

		#V4L2
		IF(DEFINED ENV{COMPILE_V4L2})
			set(COMPILE_V4L2 "$ENV{COMPILE_V4L2}" CACHE BOOL "compile v4l2 ?" FORCE)
		ELSE()
			set(COMPILE_V4L2 OFF CACHE BOOL "compile v4l2 ?")
		ENDIF()
		####

		#XPAD
		IF(DEFINED ENV{COMPILE_XPAD})
			set(COMPILE_XPAD "$ENV{COMPILE_XPAD}" CACHE BOOL "compile Xpad ?" FORCE)
		ELSE()
			set(COMPILE_XPAD OFF CACHE BOOL "compile Xpad ?")
		ENDIF()
		####

		#XSPRESS3
		IF(DEFINED ENV{COMPILE_XSPRESS3})
			set(COMPILE_XSPRESS3 "$ENV{COMPILE_XSPRESS3}" CACHE BOOL "compile xspress3 ?" FORCE)
		ELSE()
			set(COMPILE_XSPRESS3 OFF CACHE BOOL "compile xspress3 ?")
		ENDIF()
		####

		#XH
		IF(DEFINED ENV{COMPILE_XH})
			set(COMPILE_XH "$ENV{COMPILE_XH}" CACHE BOOL "compile xh ?" FORCE)
		ELSE()
			set(COMPILE_XH OFF CACHE BOOL "compile xh ?")
		ENDIF()
		####
		
		#DEXELA
		IF(DEFINED ENV{COMPILE_DEXELA})
			set(COMPILE_DEXELA "$ENV{COMPILE_DEXELA}" CACHE BOOL "compile dexela ?" FORCE)
		ELSE()
			set(COMPILE_DEXELA OFF CACHE BOOL "compile dexela ?")
		ENDIF()
		####
		
		#MYTHEN
		IF(DEFINED ENV{COMPILE_MYTHEN})
			set(COMPILE_MYTHEN "$ENV{COMPILE_MYTHEN}" CACHE BOOL "compile mythen ?" FORCE)
		ELSE()
			set(COMPILE_MYTHEN OFF CACHE BOOL "compile mythen ?")
		ENDIF()
		####
		
		#MYTHEN3
		IF(DEFINED ENV{COMPILE_MYTHEN3})
			set(COMPILE_MYTHEN3 "$ENV{COMPILE_MYTHEN3}" CACHE BOOL "compile mythen3 ?" FORCE)
		ELSE()
			set(COMPILE_MYTHEN3 OFF CACHE BOOL "compile mythen3 ?")
		ENDIF()
		####
	ENDIF()

#CAMERA ONLY WORKING UNDER WINDOWS
IF(WIN32)
		#PERKIN ELMER
		IF(DEFINED ENV{COMPILE_PERKINELMER})
			set(COMPILE_PERKINELMER "$ENV{COMPILE_PERKINELMER}" CACHE BOOL "compile perkin elmer ?" FORCE)
		ELSE()
			set(COMPILE_PERKINELMER OFF CACHE BOOL "compile perkin elmer ?")
		ENDIF()
		####
ENDIF()


IF(DEFINED ENV{COMPILE_SIP})
    set(COMPILE_SIP "$ENV{COMPILE_SIP}" CACHE BOOL "compile python modules?" FORCE)
ELSE()
    set(COMPILE_SIP ON CACHE BOOL "compile python modules?")
ENDIF()
IF(DEFINED ENV{COMPILE_TIFF_SAVING})
    set(COMPILE_TIFF_SAVING "$ENV{COMPILE_TIFF_SAVING}" CACHE BOOL "compile tiff saving code?" FORCE)
ELSE()
    set(COMPILE_TIFF_SAVING ON CACHE BOOL "compile tiff saving code?")
ENDIF()

if(COMPILE_SIP)
    IF(!${SIP_FOUND})
        MESSAGE(FATAL_ERROR "sip executable not found, cannot build python extensions")
    ENDIF()
    if (WIN32)
        set(SIP_TAGS WIN32_PLATFORM)
    elseif(UNIX)
        set(SIP_TAGS POSIX_PLATFORM)
    endif(WIN32)
    set(SIP_EXTRA_OPTIONS -e -g)
endif(COMPILE_SIP)

add_subdirectory(third-party/Processlib)
if(WIN32)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/core/include/WindowSpecific")
endif(WIN32)

set(common_srcs common/src/Constants.cpp common/src/SizeUtils.cpp common/src/Timestamp.cpp common/src/ThreadUtils.cpp
                common/src/Exceptions.cpp common/src/MemUtils.cpp common/src/RegExUtils.cpp common/src/AcqState.cpp
                common/src/Debug.cpp common/src/VideoUtils.cpp common/src/Event.cpp common/src/Timer.cpp
                common/src/DirectoryEventUtils.cpp)
#if (0 and COMPILE_CONFIG)
#    set(config_objs common/src/ConfigUtils.cpp)
#endif (0 and COMPILE_CONFIG)

if(WIN32)
    list(REMOVE_ITEM common_srcs common/src/RegExUtils.cpp)
endif()

set(hw_srcs hardware/src/HwInterface.cpp hardware/src/HwCap.cpp hardware/src/HwSyncCtrlObj.cpp hardware/src/HwFrameInfo.cpp
            hardware/src/HwFrameCallback.cpp hardware/src/HwBufferCtrlObj.cpp hardware/src/HwBufferMgr.cpp
            hardware/src/HwShutterCtrlObj.cpp hardware/src/HwMaxImageSizeCallback.cpp hardware/src/HwDetInfoCtrlObj.cpp
            hardware/src/HwBinCtrlObj.cpp hardware/src/HwRoiCtrlObj.cpp hardware/src/HwFlipCtrlObj.cpp hardware/src/HwSerialLine.cpp
            hardware/src/HwBufferSave.cpp hardware/src/HwVideoCtrlObj.cpp hardware/src/HwEventCtrlObj.cpp hardware/src/HwSavingCtrlObj.cpp
            hardware/src/HwFileEventMgr.cpp hardware/src/HwReconstructionCtrlObj.cpp)

if(WIN32)
    list(REMOVE_ITEM hw_srcs hardware/src/HwFileEventMgr.cpp)
endif()

set(control_srcs control/src/CtSaving.cpp control/src/CtControl.cpp control/src/CtAcquisition.cpp control/src/CtBuffer.cpp
                 control/src/CtImage.cpp control/src/CtSaving_Edf.cpp control/src/CtShutter.cpp control/src/CtAccumulation.cpp control/src/CtVideo.cpp
                 control/src/CtEvent.cpp)
set(ct_srcs control/software_operation/src/SoftOpInternalMgr.cpp control/software_operation/src/SoftOpExternalMgr.cpp control/software_operation/src/SoftOpId.cpp)

if(COMPILE_SPS_IMAGE)
    add_definitions(-DWITH_SPS_IMAGE)
    set(ct_srcs ${ct_srcs} control/src/CtSpsImage.cpp third-party/Sps/Src/sps.c)
endif()
if(COMPILE_TIFF_SAVING)
    add_definitions(-DWITH_TIFF_SAVING)
    set(ct_srcs ${ct_srcs} control/src/CtSaving_Tiff.cpp)
endif()

file(STRINGS "common/VERSION" core_ver)
add_library(limacore SHARED
    ${common_srcs}
    ${hw_srcs}
    ${control_srcs}
    ${ct_srcs}
)
TARGET_INCLUDE_DIRECTORIES(limacore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/common/include")
TARGET_INCLUDE_DIRECTORIES(limacore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/hardware/include")
TARGET_INCLUDE_DIRECTORIES(limacore PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/control/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/include")
TARGET_INCLUDE_DIRECTORIES(limacore PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/core/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/tasks/include")
if(COMPILE_SPS_IMAGE)
    target_include_directories(limacore PRIVATE "${CMAKE_SOURCE_DIR}/third-party/Sps/Include")
endif()
if(COMPILE_TIFF_SAVING)
    target_link_libraries(limacore -ltiff)
endif()
if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(PUBLIC limacore "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(limacore "${CMAKE_THREAD_LIBS_INIT}")
endif()
if(UNIX)
    TARGET_LINK_LIBRARIES(limacore "rt")
endif()
TARGET_LINK_LIBRARIES(limacore processlib)
if(WIN32)
	TARGET_COMPILE_DEFINITIONS(limacore PRIVATE LIMACORE_EXPORTS)
endif()
SET_TARGET_PROPERTIES(limacore PROPERTIES VERSION ${core_ver} SOVERSION "1")
if(WIN32)
    SET_TARGET_PROPERTIES(limacore PROPERTIES PREFIX "lib")
endif()
if(WIN32)
    INSTALL(TARGETS limacore
            DESTINATION lib
            PUBLIC_HEADER DESTINATION include)
else()
    INSTALL(TARGETS limacore
            LIBRARY DESTINATION lib
            PUBLIC_HEADER DESTINATION include)
endif()

if(COMPILE_SIMULATOR)
    add_subdirectory(camera/simulator)
endif(COMPILE_SIMULATOR)

#CAMERA WORKING UNDER LINUX AND WINDOWS
#BASLER
if(COMPILE_BASLER)
    add_subdirectory(camera/basler)
endif(COMPILE_BASLER)
###########
#ANDOR
if(COMPILE_ANDOR)
	add_subdirectory(camera/andor)
endif(COMPILE_ANDOR)
###########
#ROPERSCIENTIFIC
if(COMPILE_ROPERSCIENTIFIC)
	add_subdirectory(camera/roperscientific)
endif(COMPILE_ROPERSCIENTIFIC)
###########
#PCO
if(COMPILE_PCO)
	add_subdirectory(camera/pco)
endif(COMPILE_PCO)
###########

#CAMERA ONLY WORKING UNDER LINUX
IF(UNIX)
	#PROSILICA
	if(COMPILE_PROSILICA)
		add_subdirectory(camera/prosilica)
	endif(COMPILE_PROSILICA)
	###########
	#ANDOR3
	if(COMPILE_ANDOR3)
		add_subdirectory(camera/andor3)
	endif(COMPILE_ANDOR3)
	###########
	#ADSC
	if(COMPILE_ADSC)
		add_subdirectory(camera/adsc)
	endif(COMPILE_ADSC)
	###########
	#AVIEX
	if(COMPILE_AVIEX)
		add_subdirectory(camera/aviex)
	endif(COMPILE_AVIEX)
	###########
	#FRELON
	if(COMPILE_FRELON)
		add_subdirectory(camera/frelon)
	endif(COMPILE_FRELON)
	###########
	#EIGER
	if(COMPILE_EIGER)
		add_subdirectory(camera/eiger)
	endif(COMPILE_EIGER)
	###########
	#PILATUS
	if(COMPILE_PILATUS)
		add_subdirectory(camera/pilatus)
	endif(COMPILE_PILATUS)
	###########
	#IMXPAD
	if(COMPILE_IMXPAD)
		add_subdirectory(camera/imxpad)
	endif(COMPILE_IMXPAD)
	###########
	#MERLIN
	if(COMPILE_MERLIN)
		add_subdirectory(camera/merlin)
	endif(COMPILE_MERLIN)
	###########
	#PIXIRAD
	if(COMPILE_PIXIRAD)
		add_subdirectory(camera/pixirad)
	endif(COMPILE_PIXIRAD)
	###########
	#MARCCD
	if(COMPILE_MARCCD)
		add_subdirectory(camera/marccd)
	endif(COMPILE_MARCCD)
	###########
	#POINTGREY
	if(COMPILE_POINTGREY)
		add_subdirectory(camera/pointgrey)
	endif(COMPILE_POINTGREY)
	###########
	#RAYONIX HS
	if(COMPILE_RAYONIXHS)
		add_subdirectory(camera/rayonixhs)
	endif(COMPILE_RAYONIXHS)
	###########
	#UEYE
	if(COMPILE_UEYE)
		add_subdirectory(camera/ueye)
	endif(COMPILE_UEYE)
	###########
	#ULTRA
	if(COMPILE_ULTRA)
		add_subdirectory(camera/ultra)
	endif(COMPILE_ULTRA)
	###########
	#V4L2
	if(COMPILE_V4L2)
		add_subdirectory(camera/v4l2)
	endif(COMPILE_V4L2)
	###########
	#XPAD
	if(COMPILE_XPAD)
		add_subdirectory(camera/xpad)
	endif(COMPILE_XPAD)
	###########
	#XSPRESS3
	if(COMPILE_XSPRESS3)
		add_subdirectory(camera/xspress3)
	endif(COMPILE_XSPRESS3)
	###########
	#XH
	if(COMPILE_XH)
		add_subdirectory(camera/xh)
	endif(COMPILE_XH)
	###########
	#DEXELA
	if(COMPILE_DEXELA)
		add_subdirectory(camera/dexela)
	endif(COMPILE_DEXELA)
	###########
	#MYTHEN
	if(COMPILE_MYTHEN)
		add_subdirectory(camera/mythen)
	endif(COMPILE_MYTHEN)
	###########
	#MYTHEN3
	if(COMPILE_MYTHEN3)
		add_subdirectory(camera/mythen3)
	endif(COMPILE_MYTHEN3)
	###########
ENDIF()

#CAMERA ONLY WORKING UNDER WINDOWS
IF(WIN32)
	#PERKIN ELMER
	if(COMPILE_PERKINELMER)
		add_subdirectory(camera/perkinelmer)
	endif(COMPILE_PERKINELMER)
	###########
ENDIF()

if(COMPILE_SIP)
    set(NAME "core")
    set(INCLUDES)
    FILE(GLOB sipfiles
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/common/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/common/sip/*.sip"
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/hardware/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/hardware/sip/*.sip"
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/control/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/control/sip/*.sip"
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/sip/*.sip")
    FOREACH(sipfile ${sipfiles})
        set(INCLUDES
"${INCLUDES}
%Include ${sipfile}"
)
    ENDFOREACH()

    if(SIP_VERSION_STR VERSION_LESS "4.12")
	    configure_file(sip/core/limacore_before_4_12.sip.in sip/core/limacore.sip)
	    configure_file(hardware/sip/HwFrameInfo.sip_before_4_12 sip/core/HwFrameInfo.sip)
    else()
	    configure_file(sip/core/limacore.sip.in sip/core/limacore.sip)
	    configure_file(hardware/sip/HwFrameInfo.sip_after_4_12 sip/core/HwFrameInfo.sip)
    endif()
    set(SIP_CONCAT_PARTS 1)
    set(SIP_INCLUDES ${SIP_INCLUDES}
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/tasks/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/common/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/hardware/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/control/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/sip")
    set(SIP_DISABLE_FEATURES WITH_CONFIG)
    set(SIP_CHECK_EXC ON)
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/sip/core/limacore.sip PROPERTIES OBJECT_DEPENDS ${sipfiles} sip/core/HwFrameInfo.sip)
    ADD_SIP_PYTHON_MODULE(limacore ${CMAKE_CURRENT_BINARY_DIR}/sip/core/limacore.sip)
    #ADD_SIP_PYTHON_MODULE(limacore sip/core/limacore.sip)
    TARGET_INCLUDE_DIRECTORIES(python_module_limacore PRIVATE
        ${PYTHON_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/sip
        ${CMAKE_CURRENT_SOURCE_DIR}/sip/core
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/sip)
    TARGET_LINK_LIBRARIES(python_module_limacore limacore)

    INSTALL(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima")
    INSTALL(FILES common/python/__init__.py common/python/Debug.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Core")
endif(COMPILE_SIP)
