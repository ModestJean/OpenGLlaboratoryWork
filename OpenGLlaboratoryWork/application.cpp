﻿#include "Application.h"

#undef GL_VERSION_1_1

#include <gl/gl3w.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <cstdio>

application* application::app = nullptr;

application::application(const int width, const int height, const char* title)
{
	app = this;

	if (!glfwInit())
	{
		printf("Cannot initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	init_app_params(width, height, title);

	create_window();
	
	glfwMakeContextCurrent(window_);
	glfwSwapInterval(1);

	init_callbacks();

	if (gl3wInit())
	{
		printf("Cannot initialize gl3w\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	if (!gl3wIsSupported(appinfo_.major_version, appinfo_.minor_version))
	{
		printf("Initialize fail: OpenGL %s.%s unsupported\n", appinfo_.major_version, appinfo_.minor_version);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	printf("Initialize successful: OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
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
		printf("Cannot initialize GLFW Window\n");
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