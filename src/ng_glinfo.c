#include "ng_glinfo.h"

#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


void ng_glinfo_dump(enum NG_GLINFO_DUMP info_level)
{

	puts("------------------------------------");

	// Graphics Card Info
	if (info_level >= NG_GLINFO_DUMP_ADAPTER)
	{
		const GLubyte* vendor   = glGetString(GL_VENDOR);
		const GLubyte* renderer = glGetString(GL_RENDERER);
		const GLubyte* version  = glGetString(GL_VERSION);
		const GLubyte* glslver  = glGetString(GL_SHADING_LANGUAGE_VERSION);
		printf("Vendor:          %s\n", vendor);
		printf("Renderer:        %s\n", renderer);
		printf("Version:         %s\n", version);
		printf("GLSL Version:    %s\n", glslver);

	}


	// Context Info
	if (info_level >= NG_GLINFO_DUMP_CONTEXT)
	{
		// Context Profile
		int context_profile = 0;
		glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &context_profile);

		const char* profile = "UNKNOWN??";
		if (context_profile & GL_CONTEXT_CORE_PROFILE_BIT)
			profile = "Core";
		if (context_profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
			profile = "Compatibility";
		printf("Context profile: %s (0x%04X)\n", profile, context_profile);


		// Context Flags
		int context_flags = 0;
		glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);

		printf("Context Flags:   0x%08X\n", context_flags);
		if (context_flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)
			puts("\tForward compatible");
		if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT)
			puts("\tDebug");
		//if (context_flags & GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT)
		//	puts("\Robust access");
		//if (context_flags & GL_CONTEXT_FLAG_NO_ERROR_BIT)
		//	puts("\tNo error reporting");
	}


	// GL Extensions
	if (info_level >= NG_GLINFO_DUMP_EXTENSIONS)
	{
		int num_ext = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &num_ext);

		puts("Extensions:");
		for (int i = 0; i < num_ext; i++)
		{
			const GLubyte* ext = glGetStringi(GL_EXTENSIONS, i);
			putchar('\t');
			puts(ext);
		}
	}


	puts("------------------------------------");

}


static void ng_glinfo_logcallback_(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	const char* source_str = "_";
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		source_str = "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		source_str = "Window System";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		source_str = "Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		source_str = "Thirdparty";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		source_str = "Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		source_str = "Other";
		break;
	}

	const char* type_str = "_";
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		type_str = "Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		type_str = "Deprecated Behavior";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		type_str = "Undefined Behavior";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		type_str = "Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		type_str = "Performance";
		break;
	case GL_DEBUG_TYPE_MARKER:
		type_str = "Marker";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		type_str = "Push Group";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		type_str = "Pop Group";
		break;
	case GL_DEBUG_TYPE_OTHER:
		type_str = "Other";
		break;
	}

	const char* severity_str = "_";
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_LOW:
		severity_str = "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		severity_str = "MED";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		severity_str = "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		severity_str = "NOTIF";
		break;
	}


	printf("[%s] %s: %s %u - %s\n", severity_str, type_str, source_str, id, message);
}

void ng_glinfo_debug_enable()
{
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
}

void ng_glinfo_debug_attach_logger()
{
	// Are we in debug mode?
	int context_flags = 0;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
	if (!(context_flags & GL_CONTEXT_FLAG_DEBUG_BIT))
	{
		puts("Cannot attach the debug logger outside of debug mode!\n\tPlease call 'ng_glinfo_debug_enable' before the GL context is created!");
		return;
	}

	// Attach our callback
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(ng_glinfo_logcallback_, NULL);

	// Accept all msgs
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

	// Send an error so we can see if our logger works
	glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Super rad fake error!");
}