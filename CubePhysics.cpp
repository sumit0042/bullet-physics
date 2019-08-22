#include<GL/glew.h>     // use this before GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<iostream>
#include<fstream>
#include<string>
#include<tuple>
#include<Windows.h>
#include<vector>

#include "Shader.h"
#include "Camera.h"
#include "Cube.h"
#include "Sphere.h"

#include "C:\Users\Skumar3\source\repos\libraries\bullet3-master\src\btBulletDynamicsCommon.h"

using namespace std;

/*=======================================================================================================================
			CAMERA AND INPUTS
=======================================================================================================================*/

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, -0.6f, 1.6f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


/*=======================================================================================================================
			PROGRAM AND DATA BINDING
=======================================================================================================================*/

std::vector<glm::mat4x3> cube_faces(bool floor)
{
	std::vector<glm::mat4x3> faces(6);
	glm::mat4x3 frontFace = {
		{-0.2f,-0.2f, 0.2f},
		{-0.2f, 0.2f, 0.2f},
		{ 0.2f, 0.2f, 0.2f},
		{ 0.2f,-0.2f, 0.2f}
	};

	glm::mat4x3 backFace = {
		{-0.2f,-0.2f,-0.2f},
		{-0.2f, 0.2f,-0.2f},
		{ 0.2f, 0.2f,-0.2f},
		{ 0.2f,-0.2f,-0.2f},
	};

	glm::mat4x3 topFace = {
		{ 0.2f, 0.2f,-0.2f},
		{-0.2f, 0.2f,-0.2f},
		{-0.2f, 0.2f, 0.2f},
		{ 0.2f, 0.2f, 0.2f}
	};

	glm::mat4x3 bottomFace = {
		{-0.2f,-0.2f, 0.2f},
		{-0.2f,-0.2f,-0.2f},
		{ 0.2f,-0.2f,-0.2f},
		{ 0.2f,-0.2f, 0.2f},
	};

	glm::mat4x3 rightFace = {
		{ 0.2f, 0.2f, 0.2f},
		{ 0.2f, 0.2f,-0.2f},
		{ 0.2f,-0.2f,-0.2f},
		{ 0.2f,-0.2f, 0.2f},
	};

	glm::mat4x3 leftFace = {
		{-0.2f, 0.2f, 0.2f},
		{-0.2f,-0.2f, 0.2f},
		{-0.2f,-0.2f,-0.2f},
		{-0.2f, 0.2f,-0.2f},
	};

	if (floor)
	{
		frontFace = {
			{-1.0f,-0.05f, 1.0f},
			{-1.0f, 0.05f, 1.0f},
			{ 1.0f, 0.05f, 1.0f},
			{ 1.0f,-0.05f, 1.0f}
		};

		backFace = {
			{-1.0f,-0.05f,-1.0f},
			{-1.0f, 0.05f,-1.0f},
			{ 1.0f, 0.05f,-1.0f},
			{ 1.0f,-0.05f,-1.0f}
		};

		topFace = {
			{ 1.0f, 0.05f, 1.0f},
			{ 1.0f, 0.05f,-1.0f},
			{-1.0f, 0.05f,-1.0f},
			{-1.0f, 0.05f, 1.0f}
		};

		bottomFace = {
			{ 1.0f,-0.05f, 1.0f},
			{ 1.0f,-0.05f,-1.0f},
			{-1.0f,-0.05f,-1.0f},
			{-1.0f,-0.05f, 1.0f}
		};

		rightFace = {
			{ 1.0f, 0.05f, 1.0f},
			{ 1.0f, 0.05f,-1.0f},
			{ 1.0f, 0.05f,-1.0f},
			{ 1.0f, 0.05f, 1.0f},
		};

		leftFace = {
			{ -1.0f, 0.05f, 1.0f},
			{ -1.0f, 0.05f,-1.0f},
			{ -1.0f,-0.05f,-1.0f},
			{ -1.0f,-0.05f, 1.0f},
		};
	}

	faces[0] = frontFace;
	faces[1] = backFace;
	faces[2] = leftFace;
	faces[3] = bottomFace;
	faces[4] = rightFace;
	faces[5] = topFace;

	return faces;
}

int main()
{
	auto collisionConfiguration = new btDefaultCollisionConfiguration();
	auto dispatcher = new btCollisionDispatcher(collisionConfiguration);
	auto overlappingPairCache = new btDbvtBroadphase();
	auto solver = new btSequentialImpulseConstraintSolver;
	auto  dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	GLFWwindow* window;

	// Initialize the library 
	if (!glfwInit())
		return -1;

	// Create a windowed mode window and its OpenGL context 
	window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, 800, 600);
	// Make the window's context current 
	glfwMakeContextCurrent(window);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();

	dynamicsWorld->setGravity({ 0,-10,0 });

	std::vector<glm::mat4x3> faces = cube_faces(true);
	std::vector<glm::mat4x3> facesC = cube_faces(false);
	//Cube cuby(faces, { 0.24f, 0.4f,0.0f}, dynamicsWorld,1);
	//Cube cube(false, facesC, {-0.24f, 0.4f,0.0f}, dynamicsWorld, 10);
	//Cube cuba(false, facesC, {-0.04f, 0.8f,0.0f }, dynamicsWorld,1);
	//cube->m_g = g;
	Cube floor(true, faces, {0.0f, -0.95f, 0.0f}, dynamicsWorld, 0);
	Sphere sphere({ 0.04f, 0.8f,0.0f }, dynamicsWorld, 0.02f, 1);
	Sphere sphera({ 0.09f, 0.9f,0.0f }, dynamicsWorld, 0.06f, 1);

	while (!glfwWindowShouldClose(window))
	{
		glEnable(GL_DEPTH_TEST);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//cube.draw(camera);
		//cuba.draw(camera);
		/*cuby.draw(camera);*/
		floor.draw(camera);
		sphere.draw(camera);
		sphera.draw(camera);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		glfwPollEvents();
		dynamicsWorld->stepSimulation(1.f / 100.f, 100);
	}

	glUseProgram(0);
	glfwTerminate();
	return 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//----------------------------------------------------------------------------------------------------