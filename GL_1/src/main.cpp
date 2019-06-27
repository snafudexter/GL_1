#include <iostream>
#include <sstream>
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "ShaderProgram.h"

const char* APP_TITLE = "First Triangle";
const int gWindowWidth = 800;
const int gWindowHeight = 600;


GLFWwindow* gWindow = NULL;

bool gWireFrame = false;


void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void showFPS(GLFWwindow* window);

bool initOpenGL();

int main()
{
	if (!initOpenGL())
	{
		std::cerr << "OpenGL init failed" << std::endl;
	}


	//TRIANGLE STUFF
	GLfloat vertices[] = {
		-0.5f,   0.5f,  0.0f, 
		 0.5f,   0.5f,  0.0f, 
		 0.5f,  -0.5f,  0.0f,
		-0.5f,	-0.5f,	0.0f
	};

	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	GLuint vbo, ibo, vao;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//pos
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("basic.vert", "basic.frag");

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//main loop
	while (!glfwWindowShouldClose(gWindow)) {

		showFPS(gWindow);
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.use();

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glfwSwapBuffers(gWindow);
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);

	glfwTerminate();
	return 0;
}

bool initOpenGL()
{
	if (!glfwInit())
	{
		std::cerr << "GLFW Init failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);

	if (gWindow == NULL)
	{
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(gWindow);
	//glfwSwapInterval(0);
	glfwSetKeyCallback(gWindow, glfw_onKey);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed init glew" << std::endl;
		glfwTerminate();
		return false;
	}

	//set clear color
	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

	return true;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		gWireFrame = !gWireFrame;
		if (gWireFrame)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void showFPS(GLFWwindow* window)
{
	static double prevSecs = 0.0;
	static int frameCount = 0;

	double elapsedSecs;
	double currentSecs = glfwGetTime();

	elapsedSecs = currentSecs - prevSecs;

	if (elapsedSecs > 0.25)
	{
		prevSecs = currentSecs;
		double fps = (double)frameCount / elapsedSecs;
		double msPerFrame = 1000.0 / fps;

		std::ostringstream outs;
		outs.precision(3);

		outs << std::fixed
			<< APP_TITLE << "  "
			<< "FPS: " << fps << "  "
			<< "Frame Time: " << msPerFrame << "(ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		frameCount = 0;
	}
	
	frameCount++;

}