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

cmake_minimum_required(VERSION 2.8)
project (lima)

set(NAME "core")

#--------------------------------------------------------------------------------
# cmake/ directory contains tools 
#--------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(LimaTools)

find_package(Threads REQUIRED)


if(UNIX)
#--------------------------------------------------------------------------------
# Spec-like shared memory
#--------------------------------------------------------------------------------
  if(DEFINED ENV{COMPILE_SPS_IMAGE})
    set(LIMA_ENABLE_SPS_IMAGE "$ENV{COMPILE_SPS_IMAGE}" CACHE BOOL "compile sps_image?" FORCE)
  else()
    option(LIMA_ENABLE_SPS_IMAGE "compile sps_image?" OFF)
  endif()
  
# openGL real-time display
  if(DEFINED ENV{COMPILE_GLDISPLAY})
    set(LIMA_ENABLE_GLDISPLAY "$ENV{COMPILE_GLDISPLAY}" CACHE BOOL "compile gldisplay?" FORCE)
  else()
    option(LIMA_ENABLE_GLDISPLAY "compile sps_image?" OFF)
  endif()
endif()

#--------------------------------------------------------------------------------
#Compile tests
#--------------------------------------------------------------------------------
option(LIMA_ENABLE_TESTS "compile test directories ?" OFF)

#--------------------------------------------------------------------------------
# libconfig
#--------------------------------------------------------------------------------
if(DEFINED ENV{LIMA_ENABLE_CONFIG})
    set(LIMA_ENABLE_CONFIG "$ENV{LIMA_ENABLE_CONFIG}" CACHE BOOL "compile with libconfig code?" FORCE)
else()
    option(LIMA_ENABLE_CONFIG "compile with libconfig?" OFF)
endif()
      
#--------------------------------------------------------------------------------
# Saving formats can be enabled from environment variables using config.inc file
#--------------------------------------------------------------------------------
if(UNIX)
	# The formats have not been tested on windows
	if(DEFINED ENV{LIMA_ENABLE_CBF})
		set(LIMA_ENABLE_CBF "$ENV{LIMA_ENABLE_CBF}" CACHE BOOL "compile CBF saving code?" FORCE)
	else()
		option(LIMA_ENABLE_CBF "compile CBF saving code?" OFF)
	endif()
	if(DEFINED ENV{LIMA_ENABLE_EDFLZ4})
		set(LIMA_ENABLE_EDFLZ4 "$ENV{LIMA_ENABLE_EDFLZ4}" CACHE BOOL "compile EDF.LZ4 saving code?" FORCE)
	else()
		option(LIMA_ENABLE_EDFLZ4 "compile EDF.LZ4 saving code?" OFF)
	endif()
	if(DEFINED ENV{LIMA_ENABLE_FITS})
		set(LIMA_ENABLE_FITS "$ENV{LIMA_ENABLE_FITS}" CACHE BOOL "compile FITS saving code?" FORCE)
	else()
		option(LIMA_ENABLE_FITS "compile FITS saving code?" OFF)
	endif()
	if(DEFINED ENV{LIMA_ENABLE_NXS})
		set(LIMA_ENABLE_NXS "$ENV{LIMA_ENABLE_NXS}" CACHE BOOL "compile Nexus saving code?" FORCE)
	else()
		option(LIMA_ENABLE_NXS "compile Nexus saving code?" OFF) 
	endif()
	if(DEFINED ENV{LIMA_ENABLE_TIFF})
		set(LIMA_ENABLE_TIFF "$ENV{LIMA_ENABLE_TIFF}" CACHE BOOL "compile TIFF saving code?" FORCE)
	else()
		option(LIMA_ENABLE_TIFF "compile TIFF saving code?" OFF)
	endif()
endif()

if(DEFINED ENV{LIMA_ENABLE_EDFGZ})
    set(LIMA_ENABLE_EDFGZ "$ENV{LIMA_ENABLE_EDFGZ}" CACHE BOOL "compile EDF.GZ saving code?" FORCE)
else()
    option(LIMA_ENABLE_EDFGZ "compile EDF.GZ saving code?" OFF)
endif()
if(DEFINED ENV{LIMA_ENABLE_HDF5})
    set(LIMA_ENABLE_HDF5 "$ENV{LIMA_ENABLE_HDF5}" CACHE BOOL "compile HDF5 saving code?" FORCE)
