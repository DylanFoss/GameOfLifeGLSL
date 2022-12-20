#include "GameOfLife.h"
#include "pch.h"
#include <fstream>

#include "MouseCodes.h"
#include "KeyCodes.h"

#include "GLErrorHandler.h"

GameOfLife::GameOfLife(const std::string& name, uint32_t width, uint32_t height, uint32_t gameWidth, uint32_t gameHeight)
    :Application(name, width, height), m_GameWidth(gameWidth), m_GameHeight(gameHeight), m_WindowHalfHeight(height * 0.5f), m_WindowHalfWidth(width * 0.5f)
{
	Init();
	camera = Utils::OrthographicCameraController(m_WindowHalfWidth);
}

GameOfLife::~GameOfLife()
{
	Shutdown();
}

struct Vertex
{
	glm::vec2 Position;
	glm::vec2 TexCoord;
};

unsigned int VA, VB, VA2, VB2, IB;

GLuint noiseBuffer, fb;

Utils::Shader shader, GoL, noise, paint;

GLuint renderTarget;
GLuint frontTex, backTex;

glm::mat4 model;

void GameOfLife::Init()
{

	if (glewInit() != GLEW_OK)
		std::cout << "ERROR!" << std::endl;

	std::cout << glGetString(GL_VERSION) << '\n';

	{
		GLint size;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
		std::cout << "Max texture size : " << size << '\n';
	}

	m_Window.get()->SetVsync(true);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glClearColor(0.0f, 0.0f, 0.20f, 1.0f);

	//openGL

	glGenVertexArrays(1, &VA);
	glBindVertexArray(VA);

	glGenBuffers(1, &VB);

	unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

	GLCall(glCreateBuffers(1, &IB));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) , indices, GL_STATIC_DRAW));

	std::vector<Vertex> vertices = {
		{{-m_WindowHalfWidth, -m_WindowHalfHeight}, {0.0f, 0.0f}},
		{{ m_WindowHalfWidth, -m_WindowHalfHeight}, {1.0f, 0.0f}},
		{{ m_WindowHalfWidth,  m_WindowHalfHeight}, {1.0f, 1.0f}},
		{{-m_WindowHalfWidth,  m_WindowHalfHeight}, {0.0f, 1.0f}}
	};

	glBindBuffer(GL_ARRAY_BUFFER,VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*4, &vertices[0], GL_STATIC_DRAW);

	GLCall(glEnableVertexAttribArray(VA));

	GLCall(glEnableVertexArrayAttrib(VA, 0));
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

	GLCall(glEnableVertexArrayAttrib(VA, 1));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoord));

	glGenVertexArrays(1, &VA2);
	glBindVertexArray(VA2);

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	std::vector<Vertex> vertices2 = {
		{{-1.0f, -1.0f},	{0.0f, 0.0f}},
		{{ 1.0f, -1.0f},	{1.0f, 0.0f}},
		{{ 1.0f,  1.0f},	{1.0f, 1.0f}},
		{{-1.0f,  1.0f},	{0.0f, 1.0f}}
	};

	glGenBuffers(1, &VB2);
	glBindBuffer(GL_ARRAY_BUFFER, VB2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, &vertices2[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(VA2);

	GLCall(glEnableVertexArrayAttrib(VA2, 0));
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

	GLCall(glEnableVertexArrayAttrib(VA2, 1));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoord));

	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	glActiveTexture(GL_TEXTURE0);
	GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &renderTarget));
	GLCall(glBindTexture(GL_TEXTURE_2D, renderTarget));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Window->GetWidth(), m_Window->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

	glActiveTexture(GL_TEXTURE0+1);
	GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &frontTex));
	GLCall(glBindTexture(GL_TEXTURE_2D, frontTex));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_GameWidth, m_GameHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
	
	glActiveTexture(GL_TEXTURE0 + 2);
	GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &backTex));
	GLCall(glBindTexture(GL_TEXTURE_2D, backTex));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_GameWidth, m_GameHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTarget, 0);

	model = glm::mat4(1.0f);

	glm::mat4 mvp = camera.GetCamera().GetViewProjectionMatrix() * model;
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
	GLCall(glUniform2f(glGetUniformLocation(GoL.ID(), "u_Scale"), m_GameWidth, m_GameHeight));

	//inital noise
	noise.CreateShader("noise.vert.shader", "noise.frag.shader");
	noise.Bind();

	paint.CreateShader("Paint.vert.shader", "Paint.frag.shader");
	paint.Bind();
	GLCall(glUniform1i(glGetUniformLocation(paint.ID(), "u_State"), 1));
	GLCall(glUniform2f(glGetUniformLocation(paint.ID(), "u_Scale"), m_GameWidth, m_GameHeight));
	GLCall(glUniform2f(glGetUniformLocation(paint.ID(), "u_Cell"), 0, 0));

	glGenFramebuffers(1, &noiseBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, noiseBuffer);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frontTex, 0);

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, m_GameWidth, m_GameHeight);

	GLCall(glUseProgram(noise.ID()));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//show inital noise
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glPushAttrib(GL_VIEWPORT_BIT);


	shader.Bind();
	GLCall(glUniform1f(glGetUniformLocation(shader.ID(), "u_TexIndex"), 1));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameOfLife::Shutdown()
{
	//delete shaders

	//destructors handle this

	//delete textures
	glDeleteTextures(1, &frontTex);
	glDeleteTextures(1, &backTex);
	glDeleteTextures(1, &renderTarget);
}

