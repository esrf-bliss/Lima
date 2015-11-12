
cmake_minimum_required(VERSION 2.8)
project (lima)

find_package(Threads REQUIRED)

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")

set(COMPILE_CORE ON CACHE BOOL "compile core?")
set(COMPILE_SIMULATOR ON CACHE BOOL "compile simulator?")
set(COMPILE_SPS_IMAGE ON CACHE BOOL "compile sps_image?")
set(COMPILE_ESPIA ON CACHE BOOL "compile espia?")
set(COMPILE_FRELON ON CACHE BOOL "compile frelon?")
set(COMPILE_MAXIPIX ON CACHE BOOL "compile maxipix?")
set(COMPILE_PILATUS ON CACHE BOOL "compile pilatus?")
set(COMPILE_BASLER ON CACHE BOOL "compile basler?")
#set(COMPILE_CORE ON CACHE BOOL "compile core?")
#set(COMPILE_CORE ON CACHE BOOL "compile core?")
#set(COMPILE_CORE ON CACHE BOOL "compile core?")
#set(COMPILE_CORE ON CACHE BOOL "compile core?")
#set(COMPILE_CORE ON CACHE BOOL "compile core?")
#set(COMPILE_CORE ON CACHE BOOL "compile core?")
#set(COMPILE_CORE ON CACHE BOOL "compile core?")
#set(COMPILE_CORE ON CACHE BOOL "compile core?")
#set(COMPILE_CORE ON CACHE BOOL "compile core?")
#set(COMPILE_CORE ON CACHE BOOL "compile core?")
#set(COMPILE_CORE ON CACHE BOOL "compile core?")

#add_subdirectory(third-party)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/build/libprocesslib.so
    COMMAND ${MAKE}
    ARGS -C third-party/Processlib/build
)
add_custom_target(
    libprocesslib
    COMMAND make
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/build
)

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

#if (COMPILE_CORE)
#    add_subdirectory(common)
#    add_subdirectory(hardware)
#    add_subdirectory(control)
#    add_subdirectory(camera)
#    add_subdirectory(build)
#endif (COMPILE_CORE)

file(STRINGS "common/VERSION" core_ver)
add_library(limacore SHARED
    ${common_srcs}
    ${hw_srcs}
    ${control_srcs}
    ${ct_srcs}
)
TARGET_INCLUDE_DIRECTORIES(limacore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/common/include")
TARGET_INCLUDE_DIRECTORIES(limacore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/hardware/include")
TARGET_INCLUDE_DIRECTORIES(limacore PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/control/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/control/software_operation/include")
TARGET_INCLUDE_DIRECTORIES(limacore PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/core/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/tasks/include")
if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(PUBLIC limacore "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(limacore "${CMAKE_THREAD_LIBS_INIT}")
endif()
TARGET_LINK_LIBRARIES(limacore "rt")
FIND_LIBRARY(PROCESSLIB processlib "${CMAKE_CURRENT_SOURCE_DIR}/third-party/Processlib/build")
TARGET_LINK_LIBRARIES(limacore ${PROCESSLIB})
ADD_DEPENDENCIES(limacore libprocesslib)
SET_TARGET_PROPERTIES(limacore PROPERTIES VERSION ${core_ver} SOVERSION "1")

add_subdirectory(camera/simulator)
add_subdirectory(camera/basler)
