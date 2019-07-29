# This cmake code creates the configuration that is found and used by
# find_package() of another cmake project

# get lower and upper case project name for the configuration files

# configure and install the configuration files
include(CMakePackageConfigHelpers)

# Generate config for the "tools" component
configure_package_config_file(
  "${CMAKE_SOURCE_DIR}/cmake/project-tools-config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}_tools-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER}
  PATH_VARS CMAKE_INSTALL_DIR)
  
# Generate config for the "devel" component
configure_package_config_file(
  "${CMAKE_SOURCE_DIR}/cmake/project-devel-config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}_devel-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER}
  PATH_VARS SIP_INSTALL_DIR CMAKE_INSTALL_DIR)

install(FILES
  # Components
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}_devel-config.cmake"
  COMPONENT devel
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER})
install(FILES
  # Components
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}_tools-config.cmake"
  COMPONENT tools
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER})
