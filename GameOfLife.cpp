#include "GameOfLife.h"
#include <iostream>
#include <sstream>
#include <fstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "GLErrorHandler.h"

GameOfLife::GameOfLife(const std::string& name, uint32_t width, uint32_t height)
    :Application(name, width, height)
{
	Init();
}

constexpr int GameSize = 400;
constexpr int WindowSize = 800;

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

struct VP
{
	glm::mat4 projectionInital = glm::ortho(0.0f, 800.0f, 0.0f, 800.0f, -1.0f, 1.0f);
	glm::mat4 viewInital = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

	glm::mat4 projection = projectionInital;
	glm::mat4 view = viewInital;
};


VP vp;

Vertex vertices2[4] = {
		{{-1.0f, -1.0f},	{0.0f, 0.0f}},
		{{ 1.0f, -1.0f},	{1.0f, 0.0f}},
		{{ 1.0f,  1.0f},	{1.0f, 1.0f}},
		{{-1.0f,  1.0f},	{0.0f, 1.0f}}
};

Vertex vertices[4] = {
	{{0, 0},		{0.0f, 0.0f}},
	{{800, 0},		{1.0f, 0.0f}},
	{{800,  800},	{1.0f, 1.0f}},
	{{0,  800},		{0.0f, 1.0f}}
};

unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

unsigned int VA, VB, VA2, VB2, IB;

GLuint noiseBuffer, fb;

GLuint shader, GoL;

GLuint renderTarget;

GLuint frontTex, backTex;

glm::mat4 model = glm::mat4(1.0f);

void GameOfLife::Init()
{
	if (glewInit() != GLEW_OK)
		std::cout << "ERROR!" << std::endl;

	std::cout << glGetString(GL_VERSION) << '\n';

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glClearColor(0.0f, 0.0f, 0.20f, 1.0f);

	//glOrtho(0, WindowSize, 0, WindowSize, -1, 1);

	//openGL

	unsigned int VA;
	glGenVertexArrays(1, &VA);
	glBindVertexArray(VA);

	//unsigned int VB;
	glGenBuffers(1, &VB);

	//unsigned int IB;

	GLCall(glCreateBuffers(1, &IB));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) , indices, GL_STATIC_DRAW));

	glBindBuffer(GL_ARRAY_BUFFER,VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*4, vertices, GL_STATIC_DRAW);

	GLCall(glEnableVertexAttribArray(VA));

	GLCall(glEnableVertexArrayAttrib(VA, 0));
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

	GLCall(glEnableVertexArrayAttrib(VA, 1));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoord));

	//unsigned int VA2;
	glGenVertexArrays(1, &VA2);
	glBindVertexArray(VA2);

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	//unsigned int VB2;
	glGenBuffers(1, &VB2);
	glBindBuffer(GL_ARRAY_BUFFER, VB2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertices2, GL_STATIC_DRAW);

	glEnableVertexAttribArray(VA2);

	GLCall(glEnableVertexArrayAttrib(VA2, 0));
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

	GLCall(glEnableVertexArrayAttrib(VA2, 1));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoord));

	//glBindBuffer(GL_ARRAY_BUFFER, VB);


	//GLuint fb;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	//GLuint renderTarget;

	//GLuint frontTex;
	//GLuint backTex;

	glActiveTexture(GL_TEXTURE0);
	GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &renderTarget));
	GLCall(glBindTexture(GL_TEXTURE_2D, renderTarget));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WindowSize, WindowSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

	glActiveTexture(GL_TEXTURE0+1);
	GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &frontTex));
	GLCall(glBindTexture(GL_TEXTURE_2D, frontTex));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GameSize, GameSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
	
	glActiveTexture(GL_TEXTURE0 + 2);
	GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &backTex));
	GLCall(glBindTexture(GL_TEXTURE_2D, backTex));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GameSize, GameSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTarget, 0);

	//GLCall(glBindTexture(GL_TEXTURE_2D, frontTex));
	//GLCall(glBindTexture(GL_TEXTURE_2D, backTex));

	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0));

	glm::mat4 mvp = vp.projection * vp.view * model;
	shader = CreateShader("basic.vert.shader", "basic.frag.shader");

	GLCall(glUseProgram(shader));
	int sampler[3] = {0,1,2};
	auto loc = glGetUniformLocation(shader, "u_Textures");
	GLCall(glUniform1iv(loc, 3, sampler));
	GLCall(glUniform1f(glGetUniformLocation(shader, "u_TexIndex"), 0));
	GLCall(glUniformMatrix4fv(glGetUniformLocation(shader, "u_MVP"), 1, GL_FALSE, &mvp[0][0]));

	GoL = CreateShader("GoL.vert.shader", "GoL.frag.shader");
	GLCall(glUseProgram(GoL));
	GLCall(glUniform1i(glGetUniformLocation(GoL, "u_State"), 1));
	GLCall(glUniform2f(glGetUniformLocation(GoL, "u_Scale"), GameSize, GameSize));

	//inital noise

	GLuint noise = CreateShader("noise.vert.shader", "noise.frag.shader");
	GLCall(glUseProgram(noise));

	//GLuint noiseBuffer;
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

	//show inital noise

	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glPushAttrib(GL_VIEWPORT_BIT);
	//glViewport(0, 0, WindowSize, WindowSize);

	GLCall(glUseProgram(shader));
	GLCall(glUniform1f(glGetUniformLocation(shader, "u_TexIndex"), 1));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float counter = 0.0f;

int frontTexture = 1;
int backTexture = 2;
void GameOfLife::Update(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT);

	counter += deltaTime;

	if (counter > 0.05f)
	{

		glBindFramebuffer(GL_FRAMEBUFFER, noiseBuffer);

		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0, 0, GameSize, GameSize);

		GLCall(glUseProgram(GoL));
		GLCall(glUniform1i(glGetUniformLocation(GoL, "u_State"), backTexture));

		glBindBuffer(GL_ARRAY_BUFFER, VB2);
		glBindVertexArray(VA2);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glActiveTexture(GL_TEXTURE0+frontTexture);
		glBindTexture(GL_TEXTURE_2D, noiseBuffer);

		glPopAttrib();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		std::swap(backTexture, frontTexture);

		counter = 0.0f;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WindowSize, WindowSize);
	GLCall(glUseProgram(shader));
	GLCall(glUniform1f(glGetUniformLocation(shader, "u_TexIndex"), backTexture));

	glm::mat4 mvp = vp.projection * vp.view * model;
	GLCall(glUniformMatrix4fv(glGetUniformLocation(shader, "u_MVP"), 1, GL_FALSE, &mvp[0][0]));
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBindVertexArray(VA);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}

void GameOfLife::Draw(float deltaTime)
{
	//std::cout << "Drawn!" << '\n';
}
