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
#include<math.h>
#include<stdlib.h>

#include "Shader.h"
#include "Camera.h"
#include "Cube.h"
#include "Sphere.h"

#include "C:\Users\Skumar3\source\repos\libraries\bullet3-master\src\btBulletDynamicsCommon.h"

using namespace std;
std::vector<glm::mat4x3> cube_faces(float x, float y, float z);
std::vector<glm::mat4x3> facesBat = cube_faces(0.09f, 0.4f, 0.05f);


Sphere* ball;
Cube* stump1;
Cube* stump2;
Cube* stump3;
Cube* bat;
std::vector<glm::mat4x3> facesStumps = cube_faces(0.05f, 0.4f, 0.05f);
const char* texStump = "C:\\Users\\Skumar3\\source\\repos\\trainingGraphics\\assets\\stump.bmp";
const char* texPitch = "C:\\Users\\Skumar3\\source\\repos\\trainingGraphics\\assets\\pitch.bmp";

auto collisionConfiguration = new btDefaultCollisionConfiguration();
auto dispatcher = new btCollisionDispatcher(collisionConfiguration);
auto overlappingPairCache = new btDbvtBroadphase();
auto solver = new btSequentialImpulseConstraintSolver;
auto  dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
/*=======================================================================================================================
			CAMERA AND INPUTS
=======================================================================================================================*/

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, -0.2f, 2.2f));
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

bool firstRand = true;
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	/*if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);*/
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		dynamicsWorld->removeRigidBody(stump1->m_rigid);
		dynamicsWorld->removeRigidBody(stump2->m_rigid);
		dynamicsWorld->removeRigidBody(stump3->m_rigid);
		stump1 = new Cube({ 0.05f, 0.4f, 0.05f }, facesStumps, { 0.0f, -0.7f, -1.4f }, dynamicsWorld, 1, 1);
		stump2 = new Cube({ 0.05f, 0.4f, 0.05f }, facesStumps, { 0.14f, -0.7f, -1.4f }, dynamicsWorld, 1, 1);
		stump3 = new Cube({ 0.05f, 0.4f, 0.05f }, facesStumps, { -0.14f, -0.7f, -1.4f }, dynamicsWorld, 1, 1);
		if (firstRand)
		{
			/*float ran = (rand() % 14) - 7;
			camera.Position.x = ran / 10.0f;
			firstRand = false;*/
			camera.Position.x = 0.0f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (bat!=nullptr)
		{
//			bat->m_rigid->applyCentralForce({-700.0f, 0.0f, 0.0f});
//			bat->m_rigid->setAngularVelocity({0.0f, 0.0f, 10.0f});
			bat->m_rigid->applyForce({ -700.0f, 0.0f, 0.0f }, { 0.0f, -0.5f, 0.0f });
			bat->m_rigid->setLinearVelocity({-1.0f, 0.0f, 0.0f});
		}
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		if (bat != nullptr)
		{
			bat->m_rigid->applyForce({ -700.0f, 0.0f, 0.0f }, { 0.0f, 0.5f, 0.0f });
			bat->m_rigid->setLinearVelocity({ 1.0f, 0.0f, 0.0f });
		}
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if (bat != nullptr)
		{
			dynamicsWorld->removeRigidBody(bat->m_rigid);
			delete bat;
			bat = new Cube({ 0.09f, 0.4f, 0.05f }, facesBat, { 0.0f, -0.7f, -1.2f }, dynamicsWorld, 100, 0);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (ball!=nullptr)
		{
			dynamicsWorld->removeRigidBody(ball->m_rigid);
			delete ball;
		}
		ball = new Sphere({ camera.Position.x,  camera.Position.y, camera.Position.z-0.2f}/*{ 0.09f, 0.9f,0.0f }*/, dynamicsWorld, 0.06f, 1);
		ball->m_rigid->setLinearVelocity({9.0f * camera.Front.x, 9.0f * camera.Front.y, 9.0f * camera.Front.z });
		ball->m_rigid->setRestitution(0.8f);
		firstRand = true;
	}
}

/*=======================================================================================================================
			PROGRAM AND DATA BINDING
=======================================================================================================================*/

std::vector<glm::mat4x3> cube_faces(float x, float y, float z)
{
	std::vector<glm::mat4x3> faces(6);
	glm::mat4x3 frontFace = {
		{-x,-y, z},
		{-x, y, z},
		{ x, y, z},
		{ x,-y, z}
	};

	glm::mat4x3 backFace = {
		{-x,-y,-z},
		{-x, y,-z},
		{ x, y,-z},
		{ x,-y,-z},
	};

	glm::mat4x3 topFace = {
		{ x, y,-z},
		{-x, y,-z},
		{-x, y, z},
		{ x, y, z}
	};

	glm::mat4x3 bottomFace = {
		{-x,-y, z},
		{-x,-y,-z},
		{ x,-y,-z},
		{ x,-y, z},
	};

	glm::mat4x3 rightFace = {
		{ x, y, z},
		{ x, y,-z},
		{ x,-y,-z},
		{ x,-y, z},
	};

	glm::mat4x3 leftFace = {
		{-x, y, z},
		{-x,-y, z},
		{-x,-y,-z},
		{-x, y,-z},
	};

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
	GLFWwindow* window;

	// Initialize the library 
	if (!glfwInit())
		return -1;

	// Create a windowed mode window and its OpenGL context 
	window = glfwCreateWindow(1280, 720, "Hello World", /*glfwGetPrimaryMonitor()*/NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, 800, 600);
	// Make the window's context current 
	glfwMakeContextCurrent(window);

	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();

	GLuint texID = loadBMP_custom(texPitch);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);

	GLuint texID2 = loadBMP_custom(texStump);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texID2);

	dynamicsWorld->setGravity({ 0,-10,0 });

	std::vector<glm::mat4x3> facesFloor = cube_faces(1.0f, 0.05f, 2.0f);
	std::vector<glm::mat4x3> facesBat = cube_faces(0.09f, 0.4f, 0.05f);

	Cube floor ({ 1.00f, 0.05f, 2.0f }, facesFloor, {0.0f, -0.95f, 0.0f}, dynamicsWorld, 0, 1);
	floor.m_rigid->setRestitution(0.8f);
	stump1 = new Cube({ 0.05f, 0.4f, 0.05f }, facesStumps, {0.0f, -0.7f, -1.4f}, dynamicsWorld, 1, 0);
	stump2 = new Cube({ 0.05f, 0.4f, 0.05f }, facesStumps, { 0.14f, -0.7f, -1.4f }, dynamicsWorld, 1, 0);
	stump3 = new Cube({ 0.05f, 0.4f, 0.05f }, facesStumps, {-0.14f, -0.7f, -1.4f }, dynamicsWorld, 1, 0);
	//Sphere sphere({ 0.04f, 0.8f,0.0f }, dynamicsWorld, 0.02f, 1);
	ball = new Sphere({ 0.09f, 0.9f,-10.0f }, dynamicsWorld, 0.06f, 1);

	bat = new Cube({ 0.09f, 0.4f, 0.05f }, facesBat, { 0.0f, -0.7f, -1.2f }, dynamicsWorld, 100, 0);

	while (!glfwWindowShouldClose(window))
	{
		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		floor.draw(camera);
		ball->draw(camera);
		stump1->draw(camera);
		stump2->draw(camera);
		stump3->draw(camera);
		bat->draw(camera);

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