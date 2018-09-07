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
# SIP_INCLUDES - List of directories which SIP will scan through when looking
#     for included .sip files. (Corresponds to the -I option for SIP.)
#
# SIP_TAGS - List of tags to define when running SIP. (Corresponds to the -t
#     option for SIP.)
#
# SIP_DISABLE_FEATURES - List of feature names which should be disabled
#     running SIP. (Corresponds to the -x option for SIP.)
#
# SIP_EXTRA_OPTIONS - Extra command line options which should be passed on to
#     SIP.

set(SIP_INCLUDES)
set(SIP_TAGS)
set(SIP_DISABLE_FEATURES)
set(SIP_EXTRA_OPTIONS)

macro(ADD_SIP_PYTHON_MODULE MODULE_NAME MODULE_SIP)

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

    set(_sip_includes)
    foreach (_inc ${SIP_INCLUDES})
        get_filename_component(_abs_inc ${_inc} ABSOLUTE)
        list(APPEND _sip_includes -I ${_abs_inc})
    endforeach (_inc )

    set(_sip_tags)
    foreach (_tag ${SIP_TAGS})
        list(APPEND _sip_tags -t ${_tag})
    endforeach (_tag)

    set(_sip_x)
    foreach (_x ${SIP_DISABLE_FEATURES})
        list(APPEND _sip_x -x ${_x})
    endforeach (_x ${SIP_DISABLE_FEATURES})

    set(_message "-DMESSAGE=Generating CPP code for module ${MODULE_NAME}")
	
    set(_module_sbf ${_module_path}/${MODULE_NAME}.sbf)
    execute_process(COMMAND ${SIP_EXECUTABLE} ${_sip_tags} ${_sip_x}
                    ${SIP_EXTRA_OPTIONS} ${_sip_includes}
                    -b ${_module_sbf} ${_abs_module_sip}
    )

    if(${MODULE_NAME} STREQUAL "processlib")
        message(FATAL_ERROR "processlib module has its own SIPMacros")
    endif()
    set(_init_numpy "${_child_module_name}_init_numpy.cpp")
    set(_init_numpy_cpp ${_module_path}/${_init_numpy})

    set(_sip_output_files_list)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/cmake/readsipsbf.py 
                                     ${_module_sbf} ${_module_path}
        OUTPUT_VARIABLE _sip_output_files_list
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
	
    set(_sip_output_files)
    foreach(filename IN LISTS _sip_output_files_list)
        set(_sip_output_files ${_sip_output_files} ${filename})
    endforeach(filename)

    if(NOT WIN32)
        set(TOUCH_COMMAND touch)
    else(NOT WIN32)
        set(TOUCH_COMMAND echo)
        # instead of a touch command, give out the name and append to the files
        # this is basically what the touch command does.
        foreach(filename ${_sip_output_files})
            file(APPEND filename "")
        endforeach(filename ${_sip_output_files})
    endif(NOT WIN32)

    # TODO: add all SIP files with the %Include directive + Exceptions.sip

    add_custom_command(
        OUTPUT ${_sip_output_files}
        COMMAND ${CMAKE_COMMAND} -E echo ${message}
        COMMAND ${TOUCH_COMMAND} ${_sip_output_files}
        COMMAND ${SIP_EXECUTABLE} ${_sip_tags} ${_sip_x} ${SIP_EXTRA_OPTIONS}
                                  ${_sip_includes} -c ${_module_path} 
                                  ${_abs_module_sip}
#        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/cmake/checksipexc.py
#                                     ${_sip_output_files}
        DEPENDS ${_abs_module_sip} ${SIP_EXTRA_FILES_DEPEND}
    )
    # not sure if type MODULE could be uses anywhere, limit to cygwin for now
    set(_sip_all_files ${_init_numpy_cpp} ${_sip_output_files})
    if (CYGWIN)
        add_library(${_logical_name} MODULE ${_sip_all_files} )
    else (CYGWIN)
        add_library(${_logical_name} SHARED ${_sip_all_files} )
    endif (CYGWIN)
    target_link_libraries(${_logical_name} ${PYTHON_LIBRARY})
    target_link_libraries(${_logical_name} ${EXTRA_LINK_LIBRARIES})
    set_target_properties(${_logical_name} PROPERTIES
                          PREFIX "" OUTPUT_NAME ${_child_module_name}
                          LINKER_LANGUAGE CXX)

    if (WIN32)
      set_target_properties(${_logical_name} PROPERTIES SUFFIX ".pyd")
      set_target_properties(${_logical_name} PROPERTIES IMPORT_SUFFIX ".dll")
    endif (WIN32)

    install(TARGETS ${_logical_name} 
            DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/${_parent_module_path}")

endmacro(ADD_SIP_PYTHON_MODULE)
