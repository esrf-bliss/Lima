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

if(UNIX)
	SET(COMMON_PATH "/usr/local" CACHE PATH "path for lib and include used for several cameras")
	SET(YAT_ROOT "/usr/local/yat" CACHE PATH "temporary path for yat include and lib used by few cameras")
	SET(OPT_ROOT "/opt" CACHE PATH "path for /opt directory")
endif()
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
FIND_PACKAGE(PythonInterp 2.6)
FIND_PACKAGE(PythonLibs 2.6)
IF (${PYTHONINTERP_FOUND})
    # python site-packages folder
    execute_process(
       COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print get_python_lib()"
       OUTPUT_VARIABLE _PYTHON_SITE_PACKAGES_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
ENDIF()
FIND_PACKAGE(SIP)
INCLUDE(SIPMacros)

SET(PYTHON_SITE_PACKAGES_DIR ${_PYTHON_SITE_PACKAGES_DIR} CACHE PATH "where should python modules be installed?")

IF (WIN32)
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


IF(DEFINED ENV{COMPILE_SPS_IMAGE})
    set(COMPILE_SPS_IMAGE "$ENV{COMPILE_SPS_IMAGE}" CACHE BOOL "compile sps_image?" FORCE)
ELSE()
    IF (WIN32)
        set(COMPILE_SPS_IMAGE ON CACHE BOOL "compile sps_image?")
    ELSE()
        set(COMPILE_SPS_IMAGE ON CACHE BOOL "compile sps_image?")
    ENDIF()
ENDIF()

#TEST COMPILATION
if(DEFINED ENV{COMPILE_TEST})
	set(COMPILE_TEST "$ENV{COMPILE_TEST}" CACHE BOOL "compile test directories ?" FORCE)
else()
	set(COMPILE_TEST OFF CACHE BOOL "compile test directories ?")
endif()

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
    TARGET_INCLUDE_DIRECTORIES(python_module_limacore PRIVATE
        ${PYTHON_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/sip
        ${CMAKE_CURRENT_SOURCE_DIR}/sip/core
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/sip)
    TARGET_LINK_LIBRARIES(python_module_limacore limacore)

    INSTALL(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima")
    INSTALL(FILES common/python/__init__.py common/python/Debug.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Core")
endif(COMPILE_SIP)

#TESTS, run ctest or make test
if(COMPILE_TEST)
    enable_testing()
endif()

# CAMERAS
INCLUDE(CameraList)

# PACKAGES, run cpack or make package
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "ESRF/BLISS") #required
SET(CPACK_PACKAGE_CONTACT "lima@esrf.f")
INCLUDE(CPack)