else()
    option(LIMA_ENABLE_HDF5 "compile HDF5 saving code?" OFF)
endif()
if(DEFINED ENV{LIMA_ENABLE_HDF5_BS})
    set(LIMA_ENABLE_HDF5_BS "$ENV{LIMA_ENABLE_HDF5_BS}" CACHE BOOL "compile HDF5/BS saving code?" FORCE)
else()
    option(LIMA_ENABLE_HDF5_BS "compile HDF5/BS saving code?" OFF)
endif()

# Compile python wrapping code generated using SIP
IF(DEFINED ENV{LIMA_ENABLE_PYTHON})
    set(LIMA_ENABLE_PYTHON "$ENV{LIMA_ENABLE_PYTHON}" CACHE BOOL "compile python modules?" FORCE)
else()
    option(LIMA_ENABLE_PYTHON "compile python modules?" OFF)
endif()

# Python Tango server
IF(DEFINED ENV{LIMA_ENABLE_PYTANGO_SERVER})
    set(LIMA_ENABLE_PYTANGO_SERVER "$ENV{LIMA_ENABLE_PYTANGO_SEVER}" CACHE BOOL "install python tango server?" FORCE)
else()
    option(LIMA_ENABLE_PYTANGO_SERVER "install python tango server?" OFF)
endif()
#--------------------------------------------------------------------------------
# Enable python binding code compilation using sip generator
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_PYTHON)
    find_package(PythonInterp 2.7)
    find_package(PythonLibs 2.7)
    if(${PYTHONINTERP_FOUND})
      # python site-packages folder
      execute_process(
	      COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print (get_python_lib())"
	OUTPUT_VARIABLE _PYTHON_SITE_PACKAGES_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
	else()
		message(FATAL_ERROR "No python found, please install or disable LIMA_ENABLE_PYTHON")
    endif()
    find_package(SIP)
	if(${SIP_FOUND})
		include(SIPMacros)
	else()
		message(FATAL_ERROR "python SIP not installed, please install or disable LIMA_ENABLE_PYTHON")
	endif()

    set(PYTHON_SITE_PACKAGES_DIR ${_PYTHON_SITE_PACKAGES_DIR} CACHE PATH "where should python modules be installed?")

    if(WIN32)
        set(NUMPY_INCLUDE_DIR "${_PYTHON_SITE_PACKAGES_DIR}/numpy/core/include" CACHE PATH "Path to NumPy include folder")
        set(NUMPY_LIB_DIR "${_PYTHON_SITE_PACKAGES_DIR}/numpy/core/lib" CACHE PATH "Path to NumPy lib folder")
        include_directories(${NUMPY_INCLUDE_DIR})
        link_directories(${NUMPY_LIB_DIR})
    endif()
    if(!${SIP_FOUND})
        message(FATAL_ERROR "sip executable not found, cannot build python extensions")
    endif()
    if(WIN32)
        set(SIP_TAGS WIN32_PLATFORM)
    elseif(UNIX)
        set(SIP_TAGS POSIX_PLATFORM)
    endif(WIN32)
    set(SIP_EXTRA_OPTIONS -e -g)
 endif()

#--------------------------------------------------------------------------------
#Processlib third-party is mandatory
#--------------------------------------------------------------------------------
set(PROCESSLIB_EXTERNALLY_CONFIGURED ON)
if(LIMA_ENABLE_PYTHON)
  set(PROCESSLIB_EXTERNALLY_ENABLE_PYTHON ON)
endif()
add_subdirectory(third-party/Processlib)

#--------------------------------------------------------------------------------
# Define source files
#--------------------------------------------------------------------------------
if(WIN32)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/core/include/WindowSpecific")
endif(WIN32)

set(common_srcs common/src/Constants.cpp common/src/SizeUtils.cpp common/src/Timestamp.cpp common/src/ThreadUtils.cpp
                common/src/Exceptions.cpp common/src/MemUtils.cpp common/src/RegExUtils.cpp common/src/AcqState.cpp
                common/src/Debug.cpp common/src/VideoUtils.cpp common/src/Event.cpp common/src/Timer.cpp
                common/src/DirectoryEventUtils.cpp common/src/SimplePipe.cpp)

if(WIN32)
    list(REMOVE_ITEM common_srcs common/src/RegExUtils.cpp common/src/SimplePipe.cpp)
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
                 control/src/CtImage.cpp control/src/CtSaving_Compression.cpp control/src/CtSaving_Edf.cpp control/src/CtShutter.cpp
                 control/src/CtAccumulation.cpp control/src/CtVideo.cpp
                 control/src/CtEvent.cpp)
