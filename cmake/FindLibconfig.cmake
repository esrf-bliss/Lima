find_library(LIBCONFIG_LIBRARIES NAMES config++ libconfig++)
find_path(LIBCONFIG_INCLUDE_DIRS libconfig.h++)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Libconfig DEFAULT_MSG LIBCONFIG_LIBRARIES LIBCONFIG_INCLUDE_DIRS)
