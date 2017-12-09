﻿#include "Application.h"

#undef GL_VERSION_1_1

#include <gl/gl3w.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <istream>
#include <fstream>
#include <iostream>

application* application::app = nullptr;

application::application(const int width, const int height, const char* title)
{
	app = this;

	if (!glfwInit())
	{
		std::cerr << "Cannot initialize GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}

	init_app_params(width, height, title);

	create_window();
	
	glfwMakeContextCurrent(window_);
	glfwSwapInterval(1);

	init_callbacks();

	if (gl3wInit())
	{
		std::cerr << "Cannot initialize gl3w" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	if (!gl3wIsSupported(appinfo_.major_version, appinfo_.minor_version))
	{
		std::cerr << "Initialize fail: OpenGL " << appinfo_.major_version << "." << appinfo_.minor_version << "unsupported" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	std::cout << "Initialize successful: OpenGL " << glGetString(GL_VERSION) << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

application::~application()
{
	glfwDestroyWindow(window_);
	glfwTerminate();
}

void application::run() const
{
	app->start();

	do
	{
		app->render();

		glfwSwapBuffers(window_);
		glfwPollEvents();
	}
	while (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_RELEASE &&
		   glfwWindowShouldClose(window_) != GL_TRUE);

	app->finish();
}

GLuint application::load_shader(const char* file_name, const GLenum type)
{
	char * shader_code  = new char[8192];
	memset(shader_code, 0, 8192);

	std::ifstream shader_file(file_name);
	
	if (shader_file.is_open()) 
	{
		const std::streamoff size = shader_file.seekg(0, std::ios::end).tellg();
		shader_file.seekg(0);
		shader_file.read(shader_code, size);
		shader_file.close();
	}

	const GLuint shader = glCreateShader(type);

	if (!shader) {
		std::cout << "Fail shader allocation" << std::endl;
		return 0;
	}

	glShaderSource(shader, 1, &shader_code, nullptr);

	delete[] shader_code;

	glCompileShader(shader);

	GLint compilation_status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_status);

	if (!compilation_status)
	{
		std::cout << "Shader compilation error" <<std::endl;
		
		char buffer[8192];
		glGetShaderInfoLog(shader, 8192, nullptr, buffer);
		std::cout << buffer << std::endl;

		glDeleteShader(shader);
	}

	return shader;
}

void application::init_app_params(const int width, const int height, const char* title)
{
	appinfo_.window_width = width;
	appinfo_.window_height = height;

	appinfo_.major_version = 4;
	appinfo_.minor_version = 6;

	appinfo_.window_title = title;
}

void application::init_callbacks() const
{
	glfwSetWindowSizeCallback(window_, window_on_resize_handler);
	glfwSetKeyCallback(window_, window_on_key_handler);
	glfwSetMouseButtonCallback(window_, window_on_mouse_button_handler);
	glfwSetCursorPosCallback(window_, window_on_mouse_move_handler);
}

void application::create_window()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, appinfo_.major_version);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, appinfo_.minor_version);

	window_ = glfwCreateWindow(appinfo_.window_width, appinfo_.window_height, appinfo_.window_title, nullptr, nullptr);
	if (!window_)
	{
		std::cerr << "Cannot initialize GLFW Window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
}

void application::window_on_key_handler(GLFWwindow* window, const int key, const int action, int scancode, int mods)
{
	app->on_key(key, action);
}

void application::window_on_resize_handler(GLFWwindow* window, const int width, const int height)
{
	app->on_resize(width, height);
}

void application::window_on_mouse_button_handler(GLFWwindow* window, const int button, const int action, int mode)
{
	app->on_mouse_button(button, action);
}

void application::window_on_mouse_move_handler(GLFWwindow* window, const double coord_x, const double coord_y)
{
	app->on_mouse_move(static_cast<int>(coord_x), static_cast<int>(coord_y));
}