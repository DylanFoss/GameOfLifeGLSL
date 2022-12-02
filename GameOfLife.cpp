#include "GameOfLife.h"
#include "pch.h"
#include <fstream>

#include "GLErrorHandler.h"

GameOfLife::GameOfLife(const std::string& name, uint32_t width, uint32_t height)
    :Application(name, width, height)
{
	Init();
}

constexpr int GameSize = 400;
constexpr int WindowSize = 800;

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

Utils::Shader shader, GoL, noise;

GLuint renderTarget;

GLuint frontTex, backTex;

glm::mat4 model;

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

	//unsigned int VA;
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

	model = glm::mat4(1.0f);
	//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0));

	glm::mat4 mvp = vp.projection * vp.view * model;
	shader.CreateShader("basic.vert.shader", "basic.frag.shader");

	shader.Bind();
	int sampler[3] = {0,1,2};
	auto loc = glGetUniformLocation(shader.ID(), "u_Textures");
	GLCall(glUniform1iv(loc, 3, sampler));
	GLCall(glUniform1f(glGetUniformLocation(shader.ID(), "u_TexIndex"), 0));
	GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.ID(), "u_MVP"), 1, GL_FALSE, &mvp[0][0]));

	GoL.CreateShader("GoL.vert.shader", "GoL.frag.shader");
	GLCall(GoL.Bind());
	GLCall(glUniform1i(glGetUniformLocation(GoL.ID(), "u_State"), 1));
	GLCall(glUniform2f(glGetUniformLocation(GoL.ID(), "u_Scale"), GameSize, GameSize));

	//inital noise

	noise.CreateShader("noise.vert.shader", "noise.frag.shader");
	noise.Bind();

	//GLuint noiseBuffer;
	glGenFramebuffers(1, &noiseBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, noiseBuffer);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frontTex, 0);

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, GameSize, GameSize);

	GLCall(glUseProgram(noise.ID()));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//show inital noise

	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glPushAttrib(GL_VIEWPORT_BIT);
	//glViewport(0, 0, WindowSize, WindowSize);

	shader.Bind();
	GLCall(glUniform1f(glGetUniformLocation(shader.ID(), "u_TexIndex"), 1));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float counter = 0.0f;

int backTexture = 1;
int frontTexture = 2;
void GameOfLife::Update(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT);

	counter += deltaTime;

	if (counter > 0.05f)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, noiseBuffer);

		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0, 0, GameSize, GameSize);

		GoL.Bind();
		GLCall(glUniform1i(glGetUniformLocation(GoL.ID(), "u_State"), backTexture));

		glBindBuffer(GL_ARRAY_BUFFER, VB2);
		glBindVertexArray(VA2);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glActiveTexture(GL_TEXTURE0+frontTexture);
		glBindTexture(GL_TEXTURE_2D, noiseBuffer);

		glPopAttrib();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		std::swap(backTexture, frontTexture);

		counter = 0;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WindowSize, WindowSize);
	shader.Bind();
	GLCall(glUniform1f(glGetUniformLocation(shader.ID(), "u_TexIndex"), backTexture));

	glm::mat4 mvp = vp.projection * vp.view * model;
	GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.ID(), "u_MVP"), 1, GL_FALSE, &mvp[0][0]));
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBindVertexArray(VA);
	//glBindBuffer(GL_ARRAY_BUFFER, VB);
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}

void GameOfLife::Draw(float deltaTime)
{
	//std::cout << "Drawn!" << '\n';
}
