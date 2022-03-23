
# Private source files kept in /src/
set( SOURCES 
	ngui.c

	ng_math.c
	ng_render_tree.c
	ng_properties.c
	ng_ustack.h
	ng_ustack.c
	
	ng_render.h
	ng_render.c
	ng_shader.h
	ng_shader.c
	ng_vertex.h
	ng_vertex.c
	ng_painter.c
	ng_texture.h
	ng_texture.c
	ng_font.h
	ng_font.c

	ng_flex.c
	ng_properties.h
	ng_window.h
)

# Public headers kept in /include/ngui/
set( HEADERS_PUBLIC
	ngui.h
	ng_math.h
	ng_painter.h
)

set( SHADERS 

	shaders/ng_painter.fs
	shaders/ng_painter.vs

	shaders/ng_font.fs
	shaders/ng_font.vs

)


# Append the directory to our public files, otherwise CMake can't find them
set(HEADERS_PUBLIC_PATH "")
foreach(PUBLIC_FILE ${HEADERS_PUBLIC})
	list(APPEND HEADERS_PUBLIC_PATH ../include/ngui/${PUBLIC_FILE})
endforeach()

if(UNIX)
	set(GLFW_LIB_NAME glfw)
else()
	set(GLFW_LIB_NAME glfw3)
endif()


add_library( ngui ${SOURCES} ${HEADERS_PUBLIC_PATH})
target_include_directories( ngui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )
target_link_libraries( ngui ${GLFW_LIB_NAME} glad freetype ngui_shaders )

if(UNIX)
	target_link_libraries( ngui dl m )
	target_include_directories( ngui PRIVATE /usr/include/freetype2/ )
endif()


# Shader Resource Library
add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ng_all_shaders.c"
    COMMAND rescompile "${CMAKE_CURRENT_BINARY_DIR}/ng_all_shaders.c" ${SHADERS}
    DEPENDS ${SHADERS}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_library( ngui_shaders STATIC ${SHADERS} "${CMAKE_CURRENT_BINARY_DIR}/ng_all_shaders.c")
