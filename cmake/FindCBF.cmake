find_path(CBF_INCLUDE_DIRS cbflib/cbf.h)
find_library(CBF_LIBRARIES cbf)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CBF
  DEFAULT_MSG
  CBF_LIBRARIES
  CBF_INCLUDE_DIRS)
