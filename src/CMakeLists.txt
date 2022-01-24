
set( SOURCES 
	ngui.c

)

set( HEADERS
	ngui.h
)


# Set the directory for the headers, otherwise CMake can't find them
set(SOURCES_HEADERS "")
foreach(HEADER ${HEADERS})
	list(APPEND SOURCES_HEADERS ../include/ngui/${HEADER})
endforeach()


add_library( ngui ${SOURCES} ${SOURCES_HEADERS} )
target_include_directories( ngui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )
target_link_libraries( ngui glfw3 glad )