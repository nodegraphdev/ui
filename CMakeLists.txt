cmake_minimum_required( VERSION 3.0 )

# Check for an in source root build
STRING(COMPARE EQUAL ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR} insourceroot )
if(insourceroot)
message( FATAL_ERROR "\nPlease change your build directory to something like \"build\" and not the source root!\ncmake . -B build" )
endif()

# Set outputs to one spot
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/out )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/out )

project( NGUI )

# Thirdparty libraries
add_subdirectory( thirdparty/glad )


# NGUI Library
add_subdirectory( src )
target_include_directories( ngui PRIVATE include/ngui )


# On Linux, we can just install packages. On Windows, we need somewhere for them to live
if(WIN32)
target_include_directories( ngui PRIVATE thirdparty/win/include )
link_directories( thirdparty/win/lib )
endif()


# Examples
add_subdirectory( examples )
target_include_directories( example PRIVATE include )