float counter = 0.0f;

int backTexture = 1;
int frontTexture = 2;

/*
* This section updates the texture by drawing the state texture (initally noise) to a gamespace quad using the GoL shader, which updates the state accordingly.
* This is performed on buffer object so the texture can be bound to the other texture of the same size.
* Each time this operation is perforemd to the other texture, allowing for timed state updates.
*/
void GameOfLife::Update(float deltaTime)
{
	camera.Update(deltaTime);

	if (Input::Get().IsMousePressed(KC_MOUSE_BUTTON_LEFT))
	{
		glm::vec2 worldPos = camera.ScreenToWorldSpace(glm::vec2(Input::Get().GetMousePos().first, Input::Get().GetMousePos().second));

		//bounds check
		if (worldPos.x > -m_WindowHalfWidth && worldPos.x < m_WindowHalfWidth)
			if (worldPos.y > -m_WindowHalfHeight && worldPos.y < m_WindowHalfHeight)
			{
				glm::vec2 cellPosition = GetGameCell(worldPos);
				std::cout << cellPosition.x << ", " << cellPosition.y << '\n';

				glBindFramebuffer(GL_FRAMEBUFFER, noiseBuffer);

				glPushAttrib(GL_VIEWPORT_BIT);
				glViewport(0, 0, m_GameWidth, m_GameHeight);

				paint.Bind();
				GLCall(glUniform1i(glGetUniformLocation(paint.ID(), "u_State"), backTexture));
				GLCall(glUniform2f(glGetUniformLocation(paint.ID(), "u_Cell"), cellPosition.x, cellPosition.y));

				glBindBuffer(GL_ARRAY_BUFFER, VB2);
				glBindVertexArray(VA2);

				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

				glActiveTexture(GL_TEXTURE0 + backTexture);
				glBindTexture(GL_TEXTURE_2D, noiseBuffer);

				glPopAttrib();
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
	}

	if (Input::Get().IsKeyPressed(KC_KEY_SPACE))
		m_IsPaused = !m_IsPaused;

	if (!m_IsPaused)
		counter += deltaTime;

	if (counter > 0.05f)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, noiseBuffer);

		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0, 0, m_GameWidth, m_GameHeight);

		GoL.Bind();
		GLCall(glUniform1i(glGetUniformLocation(GoL.ID(), "u_State"), backTexture));

		glBindBuffer(GL_ARRAY_BUFFER, VB2);
		glBindVertexArray(VA2);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glActiveTexture(GL_TEXTURE0 + frontTexture);
		glBindTexture(GL_TEXTURE_2D, noiseBuffer);

		glPopAttrib();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		std::swap(backTexture, frontTexture);

		counter = 0;
	}

}

/*
* This section simpy draws the most recent state texture to a screenspace quad
*/
void GameOfLife::Draw(float deltaTime)
{

	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
	shader.Bind();
	GLCall(glUniform1f(glGetUniformLocation(shader.ID(), "u_TexIndex"), backTexture));

	glm::mat4 mvp = camera.GetCamera().GetViewProjectionMatrix() * model;
	GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.ID(), "u_MVP"), 1, GL_FALSE, &mvp[0][0]));
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBindVertexArray(VA);
	//glBindBuffer(GL_ARRAY_BUFFER, VB);
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}

glm::vec2 GameOfLife::GetGameCell(glm::vec2 worldPosition)
{
	return glm::vec2(
		std::floor((std::floor(worldPosition.x)) / (m_Window->GetWidth() / static_cast<float>(m_GameWidth))) + m_GameWidth * 0.5f,
		std::floor((std::floor(worldPosition.y)) / (m_Window->GetHeight() / static_cast<float>(m_GameHeight))) + m_GameHeight * 0.5f
	);
}