set(ct_srcs control/software_operation/src/SoftOpInternalMgr.cpp control/software_operation/src/SoftOpExternalMgr.cpp control/software_operation/src/SoftOpId.cpp)

#--------------------------------------------------------------------------------
# Option for libconfig++ support
#--------------------------------------------------------------------------------
set(extra_libs)
set(extra_includes)
if(LIMA_ENABLE_CONFIG)
	if(WIN32)
		if (${CMAKE_GENERATOR} MATCHES "(2008|2010|2013)")
			if (${CMAKE_GENERATOR} MATCHES "Win64$")
				set(CONFIG_VERSION "win64-msvc2008")
			else()
				set(CONFIG_VERSION "win32-msvc2008")
			endif()
		elseif(${CMAKE_GENERATOR} MATCHES "2015")
			if (${CMAKE_GENERATOR} MATCHES "Win64$")
				set(CONFIG_VERSION "win64-msvc2015")
			else()
				set(CONFIG_VERSION "win32-msvc2015")
			endif()
		else()
			message(FATAL_ERROR "This compiler is not supported: " ${CMAKE_GENERATOR})
		endif()		
		set(LIB_CONFIG_INCLUDE_DIR "C:/Program Files/LIBCONFIG-windows/${CONFIG_VERSION}/include" CACHE PATH "Path to LIBCONFIG++ includes")
		set(LIB_CONFIG_DIR "C:/Program Files/LIBCONFIG-windows/${CONFIG_VERSION}/lib" CACHE PATH "Path to GSL lib folder")
		find_library(LIB_CONFIG libconfig++ ${LIB_CONFIG_DIR})
	else()
		find_library(LIB_CONFIG config++)
	endif()
	find_path(LIB_CONFIG_INCLUDE_DIR libconfig.h++)
	
	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(config DEFAULT_MSG LIB_CONFIG LIB_CONFIG_INCLUDE_DIR)
	if(${CONFIG_FOUND})
		add_definitions(-DWITH_CONFIG)
		set(extra_libs ${extra_libs} ${LIB_CONFIG})
		set(common_srcs ${common_srcs} common/src/ConfigUtils.cpp)
		set(control_srcs ${control_srcs} control/src/CtConfig.cpp)
		link_directories(${LIB_CONFIG_DIR})
		set(extra_includes ${extra_includes} ${LIB_CONFIG_INCLUDE_DIR})
	else()
		message(FATAL_ERROR "libconfig++ not found, set LIB_CONFIG path or disable LIMA_ENABLE_CONFIG")
	endif()
endif()

#--------------------------------------------------------------------------------
# Option for Spec-like shared-memory (SPS)  support
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_SPS_IMAGE)
    add_definitions(-DWITH_SPS_IMAGE)
    set(ct_srcs ${ct_srcs} control/src/CtSpsImage.cpp third-party/Sps/Src/sps.c)
endif()

#--------------------------------------------------------------------------------
# Option for openGL real-time display  support
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_GLDISPLAY)
  add_subdirectory(third-party/gldisplay)
endif()

#--------------------------------------------------------------------------------
# Option for extra saving formats edf.gz, edf.lz4, cbf, hdf5, tiff, fits ...
#--------------------------------------------------------------------------------
set(saving_libs)
set(saving_includes)
if(LIMA_ENABLE_EDFGZ)
	if(WIN32)
		if (${CMAKE_GENERATOR} MATCHES "Win64$")
			set(ZLIB_ARCH "win64")
		else()
			set(ZLIB_ARCH "win32")
		endif()	
		set(LIB_ZLIB_INCLUDE_DIR "C:/Program Files/zlib-windows/${ZLIB_ARCH}/include" CACHE PATH "Path to zlib includes")
		set(LIB_ZLIB_DIR "C:/Program Files/zlib-windows/${ZLIB_ARCH}/lib" CACHE PATH "Path to zlib libs")
		link_directories(${LIB_ZLIB_DIR})
		find_library(LIB_ZLIB zlibstatic ${LIB_ZLIB_DIR})
		add_definitions(-DZLIB_WINAPI)
	else()
	    find_library(LIB_ZLIB z)
	endif()	
	find_path(LIB_ZLIB_INCLUDE_DIR zlib.h)
	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(ZLIB DEFAULT_MSG LIB_ZLIB LIB_ZLIB_INCLUDE_DIR)
	if(${ZLIB_FOUND})
		add_definitions(-DWITH_Z_COMPRESSION)
		set(saving_libs ${saving_libs} ${LIB_ZLIB})
		set(saving_includes ${saving_includes} ${LIB_ZLIB_INCLUDE_DIR})		
	else()
		message(FATAL_ERROR "ZLIB library not found, please install or disable LIMA_ENABLE_EDFGZ")
	endif()	
