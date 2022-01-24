
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

if(UNIX)
	set(GLFW_LIB_NAME glfw)
else()
	set(GLFW_LIB_NAME glfw3)
endif()


add_library( ngui ${SOURCES} ${SOURCES_HEADERS} )
target_include_directories( ngui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )
target_link_libraries( ngui ${GLFW_LIB_NAME} glad)
