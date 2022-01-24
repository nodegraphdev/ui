

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}../include )
add_executable( example main.c )
target_link_libraries( example ngui )