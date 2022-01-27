#include "ngui.h"
#include "ng_render.h"
#include "ng_shader.h"
#include "ng_properties.h"
#include "ng_window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


// don't want this to be visible to the user
// TODO: Throw these in private headers instead
extern void ng_render_tree_init_();
extern void ng_painter_init_();
extern void ng_painter_shutdown_();

GLFWwindow* ng_window;

int ng_init()
{

	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

	const int window_width = 640;
	const int window_height = 480;

	ng_window = glfwCreateWindow(640, 480, "Window Title", NULL, NULL);
	glfwMakeContextCurrent(ng_window);
	if (!ng_window)
		return 1;

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		return 1;

	glEnable(GL_DEPTH_TEST);


	ng_render_tree_init_();
	ng_shader_init();
	ng_painter_init_();
	ng_properties_init_();


	// Background color and clear depth
	ng_render_set_clear_colorf(0.8, 0.1, 0.1);
	ng_render_set_clear_depth(1.0);
	

	return 0;
}

void ng_shutdown()
{
	glfwTerminate();
}


int ng_open()
{
	return !glfwWindowShouldClose(ng_window);
}
void ng_begin_frame()
{
	glfwPollEvents();
	ng_render_clear_frame();

	int width = 0, height = 0;
	glfwGetWindowSize(ng_window, &width, &height);

	nmat4x4 model;
	nmat4x4 view;
	nmat4x4 proj;
	ng_identity4x4(&model);
	ng_identity4x4(&view);

	// Set the camera to fit the window size. Top left (0,0) to bottom right (width, height)
	ng_ortho4x4(&proj, 0, width, 0, height, -800, 800);

	// Update parameters
	ng_shader_set(NG_SHADERPARAM_MODEL, &model);
	ng_shader_set(NG_SHADERPARAM_VIEW, &view);
	ng_shader_set(NG_SHADERPARAM_PROJECTION, &proj);

	ng_render_setviewport(0, 0, width, height);

}
void ng_end_frame()
{
	glfwSwapBuffers(ng_window);
}

nvec2i ng_window_size_()
{
	int w, h;
	glfwGetWindowSize(ng_window, &w, &h);
	return (nvec2i){w, h};
}
