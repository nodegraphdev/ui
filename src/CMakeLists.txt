
set( SOURCES 
	ngui.c
	ng_math.c
	ng_render_tree.c
	ng_properties.c
	ng_ustack.h
	ng_ustack.c
)

set( HEADERS
	ngui.h
	ng_math.h
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
target_link_libraries( ngui ${GLFW_LIB_NAME} glad )

if(UNIX)
	target_link_libraries( ngui dl )
endif()
