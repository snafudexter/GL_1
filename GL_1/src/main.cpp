#include <iostream>
#include <sstream>
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Mesh.h"

const char* APP_TITLE = "First Triangle";
 int gWindowWidth = 1024;
 int gWindowHeight = 768;


GLFWwindow* gWindow = NULL;
bool gWireFrame = false;

/*
OrbitCamera orbitCamera;
float gYaw = 0.0f;
float gPitch = 0.0f;
float gRadius = 10.0f;
const float MOUSE_SENSTIVITY = 0.25f;
*/

FPSCamera fpsCamera(glm::vec3(0.0f, 0.0f, 5.0f));
const double ZOOM_SENSTIVITY = -3.0;
const float MOVE_SPEED = 5.0;
const float MOVE_SENSITIVITY = 0.1f;


void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onMouseMove(GLFWwindow* window, double posX, double posY);
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);

void showFPS(GLFWwindow* window);
void update(double elapsedTime);


bool initOpenGL();

int main()
{
	if (!initOpenGL())
	{
		std::cerr << "OpenGL init failed" << std::endl;
	}

		
	ShaderProgram lightShader;
	lightShader.loadShaders("basic.vert", "basic.frag");

	ShaderProgram lightingShader;
	lightingShader.loadShaders("lighting.vert", "lighting.frag");

	glm::vec3 modelPos[] = {
		glm::vec3(-2.5f, 1.0f, 0.0f),
		glm::vec3(2.5f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f)
	};

	glm::vec3 modelScale[] = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(10.0f, 1.0f, 10.0f),
	};

	const int numModels = 4;
	Mesh mesh[numModels];
	Texture2D texture[numModels];

	mesh[0].loadOBJ("models/crate.obj");
	mesh[1].loadOBJ("models/woodcrate.obj");
	mesh[2].loadOBJ("models/robot.obj");
	mesh[3].loadOBJ("models/floor.obj");


	texture[0].loadTexture("textures/crate.jpg", true);
	texture[1].loadTexture("textures/woodcrate_diffuse.jpg", true);
	texture[2].loadTexture("textures/robot_diffuse.jpg", true);
	texture[3].loadTexture("textures/tile_floor.jpg", true);

	Mesh lightMesh;
	lightMesh.loadOBJ("models/light.obj");


	double lastTime = glfwGetTime();
	float angle = 0.0f;
	


	//main loop
	while (!glfwWindowShouldClose(gWindow)) {

		showFPS(gWindow);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		glfwPollEvents();
		update(deltaTime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 model, view, projection;


		/*
		orbitCamera.setLookAt(cubePos);
		orbitCamera.rotate(gYaw, gPitch);
		orbitCamera.setRadius(gRadius);
		*/


		// Create the View matrix
		view = fpsCamera.getViewMatrix();

		// Create the projection matrix
		projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 100.0f);


		glm::vec3 viewPos;
		viewPos.x = fpsCamera.getPosition().x;
		viewPos.y = fpsCamera.getPosition().y;
		viewPos.z = fpsCamera.getPosition().z;
		
		//light pos
		glm::vec3 lightPos(0.0f, 1.0f, 10.0f);
		glm::vec3 lightColor(0.0f, 1.0f, 0.0f);

		angle += (float)deltaTime * 50.0f;
		lightPos.x = 8.0f * sinf(glm::radians(angle));

		lightingShader.use();

		lightingShader.setUniform("model", model);
		lightingShader.setUniform("view", view);
		lightingShader.setUniform("projection", projection);
		lightingShader.setUniform("lightColor", lightColor);
		lightingShader.setUniform("lightPos", lightPos);
		lightingShader.setUniform("viewPos", viewPos);

		for (int i = 0; i < numModels; i++)
		{
			model = glm::translate(glm::mat4(), modelPos[i]) * glm::scale(glm::mat4(), modelScale[i]);
			lightingShader.setUniform("model", model);
			texture[i].bind(0);
			mesh[i].draw();
			texture[i].unbind(0);
		}

		//render light
		model = glm::translate(glm::mat4(), lightPos);
		lightShader.use();
		lightShader.setUniform("lightColor", lightColor);
		lightShader.setUniform("model", model);
		lightShader.setUniform("view", view);
		lightShader.setUniform("projection", projection);
		lightMesh.draw();

		glfwSwapBuffers(gWindow);
		lastTime = currentTime;

	}

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

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed init glew" << std::endl;
		glfwTerminate();
		return false;
	}
	
	glfwSetKeyCallback(gWindow, glfw_onKey);
	glfwSetCursorPosCallback(gWindow, glfw_onMouseMove);
	glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);
	glfwSetScrollCallback(gWindow, glfw_onMouseScroll);

	glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0f);

	//set clear color
	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

	glViewport(0, 0, gWindowWidth, gWindowHeight);
	glEnable(GL_DEPTH_TEST);

	return true;
}
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height)
{
	gWindowWidth = width;
	gWindowHeight = height;
	glViewport(0, 0, gWindowWidth, gWindowHeight);
}


void glfw_onMouseMove(GLFWwindow* window, double posX, double posY)
{
	/*static glm::vec2 lastMousePos = glm::vec2(0, 0);

	if (glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_LEFT) == 1)
	{
		gYaw -= ((float)posX - lastMousePos.x) * MOUSE_SENSTIVITY;
		gPitch += ((float)posY - lastMousePos.y) * MOUSE_SENSTIVITY;
	}

	if (glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_RIGHT) == 1)
	{
		float dx = 0.01f * ((float)posX - lastMousePos.x);
		float dy = 0.01f * ((float)posY - lastMousePos.y);
		gRadius += dx - dy;
	}

	lastMousePos.x = (float)posX;
	lastMousePos.y = (float)posY;*/
}

void glfw_onMouseScroll(GLFWwindow * window, double deltaX, double deltaY)
{
	double fov = fpsCamera.getFOV() + deltaY + ZOOM_SENSTIVITY;
	fov = glm::clamp((float)fov, 1.0f, 130.0f);
	fpsCamera.setFOV((float)fov);
	
}

void update(double elapsedTime)
{
	double mouseX, mouseY;

	glfwGetCursorPos(gWindow, &mouseX, &mouseY);
	fpsCamera.rotate((float)(gWindowWidth / 2.0 - mouseX) * MOVE_SENSITIVITY, (float)(gWindowHeight / 2.0 - mouseY) * MOVE_SENSITIVITY);
	
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);


	if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getLook());
	else if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getLook());

	if (glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getRight());
	else if (glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getRight());

	if (glfwGetKey(gWindow, GLFW_KEY_Z) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getUp());
	else if (glfwGetKey(gWindow, GLFW_KEY_X) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getUp());
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_E && action == GLFW_PRESS)
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