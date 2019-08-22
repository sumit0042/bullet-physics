#pragma once
#include <GL/glew.h>     // use this before GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Shader.h"

#include <vector>
#include <math.h>

#include "C:\Users\Skumar3\source\repos\libraries\bullet3-master\src\btBulletDynamicsCommon.h"


class Sphere
{
public:
	GLuint m_g;
	GLuint m_program;
	float m_r;
	Vertex m_c[12];
	Vertex m_vertices[20][3];

	glm::vec3 m_origin;
	
	Sphere(/*glm::vec3 origin, */float r):m_r(r)
	{
		/*m_origin = origin;*/
		create_vertex_data();
		this->m_bind_buffer();
		m_program = create_program();
	}
	
	~Sphere()
	{
		glDeleteProgram(m_program);
		glDeleteVertexArrays(1, &m_g);
	}
	
	void create_vertex_data()
	{
		m_c[0] = { 0.0f, 0.0f, m_r, 0.0f, 0.0f };
		float conv = 2 * 3.14f / 360.0f;
		float alt = conv*26.565f;
		float offset = conv * 36.0f;
		for (size_t i = 1; i < 6; i++)
		{
			m_c[i] = { 
				m_r * cos(alt) * cos(conv * 72.0f * i - offset), 
				m_r * cos(alt) * sin(conv * 72.0f * i - offset), 
				m_r * sin(alt), 0.0f, 0.0f };
		}
		for (size_t i = 6; i < 11; i++)
		{
			m_c[i] = {
				m_r * cos(-1 * alt) * cos(conv * 72.0f * i), 
				m_r * cos(-1 * alt) * sin(conv * 72.0f * i), 
				m_r * sin(-1 * alt), 0.0f, 0.0f };
		}
		m_c[11] = { 0.0f, 0.0f,-1 * m_r, 0.0f, 0.0f };

		for (size_t i = 0; i < 5; i++)
		{
			m_vertices[i][0] = m_c[0];
			m_vertices[i][1] = m_c[i + 1];
			m_vertices[i][2] = m_c[(i+2)%5];
		}
		m_vertices[3][2] = m_c[5];
		
		for (size_t i = 9; i > 4; i--)
		{
			m_vertices[i][0] = m_c[11];
			m_vertices[i][1] = m_c[i+1];
			m_vertices[i][2] = m_c[(i+2)%5 + 5];
		}
		m_vertices[8][2] = m_c[10];

		for (size_t i = 10; i < 14; i++)
		{
			m_vertices[i][0] = m_c[i - 9];
			m_vertices[i][1] = m_c[i - 4];
			m_vertices[i][2] = m_c[i - 5];
		}
		m_vertices[10][2] = m_c[10];

		for (size_t i = 15; i < 20; i++)
		{
			m_vertices[i][0] = m_c[i - 9];
			m_vertices[i][1] = m_c[i - 14];
			m_vertices[i][2] = m_c[(i - 13) % 5];
		}
		m_vertices[18][2] = m_c[5];
	}
	
	void m_bind_buffer()
	{
		glGenBuffers(1, &m_g);
		glBindBuffer(GL_ARRAY_BUFFER, this->m_g);
		glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_vertices), this->m_vertices, GL_STATIC_DRAW);
	}

	void bind_static_attribs()
	{
		glUseProgram(m_program);
		bind_vert_n_texel(m_program);
	}

	void draw(Camera camera)
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->m_g);
		bind_static_attribs();

		glm::mat4 model = glm::mat4(1.0f);
		model[3][2] = 0.5f;
		GLuint modelLoc = glGetUniformLocation(m_program, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glm::mat4 view = camera.GetViewMatrix();
		GLint viewLoc = glGetUniformLocation(m_program, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 pro = glm::mat4(1.0f);
		pro = glm::perspective(glm::radians(camera.Zoom), 1280.0f / 720.0f, 0.01f, 100.0f);
		GLint projectionLoc = glGetUniformLocation(m_program, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(pro));
		
		glDrawArrays(GL_TRIANGLES, 0, 60);

		glUseProgram(0);
	}
};