endif()

if(LIMA_ENABLE_EDFLZ4)
    # look for a lz4  1.7 where lz4frame.h was added
    find_path(LIB_LZ4_INCLUDE_DIR lz4frame.h)
    find_library(LIB_LZ4 NAMES lz4)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(LZ4 DEFAULT_MSG LIB_LZ4 LIB_LZ4_INCLUDE_DIR)
    if (LZ4_FOUND)
        add_definitions(-DWITH_LZ4_COMPRESSION)
        set(saving_libs ${saving_libs} ${LIB_LZ4})
	set(saving_includes ${saving_includes} ${LIB_LZ4_INCLUDE_DIR})		
    else()
        message(FATAL_ERROR "LZ4 library: required version = 1.7.x, please update or switch off LIMA_ENABLE_EDFLZ4")
    endif()
endif()

if(LIMA_ENABLE_CBF)
	find_path(LIB_CBF_INCLUDE_DIR cbf/cbf.h)
    find_library(LIB_CBF cbf)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(CBF DEFAULT_MSG LIB_CBF LIB_CBF_INCLUDE_DIR)
	if (${CBF_FOUND})
		add_definitions(-DWITH_CBF_SAVING)
		add_definitions(-DPROTOTYPES)
		set(ct_srcs ${ct_srcs} control/src/CtSaving_Cbf.cpp)
		set(saving_libs ${saving_libs} ${LIB_CBF} crypto)
		set(saving_includes ${saving_includes} ${LIB_CBF_INCLUDE_DIR})		
	else()
		message(FATAL_ERROR "CBF library not found, please install or disable LIMA_ENABLE_CBF")
	endif()		
endif()

if(LIMA_ENABLE_FITS)
    find_path(LIB_FITS_INCLUDE_DIR CCfits/CCfits.h)
    find_library(LIB_CFITSIO cfitsio)
    find_library(LIB_CCFITS CCfits)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(FITS DEFAULT_MSG LIB_CCFITS LIB_FITS_INCLUDE_DIR)
	if(${FITS_FOUND})
		add_definitions(-DWITH_FITS_SAVING)
		set(ct_srcs ${ct_srcs} control/src/CtSaving_Fits.cpp)
		set(saving_libs ${saving_libs} ${LIB_CFITSIO} ${LIB_CCFITS})
		set(saving_includes ${saving_includes} ${LIB_FITS_INCLUDE_DIR})		
	else()
		message(FATAL_ERROR "CFITSIO and/or CCFITS library not found, please install or disable LIMA_ENABLE_FITS")	
	endif()
endif()

