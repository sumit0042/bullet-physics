#pragma once

#include<GL/glew.h>     // use this before GLFW
#include <GLFW/glfw3.h>


#define GLSL(src) "#version 410 core\n" #src
void shaderAttach(GLuint program, GLenum type, char* shaderSource = NULL);

const char* vertexShaderSrc = GLSL(
	in vec3 pos;
	in vec2 texco;
	out vec2 texco1;
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
	void main()
	{
		gl_Position = projection * view * model * vec4(pos, 1.0f);
		texco1 = texco;
	}
);

// Fragment shader
const char* fragmentShaderSrc = GLSL(
	precision mediump float;
	in vec2 texco1;

	uniform sampler2D sampler1;

	out vec4 FragColor;
	void main() {
		FragColor = texture(sampler1, texco1.xy);
	}
);


static GLuint shaderCompile(GLenum type, const char* source)
{
	GLuint shader;
	GLint length, result;

	/* create shader object, set the source, and compile */
	shader = glCreateShader(type);
	length = strlen(source);
	glShaderSource(shader, 1, (const char**)& source, &length);
	glCompileShader(shader);

	/* make sure the compilation was successful */
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		char* log;

		/* get the shader info log */
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		log = (char*)malloc(length);
		glGetShaderInfoLog(shader, length, &result, log);

		/* print an error message and the info log */
		printf("shaderCompileFromFile(): Unable to compile %s\n", log);
		free(log);

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

/*
 * Compiles and attaches a shader of the
 * given type to the given program object.
 */
void shaderAttach(GLuint program, GLenum type, const char* shaderSource)
{
	/* compile the shader */
	GLuint shader = shaderCompile(type, shaderSource);
	if (shader != 0) {
		/* attach the shader to the program */
		glAttachShader(program, shader);

		/* delete the shader - it won't actually be
		 * destroyed until the program that it's attached
		 * to has been destroyed */
		glDeleteShader(shader);
	}
}
