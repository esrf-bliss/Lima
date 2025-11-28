# Macros for SIP
# ~~~~~~~~~~~~~~
# Copyright (c) 2007, Simon Edwards <simon@simonzone.com>
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# SIP website: http://www.riverbankcomputing.co.uk/sip/index.php
#
# This file defines the following macros:
#
# ADD_SIP_PYTHON_MODULE (MODULE_NAME MODULE_SIP [library1, libaray2, ...])
#     Specifies a SIP file to be built into a Python module and installed.
#     MODULE_NAME is the name of Python module including any path name. (e.g.
#     os.sys, Foo.bar etc). MODULE_SIP the path and filename of the .sip file
#     to process and compile. libraryN are libraries that the Python module,
#     which is typically a shared library, should be linked to. The built
#     module will also be install into Python's site-packages directory.
#
# The behaviour of the ADD_SIP_PYTHON_MODULE macro can be controlled by a
# number of variables:
#
# SIP_INCLUDE_DIRS - List of directories which SIP will scan through when looking
#     for included .sip files. (Corresponds to the -I option for SIP.)
#
# SIP_TAGS - List of tags to define when running SIP. (Corresponds to the -t
#     option for SIP.)
#
# SIP_CONCAT_PARTS - An integer which defines the number of parts the C++ code
#     of each module should be split into. Defaults to 8. (Corresponds to the
#     -j option for SIP.)
#
# SIP_DISABLE_FEATURES - List of feature names which should be disabled
#     running SIP. (Corresponds to the -x option for SIP.)
#
# SIP_EXTRA_OPTIONS - Extra command line options which should be passed on to
#     SIP.

find_package(Python3 COMPONENTS Interpreter)

# See https://itk.org/Bug/view.php?id=12265
get_filename_component(_SIPMACRO_LIST_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

set(SIP_INCLUDE_DIRS)
set(SIP_TAGS)
set(SIP_CONCAT_PARTS 16)
set(SIP_DISABLE_FEATURES)
set(SIP_EXTRA_OPTIONS)

macro(ADD_SIP_PYTHON_MODULE MODULE_NAME MODULE_SIP RUN_CHECK_SIP_EXC)

    set(EXTRA_LINK_LIBRARIES ${ARGN})

    string(REPLACE "." "/" _x ${MODULE_NAME})
    get_filename_component(_parent_module_path ${_x}  PATH)
    get_filename_component(_child_module_name ${_x} NAME)

    get_filename_component(_module_path ${MODULE_SIP} PATH)
    get_filename_component(_abs_module_sip ${MODULE_SIP} ABSOLUTE)

    # We give this target a long logical target name.
    # (This is to avoid having the library name clash with any already
    # install library names. If that happens then cmake dependancy
    # tracking get confused.)
    string(REPLACE "." "_" _logical_name ${MODULE_NAME})
    set(_logical_name "python_module_${_logical_name}")

    file(MAKE_DIRECTORY ${_module_path})    # Output goes in this dir.

    set(_sip_include_dirs)
    foreach (_inc ${SIP_INCLUDE_DIRS})
        get_filename_component(_abs_inc ${_inc} ABSOLUTE)
        list(APPEND _sip_include_dirs -I ${_abs_inc})
    endforeach (_inc )

    set(_sip_tags)
    foreach (_tag ${SIP_TAGS})
        list(APPEND _sip_tags -t ${_tag})
    endforeach (_tag)

    set(_sip_x)
    foreach (_x ${SIP_DISABLE_FEATURES})
        list(APPEND _sip_x -x ${_x})
    endforeach (_x ${SIP_DISABLE_FEATURES})

    # Set in the pyproject.toml
    set(_build_path ${PROJECT_SOURCE_DIR}/build-sip)
    message("_build_path: ${_build_path}")

    set(_message "Generating CPP code for module ${MODULE_NAME}")
    set(_sip_output_files)
    foreach(CONCAT_NUM RANGE 0 ${SIP_CONCAT_PARTS} )
        if( ${CONCAT_NUM} LESS ${SIP_CONCAT_PARTS} )
            set(_sip_output_files ${_sip_output_files} ${_build_path}/${_child_module_name}/sip${_child_module_name}part${CONCAT_NUM}.cpp )
        endif( ${CONCAT_NUM} LESS ${SIP_CONCAT_PARTS} )
    endforeach(CONCAT_NUM RANGE 0 ${SIP_CONCAT_PARTS} )

    message("command: ${SIP_BUILD_EXECUTABLE} --no-protected-is-public --pep484-pyi --no-compile --concatenate ${SIP_CONCAT_PARTS} ${SIP_BUILD_EXTRA_OPTIONS}") 
    message("_sip_output_files: ${_sip_output_files}")

    list(APPEND _sip_module_files sip_array.c sip_core.c sip_descriptors.c sip_enum.c sip_int_convertors.c sip_object_map.c sip_threads.c sip_voidptr.c)

    foreach(src ${_sip_module_files})
        list(APPEND _sip_output_files ${_build_path}/${_child_module_name}/${src})
    endforeach()

    add_custom_command(
        OUTPUT ${_sip_output_files}
        COMMAND ${CMAKE_COMMAND} -E echo ${_message}
        COMMAND ${CMAKE_COMMAND} -E touch ${_sip_output_files}
        COMMAND ${SIP_BUILD_EXECUTABLE} --no-protected-is-public --pep484-pyi --no-compile --concatenate ${SIP_CONCAT_PARTS} ${SIP_BUILD_EXTRA_OPTIONS}
        DEPENDS ${_abs_module_sip} ${SIP_EXTRA_FILES_DEPEND}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )

    if (${RUN_CHECK_SIP_EXC})
    add_custom_command(
        OUTPUT ${_sip_output_files}
        COMMAND ${Python3_EXECUTABLE} "${_SIPMACRO_LIST_DIR}/checksipexc.py" ${_sip_include_dirs} ${_sip_output_files}
        COMMENT "Running checksipexc.py"
        APPEND
    )
    endif ()

    # Add the import numpy compilation unit
    # See https://docs.scipy.org/doc/numpy/reference/c-api.array.html#importing-the-api
    configure_file(${_SIPMACRO_LIST_DIR}/sip_init_numpy.cpp.in sip/sip_init_numpy.cpp)
    list(APPEND _sip_output_files "${CMAKE_CURRENT_BINARY_DIR}/sip/sip_init_numpy.cpp")

    # not sure if type MODULE could be uses anywhere, limit to cygwin for now
    if (CYGWIN)
        add_library(${_logical_name} MODULE ${_sip_output_files} )
    else (CYGWIN)
        add_library(${_logical_name} SHARED ${_sip_output_files} )
    endif (CYGWIN)
    target_link_libraries(${_logical_name} PRIVATE ${Python3_LIBRARY})
    target_link_libraries(${_logical_name} PRIVATE ${EXTRA_LINK_LIBRARIES})
    set_target_properties(${_logical_name} PROPERTIES PREFIX "" OUTPUT_NAME ${_child_module_name})

    if (WIN32)
      set_target_properties(${_logical_name} PROPERTIES SUFFIX ".pyd")
    endif (WIN32)

    install(TARGETS ${_logical_name}
      LIBRARY DESTINATION "${Python3_SITEARCH}/${_parent_module_path}"
      RUNTIME DESTINATION "${Python3_SITEARCH}/${_parent_module_path}"
    )

endmacro(ADD_SIP_PYTHON_MODULE)