if(LIMA_ENABLE_HDF5)
	if(WIN32)
		if (${CMAKE_GENERATOR} MATCHES "2008")
			set(HDF5_VERSION "1.8.13")
		elseif (${CMAKE_GENERATOR} MATCHES "2015")
			set(HDF5_VERSION "1.8.19")
		else()
			message(FATAL_ERROR "Unsupported VS version for HDF5 saving format, only vs2008 or vs2015 are supported respectively for hdf5 1.8.13 and 1.8.16")
		endif()
		set(LIB_HDF5_INCLUDE_DIR "C:/Program Files/HDF_Group/HDF5/${HDF5_VERSION}/include" CACHE PATH "Path to HDF5 includes")
		set(LIB_HDF5_DIR "C:/Program Files/HDF_Group/HDF5/${HDF5_VERSION}/lib" CACHE PATH "Path to HDF5 libs")
		link_directories(${LIB_HDF5_DIR})
		# we use static libs libhdf5.lib and libhdf5_cpp.lib
		find_library(LIB_HDF5_CPP libhdf5_cpp ${LIB_HDF5_DIR})
		find_library(LIB_HDF5 libhdf5 ${LIB_HDF5_DIR})
		find_library(LIB_ZLIB libzlib ${LIB_HDF5_DIR})
		find_library(LIB_SZIP libszip ${LIB_HDF5_DIR})
	 else()
		set(LIB_HDF5_INCLUDE_DIR "/usr/local/hdf5/include" CACHE PATH "Path to HDF5 directories")
                set(LIB_HDF5_DIR "/usr/local/hdf5/lib")
		find_library(LIB_HDF5_CPP hdf5_cpp ${LIB_HDF5_DIR})
		find_library(LIB_HDF5 hdf5 ${LIB_HDF5_DIR})
		find_library(LIB_HDF5_HL hdf5_hl ${LIB_HDF5_DIR})
	        find_path(LIB_HDF5_INCLUDE_DIR H5Cpp.h)
                link_directories(${LIB_HDF5_DIR})
	endif()	
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(HDF5 DEFAULT_MSG LIB_HDF5 LIB_HDF5_INCLUDE_DIR)	
    if(${HDF5_FOUND})
		add_definitions(-DWITH_HDF5_SAVING)
		set(ct_srcs ${ct_srcs} control/src/CtSaving_Hdf5.cpp)	
		set(saving_libs ${saving_libs} ${LIB_HDF5} ${LIB_HDF5_CPP} ${LIB_HDF5_HL} ${LIB_SZIP} ${LIB_ZLIB})
		set(saving_includes ${saving_includes} ${LIB_HDF5_INCLUDE_DIR})
    else()
		message(FATAL_ERROR "hdf5_cpp library not found, please install or disable LIMA_ENABLE_HDF5")
    endif()

    if(LIMA_ENABLE_HDF5_BS)
        set(BITSHUFFLE_EXTERNALLY_CONFIGURED ON)
        add_subdirectory(control/bitshuffle)

	add_definitions(-DWITH_BS_COMPRESSION)
        set(saving_includes ${saving_includes} ${CMAKE_CURRENT_SOURCE_DIR}/third-party/bitshuffle/src)
        set(saving_libs ${saving_libs} h5bshuf)

        # set(LIB_BS_INCLUDE_DIR "/usr/local/include/" CACHE PATH "Path to BitShuffle include files")
        # link_directories(${LIB_HDF5_PLUGIN})
        # find_library(LIB_HDF5_BS h5bshuf ${LIB_HDF5_PLUGIN})
        # find_path(LIB_BS_INCLUDE_DIR bitshuffle.h)
        # set(saving_libs ${saving_libs} ${LIB_HDF5_BS})
        # set(saving_includes ${saving_includes} ${LIB_BS_INCLUDE_DIR})
    endif()
endif()

if(LIMA_ENABLE_NXS)
    find_path(LIB_NXS_INCLUDE_DIR nexuscpp/nexuscpp.h)
    find_library(LIB_NXS nexuscpp)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(NXS DEFAULT_MSG LIB_NXS LIB_NXS_INCLUDE_DIR)		
	if(${NXS_FOUND})
		add_definitions(-DWITH_NXS_SAVING)    
		set(ct_srcs ${ct_srcs} control/src/CtSaving_NXS.cpp)
		set(saving_includes ${saving_includes} ${LIB_NXS_INCLUDE_DIR})		
	else()
		message(FATAL_ERROR "NEXUS cpp library not installed, please install or disable LIMA_ENABLE_NXS")
	endif()
endif()

if(LIMA_ENABLE_TIFF)
    find_path(LIB_TIFF_INCLUDE_DIR tiffio.h)
    find_library(LIB_TIFF tiff)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(TIFF DEFAULT_MSG LIB_TIFF LIB_TIFF_INCLUDE_DIR)	
	if(${TIFF_FOUND})
		add_definitions(-DWITH_TIFF_SAVING)
		set(ct_srcs ${ct_srcs} control/src/CtSaving_Tiff.cpp)
		set(saving_libs ${saving_libs} tiff)
		set(saving_includes ${saving_includes} ${LIB_TIFF_INCLUDE_DIR})		
	else()
		message(FATAL_ERROR "TIFF library not found, please install or disable LIMA_ENABLE_TIFF")
	endif()
