#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>

#include "GLErrorHandler.h"

#define GameSize 800

struct ShaderSource
{
	uint32_t type;
	std::string source;
};

ShaderSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	std::string line;
	std::stringstream ss;
	uint32_t type = GL_INVALID_ENUM;

	while (std::getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = GL_VERTEX_SHADER;
			else if (line.find("fragment") != std::string::npos)
				type = GL_FRAGMENT_SHADER;
		}
		else
		{
			ss << line << '\n';
		}
	}

	return { type, ss.str() };
};

GLuint CompileShader(const ShaderSource& shaderSource)
{
	unsigned int id = glCreateShader(shaderSource.type);
	const char* src = shaderSource.source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	//error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (shaderSource.type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
};

GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(ParseShader(vertexShader));
	unsigned int fs = CompileShader(ParseShader(fragmentShader));

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

struct Vertex
{
	glm::vec2 Position;
	glm::vec2 TexCoord;
};

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(800, 800, "Game of Life", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // 1 to cap to refresh rate

	std::cout << glGetString(GL_VERSION) << '\n';

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	if (glewInit() != GLEW_OK)
		std::cout << "ERROR!" << std::endl;

	//openGL

	Vertex vertices[4] = {
		{{-1.0f, -1.0f},	{0.0f, 0.0f}},
		{{ 1.0f, -1.0f},	{1.0f, 0.0f}},
		{{ 1.0f,  1.0f},	{1.0f, 1.0f}},
		{{-1.0f,  1.0f},	{0.0f, 1.0f}}
	};

	unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

	unsigned int VA;
	glGenVertexArrays(1, &VA);
	glBindVertexArray(VA);

	unsigned int VB;
	glGenBuffers(1, &VB);

	glBindBuffer(GL_ARRAY_BUFFER,VB);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*4, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(VA);

	GLCall(glEnableVertexArrayAttrib(VA, 0));
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

	GLCall(glEnableVertexArrayAttrib(VA, 1));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoord));

	unsigned int IB;

	GLCall(glCreateBuffers(1, &IB));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) , indices, GL_STATIC_DRAW));

	GLuint fb;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	GLuint renderTarget;

	GLuint frontTex;
	GLuint backTex;

	glActiveTexture(GL_TEXTURE0);
	GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &renderTarget));
	GLCall(glBindTexture(GL_TEXTURE_2D, renderTarget));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr))

	glActiveTexture(GL_TEXTURE0+1);
	GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &frontTex));
	GLCall(glBindTexture(GL_TEXTURE_2D, frontTex));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	//uint32_t colour = 0xffffffff;
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GameSize, GameSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr))
	
	glActiveTexture(GL_TEXTURE0 + 2);
	GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &backTex));
	GLCall(glBindTexture(GL_TEXTURE_2D, backTex));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GameSize, GameSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTarget, 0);

	GLCall(glBindTexture(GL_TEXTURE_2D, frontTex));
	GLCall(glBindTexture(GL_TEXTURE_2D, backTex));
	GLuint shader = CreateShader("test.vert.shader", "test.frag.shader");

	GLCall(glUseProgram(shader));
	int sampler[3] = {0,1,2};
	auto loc = glGetUniformLocation(shader, "u_Textures");
	GLCall(glUniform1iv(loc, 3, sampler));
	GLCall(glUniform1f(glGetUniformLocation(shader, "u_TexIndex"), 0));
	GLCall(glUniform1f(glGetUniformLocation(shader, "u_Scale"), 100));

	GLuint GoL = CreateShader("test.vert.shader", "GoL.frag.shader");
	GLCall(glUseProgram(GoL));
	GLCall(glUniform1i(glGetUniformLocation(GoL, "u_State"), 1));
	GLCall(glUniform2f(glGetUniformLocation(GoL, "u_Scale"), GameSize, GameSize));

	//inital noise

	GLuint noise = CreateShader("test.vert.shader", "noise.frag.shader");
	GLCall(glUseProgram(noise));

	GLuint noiseBuffer;
	glGenFramebuffers(1, &noiseBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, noiseBuffer);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frontTex, 0);

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, GameSize, GameSize);

	GLCall(glUseProgram(noise));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Time keeping

	std::chrono::time_point <std::chrono::high_resolution_clock> t1, t2;
	t1 = std::chrono::high_resolution_clock::now();

	//show inital noise

	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glPushAttrib(GL_VIEWPORT_BIT);
	//glViewport(0, 0, 800, 800);

	GLCall(glUseProgram(shader));
	GLCall(glUniform1f(glGetUniformLocation(shader, "u_TexIndex"), 1));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int backTexture = 1;
	unsigned int frontTexture = 2;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{

		t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsedTime = t2 - t1;

		if (elapsedTime.count() > 0.05f)
		{

			glBindFramebuffer(GL_FRAMEBUFFER, noiseBuffer);

			glPushAttrib(GL_VIEWPORT_BIT);
			glViewport(0, 0, GameSize, GameSize);

			GLCall(glUseProgram(GoL));
			GLCall(glUniform1i(glGetUniformLocation(GoL, "u_State"), backTexture));

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			glActiveTexture(GL_TEXTURE0+frontTexture);
			glBindTexture(GL_TEXTURE_2D, noiseBuffer);

			glPopAttrib();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			std::swap(backTexture, frontTexture);

			t1 = t2;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 800, 800);
		GLCall(glUseProgram(shader));
		GLCall(glUniform1f(glGetUniformLocation(shader, "u_TexIndex"), backTexture));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}