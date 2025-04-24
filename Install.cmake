# Set install dir for SIP files and CMake packages
set(SIP_INSTALL_DIR ${CMAKE_INSTALL_DATADIR}/sip/lima)
set(CMAKE_INSTALL_DIR ${CMAKE_INSTALL_DATADIR}/cmake/lima)

if(NOT LIMA_BUILD_SUBMODULES)
    # Generate and install package config file and version
    set(PROJECT_LIBRARIES limacore h5bshuf)
    include(cmake/package_config.cmake)
    include(cmake/components_config.cmake)
endif()

install(
    TARGETS limacore
    EXPORT "${TARGETS_EXPORT_NAME}"
    COMPONENT runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}   # import library
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}   # .so files are libraries
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}   # .dll files are binaries
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}  # this does not actually install anything (but used by downstream projects)
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}/project_version.h
        ${CMAKE_CURRENT_BINARY_DIR}/limacore_export.h
    COMPONENT devel
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lima
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/common/include/
    COMPONENT devel
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/hardware/include/
    COMPONENT devel
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/control/include/
    COMPONENT devel
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/control/software_operation/include/
    COMPONENT devel
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

if(NOT LIMA_BUILD_SUBMODULES)
    install(
        FILES ${CMAKE_SOURCE_DIR}/cmake/checksipexc.py
              ${CMAKE_SOURCE_DIR}/cmake/FindSIP.cmake
              ${CMAKE_SOURCE_DIR}/cmake/FindSIP.py
              ${CMAKE_SOURCE_DIR}/cmake/SIPMacros.cmake
              ${CMAKE_SOURCE_DIR}/cmake/sip_init_numpy.cpp.in
              ${CMAKE_SOURCE_DIR}/cmake/package_config.cmake
              ${CMAKE_SOURCE_DIR}/cmake/components_config.cmake
              ${CMAKE_SOURCE_DIR}/cmake/project_version.cmake
              ${CMAKE_SOURCE_DIR}/cmake/project_version.cc.in
              ${CMAKE_SOURCE_DIR}/cmake/project_version.h.in
              ${CMAKE_SOURCE_DIR}/cmake/LimaTools.cmake
        COMPONENT tools
        DESTINATION ${CMAKE_INSTALL_DIR}
    )
endif()

if(LIMA_ENABLE_PYTHON)
    install(
         DIRECTORY python/Lima/
         COMPONENT sip
         DESTINATION "$<PATH:CMAKE_PATH,NORMALIZE,${Python3_SITEARCH}/Lima>")

    file(GLOB SIP_SOURCES
        "${CMAKE_SOURCE_DIR}/common/sip/*.sip"
        "${CMAKE_SOURCE_DIR}/common/sip/*.sip"
        "${CMAKE_SOURCE_DIR}/hardware/sip/*.sip"
        "${CMAKE_SOURCE_DIR}/control/sip/*.sip"
        "${CMAKE_SOURCE_DIR}/control/software_operation/sip/*.sip")

    if(NOT LIMA_BUILD_SUBMODULES)
        install(
            FILES ${SIP_SOURCES}
                  ${CMAKE_CURRENT_BINARY_DIR}/sip/limacore.sip
                  ${CMAKE_SOURCE_DIR}/sip/limamodules.sip.in
            COMPONENT sip
            DESTINATION ${SIP_INSTALL_DIR}
        )
    endif()
endif()
