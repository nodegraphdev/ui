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


# On Linux, we can just install packages. On Windows, we need somewhere for them to live
if(WIN32)
link_directories( thirdparty/win/lib )
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win/bin")
include_directories( thirdparty/win/include )
add_compile_definitions( YY_NO_UNISTD_H=1 )
endif()



# Thirdparty libraries
add_subdirectory( thirdparty/glad )

# Required for shaders
add_subdirectory( rescompile )

# NGUI Library
add_subdirectory( src )
target_include_directories( ngui PRIVATE include/ngui )



# Examples
add_subdirectory( examples )
target_include_directories( example PRIVATE include )