endif()

#--------------------------------------------------------------------------------
# Add the library limacore definition
#--------------------------------------------------------------------------------
add_library(limacore SHARED
    ${common_srcs}
    ${hw_srcs}
    ${control_srcs}
    ${ct_srcs}
)

limatools_set_library_soversion(limacore "common/VERSION")       
  
target_include_directories(limacore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/common/include")
target_include_directories(limacore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/hardware/include")
target_include_directories(limacore PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/control/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/include")
target_include_directories(limacore PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/core/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/tasks/include")
if(LIMA_ENABLE_SPS_IMAGE)
    target_include_directories(limacore PRIVATE "${CMAKE_SOURCE_DIR}/third-party/Sps/Include")
endif()

# add all include paths coming from saving format options
target_include_directories(limacore PRIVATE ${extra_includes} ${saving_includes})

if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(PUBLIC limacore "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(limacore "${CMAKE_THREAD_LIBS_INIT}")
endif()
if(UNIX)
    add_definitions(-DHAS_INOTIFY)
    target_link_libraries(limacore "rt")
endif()
target_link_libraries(limacore processlib)
target_link_libraries(limacore ${saving_libs} ${extra_libs})
if(WIN32)
    target_compile_definitions(limacore PRIVATE LIMACORE_EXPORTS)
    set_target_properties(limacore PROPERTIES PREFIX "lib")
endif()
if(WIN32)
    install(TARGETS limacore
            DESTINATION lib
            PUBLIC_HEADER DESTINATION include)
else()
    install(TARGETS limacore
            LIBRARY DESTINATION lib
            PUBLIC_HEADER DESTINATION include)
endif()


#--------------------------------------------------------------------------------
# SIP generates binding code for python
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_PYTHON)
    set(INCLUDES)
    file(GLOB sipfiles
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/common/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/common/sip/*.sip"
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/hardware/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/hardware/sip/*.sip"
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/control/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/control/sip/*.sip"
         RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/sip"
         "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/sip/*.sip")
    foreach(sipfile ${sipfiles})
        set(INCLUDES
"${INCLUDES}
%Include ${sipfile}"
)
    endforeach()

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
    if (NOT LIMA_ENABLE_CONFIG)
      set(SIP_DISABLE_FEATURES ${SIP_DISABLE_FEATURES} WITH_CONFIG)
    endif()
    if (NOT LIMA_ENABLE_SPS_IMAGE)
      set(SIP_DISABLE_FEATURES ${SIP_DISABLE_FEATURES} WITH_SPS_IMAGE)
    endif()
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/sip/core/limacore.sip PROPERTIES OBJECT_DEPENDS ${sipfiles} sip/core/HwFrameInfo.sip)
    # Run sip now !
    add_sip_python_module(limacore ${CMAKE_CURRENT_BINARY_DIR}/sip/core/limacore.sip)
    
    target_include_directories(python_module_limacore PRIVATE
        ${PYTHON_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/sip
        ${CMAKE_CURRENT_SOURCE_DIR}/sip/core
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/sip)
    target_link_libraries(python_module_limacore limacore)

    install(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima")
    install(FILES common/python/__init__.py common/python/Debug.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Core")
endif()

#--------------------------------------------------------------------------------
#TESTS, run ctest or make test
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_TESTS)
    enable_testing()
endif()

if(LIMA_ENABLE_TESTS)
        add_subdirectory(control/test)
	if (NOT WIN32)
          add_subdirectory(common/test)
	endif()
endif()

#--------------------------------------------------------------------------------
# CAMERA list is in cmake/CameraList.cmake file
#--------------------------------------------------------------------------------
include(CameraList)

#--------------------------------------------------------------------------------
# Python Tango server
#--------------------------------------------------------------------------------
if(LIMA_ENABLE_PYTANGO_SERVER)
    add_subdirectory(applications/tango/python)
endif()

#--------------------------------------------------------------------------------
# PACKAGES debian at least, run cpack or make package
#--------------------------------------------------------------------------------
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "ESRF/BLISS") #required
set(CPACK_PACKAGE_CONTACT "lima@esrf.fr")
include(CPack)
