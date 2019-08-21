#pragma once
#include<GL/glew.h>     // use this before GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"

#include "C:\Users\Skumar3\source\repos\libraries\bullet3-master\src\btBulletDynamicsCommon.h"

const int no_rects = 6;


GLuint create_program()
{
	GLuint program = glCreateProgram();
	shaderAttach(program, GL_VERTEX_SHADER, vertexShaderSrc);
	shaderAttach(program, GL_FRAGMENT_SHADER, fragmentShaderSrc);
	glLinkProgram(program);
	glValidateProgram(program);
	return program;
}


void bind_vert_n_texel(GLuint program)
{
	GLuint posAttrib = glGetAttribLocation(program, "pos");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	posAttrib = glGetAttribLocation(program, "texco");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}


struct Vertex
{
	float x, y, z;
	float tx = 0.0; float ty = 0.0;
};


class Cube
{
public:
	float mass = 1;
	btRigidBody* m_rigid = 0;
	GLuint m_g;
	GLuint program[no_rects];
	glm::mat4x3 m_faces[no_rects];
	Vertex m_c[no_rects][4];

	Cube(btVector3 pos, btDiscreteDynamicsWorld* world, float m):mass(m)
	{
		m_bind_buffer();

		//physics
		auto box = new btBoxShape({ .2,.2,.2});
		btTransform startTransform;
		startTransform.setIdentity();
		btVector3 localInertia(0, 0, 0);
		if (mass > 0)
			box->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(pos);
		auto motionstate = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionstate, box, localInertia);
		m_rigid = new btRigidBody(rbInfo);
		world->addRigidBody(m_rigid);

		//
		for (size_t i = 0; i < no_rects; i++)
		{
			program[i] = create_program();
		}
	}

	~Cube()
	{
		for (size_t i = 0; i < no_rects; i++)
		{
			glDeleteProgram(program[i]);
		}
		glDeleteVertexArrays(1, &m_g);
	}

	void bind_static_attribs()
	{
		for (size_t i = 0; i < no_rects; i++)
		{
			glUseProgram(program[i]);
			bind_vert_n_texel(program[i]);
			//bind_texture(program[i]);
		}
	}

	void m_bind_buffer()
	{
		glGenBuffers(1, &m_g);
		this->create_faces_data();
		glBindBuffer(GL_ARRAY_BUFFER, this->m_g);
		glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_c), this->m_c, GL_STATIC_DRAW);
	}

	void draw(Camera camera)
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->m_g);
		bind_static_attribs();
		
		for (size_t i = 0; i < no_rects; i++)
		{
			glUseProgram(program[i]);

			glm::mat4 ModelMatrix(1.0f);
			auto trans = m_rigid->getWorldTransform();

			glm::value_ptr(ModelMatrix);
			double mat[4][4] = {};
			float matf[4][4];
			trans.getOpenGLMatrix((btScalar*)& mat[0][0]);

			for (int i = 0;i < 4;++i)
				for (int j = 0;j < 4;++j)
					matf[i][j] = mat[i][j];

			auto loc = glGetUniformLocation(program[i], "model");
			glUniformMatrix4fv(loc, 1, GL_FALSE, &matf[0][0]);

			/*glm::mat4 model = glm::mat4(1.0f);
			model[3][0] = 0.6f;
			GLuint modelLoc = glGetUniformLocation(program[i], "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));*/

			glm::mat4 view = camera.GetViewMatrix();
			GLint viewLoc = glGetUniformLocation(program[i], "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			glm::mat4 pro = glm::mat4(1.0f);
			pro = glm::perspective(glm::radians(camera.Zoom), 1280.0f / 720.0f, 0.01f, 100.0f);
			GLint projectionLoc = glGetUniformLocation(program[i], "projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(pro));

			glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
			glUseProgram(0);
		}
	}

	void create_faces_data()
	{
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

		m_faces[0] = frontFace;
		m_faces[1] = backFace;
		m_faces[2] = leftFace;
		m_faces[3] = bottomFace;
		m_faces[4] = rightFace;
		m_faces[5] = topFace;

		for (size_t i = 0; i < no_rects; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				m_c[i][j].x = m_faces[i][j].x;
				m_c[i][j].y = m_faces[i][j].y;
				m_c[i][j].z = m_faces[i][j].z;
				switch (j)
				{
				case 0:
				{
					m_c[i][j].tx = 0.0f;
					m_c[i][j].ty = 0.0f;
					break;
				}
				case 1:
				{
					m_c[i][j].tx = 0.0f;
					m_c[i][j].ty = 1.0f;
					break;
				}
				case 2:
				{
					m_c[i][j].tx = 1.0f;
					m_c[i][j].ty = 1.0f;
					break;
				}
				case 3:
				{
					m_c[i][j].tx = 1.0f;
					m_c[i][j].ty = 0.0f;
					break;
				}
				default:
					break;
				}
			}
		}
	}
};