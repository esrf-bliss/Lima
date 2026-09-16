include(FindPackageHandleStandardArgs)

find_path(OPENJPH_INCLUDE_DIRS
  NAMES ojph_version.h
  HINTS ${OPENJPH_ROOT_DIR}
  PATH_SUFFIXES openjph
  DOC "OpenJPH include directory")

find_library(OPENJPH_LIBRARY
  NAMES openjph openjp2
  HINTS ${OPENJPH_ROOT_DIR}
  DOC "OpenJPH library")

if (OPENJPH_LIBRARY)
    get_filename_component(OPENJPH_LIBRARY_DIR ${OPENJPH_LIBRARY} PATH)
endif()

mark_as_advanced(OPENJPH_INCLUDE_DIRS OPENJPH_LIBRARY_DIR OPENJPH_LIBRARY)

find_package_handle_standard_args(OpenJPH REQUIRED_VARS OPENJPH_INCLUDE_DIRS OPENJPH_LIBRARY)

if(OPENJPH_FOUND)
    if(NOT TARGET OpenJPH::OpenJPH)
        add_library(OpenJPH::OpenJPH SHARED IMPORTED)
    endif()
    if(OPENJPH_INCLUDE_DIRS)
        set_target_properties(OpenJPH::OpenJPH PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${OPENJPH_INCLUDE_DIRS}")
    endif()
    if(EXISTS "${OPENJPH_LIBRARY}")
        set_target_properties(OpenJPH::OpenJPH PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${OPENJPH_LIBRARY}")
    endif()
endif()
