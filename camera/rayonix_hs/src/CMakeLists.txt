cmake_minimum_required(VERSION 2.8)
project(RayonixHsLimaPlugin)

set (BUILD_SHARED_LIBS true)

set(ROOT_DIR ${CMAKE_SOURCE_DIR}/..)
set(LIMA_DIR ${ROOT_DIR}/../..)

set(BOOST_INCLUDE /opt/rayonix/include)
set(LIMA_INCLUDE ${LIMA_DIR}/hardware/include ${LIMA_DIR}/common/include)
set(LIMA_LIB ${LIMA_DIR}/build ${LIMA_DIR}/third-party/Processlib/build)
set(RAYONIXHS_LIB ${ROOT_DIR}/lib64)
set(RAYONIXHS_INCLUDE ${ROOT_DIR}/include ${ROOT_DIR}/include/craydl)

################################################################################################################################################################################
set(ALL_INCLUDES ${BOOST_INCLUDE} ${LIMA_INCLUDE} ${RAYONIXHS_INCLUDE})
set(ALL_LIBS ${RAYONIXHS_LIB} ${LIMA_LIB})

#separate_arguments(ALL_INCLUDES_SEP UNIX_COMMAND "${ALL_INCLUDES}")
#message(ALL_INCLUDES: ${ALL_INCLUDES_SEP})
#separate_arguments(ALL_LIBS_SEP UNIX_COMMAND "${ALL_LIBS}")
#message(ALL_LIBS: ${ALL_LIBS_SEP})

include_directories(${ALL_INCLUDES})
link_directories(${ALL_LIBS})

set(CPP_FILES RayonixHsBufferCtrlObj.cpp RayonixHsCamera.cpp  RayonixHsDetInfoCtrlObj.cpp  RayonixHsInterface.cpp
	      RayonixHsSyncCtrlObj.cpp FrameStatusCb.cpp RayonixHsBinCtrlObj.cpp)

add_library(RayonixHs ${CPP_FILES})

find_library(LimaLib limacore ${ALL_LIBS})
if (NOT EXISTS ${LimaLib})
   message( "Could not find limacore lima library!")
endif (NOT EXISTS ${LimaLib})

find_library(ProcessLib processlib ${ALL_LIBS})
if (NOT EXISTS ${ProcessLib})
   message( "Could not find processlib library!")
endif (NOT EXISTS ${ProcessLib})

find_library(SpecInstLib SiVCamDll ${ALL_LIBS})
if (NOT EXISTS ${SpecInstLib})
   message( "Could not find SpecInst SiVCamDll library!")
endif (NOT EXISTS ${SpecInstLib})

find_library(craydlLib craydl ${ALL_LIBS})
if (NOT EXISTS ${craydlLib})
   message( "Could not find craydl library!")
endif (NOT EXISTS ${craydlLib})

find_library(craydlGpuLib craydl_gpu ${ALL_LIBS})
if (NOT EXISTS ${craydlGpuLib})
   message( "Could not find craydl_gpu library!")
endif (NOT EXISTS ${craydlGpuLib})

find_library(cbfLib cbf ${ALL_LIBS})
if (NOT EXISTS ${cbfLib})
   message( "Could not find CBF library!")
endif (NOT EXISTS ${cbfLib})

#set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})
#cmake_policy(PUSH)
#set(MARCCD_USE_STATIC_LIBS        ON)
#find_package(MarCCD)
#cmake_policy(POP)
#if (MARCCD_FOUND)
#   if (MARCCD_USE_STATIC_LIBS)
#      set (EXTERNAL_STATIC_LIBRARIES ${EXTERNAL_STATIC_LIBRARIES}
#      ${MARCCD_LIBRARIES})
#   else()
#      set (EXTERNAL_SHARED_LIBRARIES ${EXTERNAL_SHARED_LIBRARIES}
#      ${MARCCD_LIBRARIES})
#   endif()
#else()
#   message( "Could not find marccd libraries!")
#endif()

include(FindOpenSSL)
find_package(OpenSSL)
if (NOT OPENSSL_FOUND)
   message( "Could not find openssl libraries!")
endif ()

add_executable(test test.cpp)
#target_link_libraries(test RayonixHs ${LimaLib} ${ProcessLib} ${SpecInstLib} ${craydlLib} ${craydlGpuLib} ${MARCCD_LIBRARIES} ${cbfLib} ${OPENSSL_LIBRARIES} gsl dl)
target_link_libraries(test RayonixHs ${LimaLib} ${ProcessLib} ${SpecInstLib} ${craydlLib} ${craydlGpuLib} ${cbfLib} ${OPENSSL_LIBRARIES} gsl dl)

