# Locate the pulse library
#
# This module defines the following variables:
#
# PULSE_LIBRARY the name of the library;
# PULSE_INCLUDE_DIR where to find glfw include files.
# PULSE_FOUND true if both the PULSE_LIBRARY and PULSE_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called PULSE_ROOT which points to the root of the glfw library
# installation.
#
# default search dirs
# 
# Cmake file from: https://github.com/daw42/glslcookbook

set( _pulse_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/include"
"C:/Program Files (x86)/glfw/include" )
set( _pulse_LIB_SEARCH_DIRS
"/usr/lib"
"/usr/local/lib"
"${CMAKE_SOURCE_DIR}/lib"
"C:/Program Files (x86)/glfw/lib-msvc110" )

# Check environment for root search directory
set( _pulse_ENV_ROOT $ENV{PULSE_ROOT} )
if( NOT PULSE_ROOT AND _pulse_ENV_ROOT )
	set(PULSE_ROOT ${_pulse_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( PULSE_ROOT )
	list( INSERT _pulse_HEADER_SEARCH_DIRS 0 "${PULSE_ROOT}/include" )
	list( INSERT _pulse_LIB_SEARCH_DIRS 0 "${PULSE_ROOT}/lib" )
endif()

# Search for the header
FIND_PATH(PULSE_INCLUDE_DIR "pulse/pulseaudio.h"
PATHS ${_pulse_HEADER_SEARCH_DIRS} )

# Search for the library
FIND_LIBRARY(PULSE_LIBRARY NAMES pulse pulseaudio
PATHS ${_pulse_LIB_SEARCH_DIRS} )
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLFW3 DEFAULT_MSG
PULSE_LIBRARY PULSE_INCLUDE_DIR)
