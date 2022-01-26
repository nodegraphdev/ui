#include "ngui.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// don't want this to be visible to the user
extern void ng_render_tree_init_();

int ng_init()
{
	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Window Title", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (!window)
		return 1;

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		return 1;

	ng_render_tree_init_();

	// For now don't do the render loop
	/*
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.87f, 0.20f, 0.20f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}
	*/


	glfwTerminate();

	return 0;
}
