
cmake_minimum_required(VERSION 2.8)
project (lima)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(Threads REQUIRED)

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")

set(COMPILE_CORE ON CACHE BOOL "compile core?")
set(COMPILE_SIMULATOR ON CACHE BOOL "compile simulator?")
set(COMPILE_SPS_IMAGE ON CACHE BOOL "compile sps_image?")
set(COMPILE_BASLER ON CACHE BOOL "compile basler?")
set(COMPILE_SIP OFF CACHE BOOL "compile python modules?")

if(COMPILE_SIP)
    FIND_PACKAGE(PythonInterp 2.7 REQUIRED)
    FIND_PACKAGE(PythonLibs 2.7 REQUIRED)
    # python site-packages folder
    execute_process(
       COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print get_python_lib()"
       OUTPUT_VARIABLE PYTHON_SITE_PACKAGES_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    FIND_PACKAGE(SIP REQUIRED)
    INCLUDE(SIPMacros)
    if (WIN32)
        set(SIP_TAGS WIN32_PLATFORM)
    elseif(UNIX)
        set(SIP_TAGS POSIX_PLATFORM)
    endif(WIN32)
    set(SIP_EXTRA_OPTIONS -e -g)
endif(COMPILE_SIP)

add_subdirectory(third-party/Processlib)

set(common_srcs common/src/Constants.cpp common/src/SizeUtils.cpp common/src/Timestamp.cpp common/src/ThreadUtils.cpp
                common/src/Exceptions.cpp common/src/MemUtils.cpp common/src/RegExUtils.cpp common/src/AcqState.cpp
                common/src/Debug.cpp common/src/VideoUtils.cpp common/src/Event.cpp common/src/Timer.cpp
                common/src/DirectoryEventUtils.cpp)
#if (0 and COMPILE_CONFIG)
#    set(config_objs common/src/ConfigUtils.cpp)
#endif (0 and COMPILE_CONFIG)

set(hw_srcs hardware/src/HwInterface.cpp hardware/src/HwCap.cpp hardware/src/HwSyncCtrlObj.cpp hardware/src/HwFrameInfo.cpp
            hardware/src/HwFrameCallback.cpp hardware/src/HwBufferCtrlObj.cpp hardware/src/HwBufferMgr.cpp
            hardware/src/HwShutterCtrlObj.cpp hardware/src/HwMaxImageSizeCallback.cpp hardware/src/HwDetInfoCtrlObj.cpp
            hardware/src/HwBinCtrlObj.cpp hardware/src/HwRoiCtrlObj.cpp hardware/src/HwFlipCtrlObj.cpp hardware/src/HwSerialLine.cpp
            hardware/src/HwBufferSave.cpp hardware/src/HwVideoCtrlObj.cpp hardware/src/HwEventCtrlObj.cpp hardware/src/HwSavingCtrlObj.cpp
            hardware/src/HwFileEventMgr.cpp hardware/src/HwReconstructionCtrlObj.cpp)

set(control_srcs control/src/CtSaving.cpp control/src/CtControl.cpp control/src/CtAcquisition.cpp control/src/CtBuffer.cpp
                 control/src/CtImage.cpp control/src/CtSaving_Edf.cpp control/src/CtShutter.cpp control/src/CtAccumulation.cpp control/src/CtVideo.cpp
                 control/src/CtEvent.cpp)
set(ct_srcs control/software_operation/src/SoftOpInternalMgr.cpp control/software_operation/src/SoftOpExternalMgr.cpp control/software_operation/src/SoftOpId.cpp)

if(COMPILE_SPS_IMAGE)
    add_definitions(-DWITH_SPS_IMAGE)
    set(ct_srcs ${ct_srcs} control/src/CtSpsImage.cpp third-party/Sps/Src/sps.c)
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
if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(PUBLIC limacore "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(limacore "${CMAKE_THREAD_LIBS_INIT}")
endif()
TARGET_LINK_LIBRARIES(limacore "rt")
TARGET_LINK_LIBRARIES(limacore processlib)
SET_TARGET_PROPERTIES(limacore PROPERTIES VERSION ${core_ver} SOVERSION "1")
INSTALL(TARGETS limacore
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include)

if(COMPILE_SIMULATOR)
    add_subdirectory(camera/simulator)
endif(COMPILE_SIMULATOR)
if(COMPILE_BASLER)
    add_subdirectory(camera/basler)
endif(COMPILE_BASLER)

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

    configure_file(sip/core/limacore.sip.in sip/core/limacore.sip)
    set(SIP_CONCAT_PARTS 1)
    set(SIP_INCLUDES ${SIP_INCLUDES}
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/tasks/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/common/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/hardware/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/control/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/sip")
    set(SIP_DISABLE_FEATURES WITH_CONFIG)
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/sip/core/limacore.sip PROPERTIES OBJECT_DEPENDS ${sipfiles})
    ADD_SIP_PYTHON_MODULE(limacore ${CMAKE_CURRENT_BINARY_DIR}/sip/core/limacore.sip)
    #ADD_SIP_PYTHON_MODULE(limacore sip/core/limacore.sip)
    TARGET_INCLUDE_DIRECTORIES(python_module_limacore PRIVATE
        ${PYTHON_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/sip
        ${CMAKE_CURRENT_SOURCE_DIR}/sip/core
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/sip)
    TARGET_LINK_LIBRARIES(python_module_limacore limacore)
endif(COMPILE_SIP)
