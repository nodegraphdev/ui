
set( SOURCES 
	ngui.c

)

set( INCLUDE_DIRS


)


include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}../include )
add_library( ngui ${SOURCES} )
target_include_directories( ngui PRIVATE ${INCLUDE_DIRS} )