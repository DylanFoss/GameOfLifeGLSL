#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "GLErrorHandler.h"

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
	window = glfwCreateWindow(800, 800, "Game Of Life", NULL, NULL);
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

	//GLuint fb;
	//glGenFramebuffers(1, &fb);
	//glBindFramebuffer(GL_FRAMEBUFFER, fb);

	//GLuint frontTex;
	//GLuint backTex;

	//glActiveTexture(GL_TEXTURE0);
	//GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &frontTex));
	//GLCall(glBindTexture(GL_TEXTURE_2D, frontTex));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	//uint32_t colour = 0xffffffff;
	//GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, &colour))

	
	//glActiveTexture(GL_TEXTURE0 + 1);
	//GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &backTex));
	//GLCall(glBindTexture(GL_TEXTURE_2D, backTex));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	//uint32_t color = 0x00000000;
	//GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color));

	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, frontTex, 0);
	//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	//GLCall(glBindTexture(GL_TEXTURE_2D, frontTex));
	GLuint shader = CreateShader("test.vert.shader", "test.frag.shader");
	//GLCall(glUniform1i(glGetUniformLocation(shader, "u_Texture"), 0));

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		GLCall(glBindVertexArray(VA));
		GLCall(glBindVertexArray(VB));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB));

		GLCall(glUseProgram(shader));
		//GLCall

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backTex, 0);
		//glViewport(0, 0, 800, 800);
		//glBindTexture(GL_TEXTURE_2D, frontTex);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}