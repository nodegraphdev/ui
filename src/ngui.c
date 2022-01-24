#include "ngui.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int ng_init()
{
	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Window Title", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (!window)
		return 1;

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		return 1;


	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.87f, 0.20f, 0.20f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}


	glfwTerminate();

	return 0;
}