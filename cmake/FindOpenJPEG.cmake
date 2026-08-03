include(FindPackageHandleStandardArgs)

find_path(OPENJPEG_INCLUDE_DIRS
  NAMES openjpeg.h
  HINTS ${OPENJPEG_ROOT_DIR}
  PATH_SUFFIXES
    openjpeg
    openjpeg-1.5
    openjpeg-2.1
    openjpeg-2.3
    openjpeg-2.5
  DOC "OPENJPEG include directory")

find_library(OPENJPEG_LIBRARY
  NAMES openjpeg openjp2
  HINTS ${OPENJPEG_ROOT_DIR}
  DOC "OPENJPEG library")

if (OPENJPEG_LIBRARY)
    get_filename_component(OPENJPEG_LIBRARY_DIR ${OPENJPEG_LIBRARY} PATH)
endif()

mark_as_advanced(OPENJPEG_INCLUDE_DIRS OPENJPEG_LIBRARY_DIR OPENJPEG_LIBRARY)

find_package_handle_standard_args(OpenJPEG REQUIRED_VARS OPENJPEG_INCLUDE_DIRS OPENJPEG_LIBRARY)

if(OPENJPEG_FOUND)
    if(NOT TARGET OpenJPEG::OpenJPEG)
        add_library(OpenJPEG::OpenJPEG SHARED IMPORTED)
    endif()
    if(OPENJPEG_INCLUDE_DIRS)
        set_target_properties(OpenJPEG::OpenJPEG PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${OPENJPEG_INCLUDE_DIRS}")
    endif()
    if(EXISTS "${OPENJPEG_LIBRARY}")
        set_target_properties(OpenJPEG::OpenJPEG PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${OPENJPEG_LIBRARY}")
    endif()
endif()
