find_path(LZ4_INCLUDE_DIRS NAMES lz4.h)
find_library(LZ4_LIBRARIES NAMES lz4 liblz4)

# We require LZ4_compress_default() which was added in v1.8.2
if (LZ4_LIBRARIES)
  include(CheckCSourceRuns)
  set(CMAKE_REQUIRED_INCLUDES ${LZ4_INCLUDE_DIRS})
  set(CMAKE_REQUIRED_LIBRARIES ${LZ4_LIBRARY})
  check_c_source_runs("
#include <lz4.h>
int main() {
  int good = (LZ4_VERSION_MAJOR > 1) ||
    ((LZ4_VERSION_MAJOR == 1) && (LZ4_VERSION_MINOR >= 8) && (LZ4_VERSION_RELEASE >= 2));
return !good;
}" LZ4_GOOD_VERSION)
  set(CMAKE_REQUIRED_INCLUDES)
  set(CMAKE_REQUIRED_LIBRARIES)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LZ4
  DEFAULT_MSG
  LZ4_LIBRARIES
  LZ4_INCLUDE_DIRS
  LZ4_GOOD_VERSION)

mark_as_advanced(LZ4_INCLUDE_DIRS LZ4_LIBRARIES)
