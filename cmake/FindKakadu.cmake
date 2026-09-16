include(FindPackageHandleStandardArgs)

find_path(KAKADU_CORESYS_INCLUDE_DIRS
  NAMES kdu_compressed.h
  PATH_SUFFIXES kakadu/coresys/common
  HINTS "${KAKADU_ROOT}"
  DOC "Kakadu coresys include directory")

find_path(KAKADU_AUX_INCLUDE_DIRS
  NAMES kdu_stripe_compressor.h
  PATH_SUFFIXES kakadu/apps/support
  HINTS "${KAKADU_ROOT}"
  DOC "Kakadu support include directory")

find_library(KAKADU_CORE_LIBRARY
  NAMES kdu_v86R kdu_v87R
  HINTS "${KAKADU_ROOT}/lib/Linux-x86-64-gcc"
  DOC "Path to Kakadu auxiliary library")

find_library(KAKADU_AUX_LIBRARY
  NAMES kdu_a86R kdu_a87R
  HINTS "${KAKADU_ROOT}/lib/Linux-x86-64-gcc"
  DOC "Path to Kakadu auxiliary library")

if (KAKADU_CORE_LIBRARY)
    get_filename_component(KAKADU_LIBRARY_DIR ${KAKADU_CORE_LIBRARY} PATH)
endif()

mark_as_advanced(KAKADU_CORESYS_INCLUDE_DIRS KAKADU_AUX_INCLUDE_DIRS KAKADU_CORE_LIBRARY KAKADU_AUX_LIBRARY)

find_package_handle_standard_args(Kakadu
  REQUIRED_VARS
  KAKADU_CORESYS_INCLUDE_DIRS
  KAKADU_AUX_INCLUDE_DIRS
  KAKADU_CORE_LIBRARY
  KAKADU_AUX_LIBRARY)

set(KAKADU_INCLUDE_DIRS ${KAKADU_CORESYS_INCLUDE_DIRS} ${KAKADU_AUX_INCLUDE_DIRS})

if(KAKADU_FOUND)
    if(NOT TARGET Kakadu::Kakadu)
        add_library(Kakadu::Kakadu SHARED IMPORTED)
    endif()
    if(KAKADU_CORESYS_INCLUDE_DIRS AND KAKADU_AUX_INCLUDE_DIRS)
        set(KAKADU_INCLUDE_DIRS ${KAKADU_CORESYS_INCLUDE_DIRS} ${KAKADU_AUX_INCLUDE_DIRS})
        set_target_properties(Kakadu::Kakadu PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${KAKADU_INCLUDE_DIRS}")
    endif()
    if(EXISTS "${KAKADU_CORE_LIBRARY}")
        set_target_properties(Kakadu::Kakadu PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${KAKADU_CORE_LIBRARY}")
    endif()
    if(EXISTS "${KAKADU_AUX_LIBRARY}")
        set_target_properties(Kakadu::Kakadu PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${KAKADU_AUX_LIBRARY}")
    endif()
endif()
