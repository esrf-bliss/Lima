if(NOT LIMA_BUILD_SUBMODULES)
    # Generate and install package config file and version
    set(PROJECT_LIBRARIES limacore h5bshuf)
    set(SIP_INSTALL_DIR ${CMAKE_INSTALL_DATADIR}/sip/lima)
    set(CMAKE_INSTALL_DIR ${CMAKE_INSTALL_DATADIR}/cmake/lima)
    include(cmake/package_config.cmake)
endif()

install(
    TARGETS limacore
    EXPORT "${TARGETS_EXPORT_NAME}"
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}   # import library
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}   # .so files are libraries
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}   # .dll files are binaries
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}  # this does not actually install anything (but used by downstream projects)
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/project_version.h
        ${CMAKE_CURRENT_BINARY_DIR}/limacore_export.h
    COMPONENT devel
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lima
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/common/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/hardware/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/control/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/control/software_operation/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(
  FILES ${CMAKE_SOURCE_DIR}/cmake/checksipexc.py
        ${CMAKE_SOURCE_DIR}/cmake/FindNumPy.cmake
        ${CMAKE_SOURCE_DIR}/cmake/FindSIP.cmake
        ${CMAKE_SOURCE_DIR}/cmake/FindSIP.py
        ${CMAKE_SOURCE_DIR}/cmake/SIPMacros.cmake
        ${CMAKE_SOURCE_DIR}/cmake/package_config.cmake
        ${CMAKE_SOURCE_DIR}/cmake/project_version.cmake
        ${CMAKE_SOURCE_DIR}/cmake/project_version.cc.in
        ${CMAKE_SOURCE_DIR}/cmake/project_version.h.in
        ${CMAKE_SOURCE_DIR}/cmake/LimaTools.cmake
  COMPONENT devel
  DESTINATION ${CMAKE_INSTALL_DIR}
)

if(LIMA_ENABLE_PYTHON)
    install(DIRECTORY python/Lima/ DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima")

    file(GLOB SIP_SOURCES
        "${CMAKE_SOURCE_DIR}/common/sip/*.sip"
        "${CMAKE_SOURCE_DIR}/common/sip/*.sip"
        "${CMAKE_SOURCE_DIR}/hardware/sip/*.sip"
        "${CMAKE_SOURCE_DIR}/control/sip/*.sip"
        "${CMAKE_SOURCE_DIR}/control/software_operation/sip/*.sip")

    install(
        FILES ${SIP_SOURCES}
              ${CMAKE_CURRENT_BINARY_DIR}/sip/limacore.sip
              ${CMAKE_SOURCE_DIR}/sip/limamodules.sip.in
        COMPONENT devel
        DESTINATION ${SIP_INSTALL_DIR}
    )
endif()
