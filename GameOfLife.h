#pragma once
#include "GL.h"
#include "GLUtils.h"

class GameOfLife :
    public Application
{
public:
    GameOfLife(const std::string& name, uint32_t width, uint32_t height, uint32_t gameWidth, uint32_t gameHeight);
    ~GameOfLife();

    void Init();
    void Shutdown();

    void Update(float deltatime);
    void Draw(float deltatime);

private:

    uint32_t m_GameWidth;
    uint32_t m_GameHeight;

    int32_t m_WindowHalfWidth;
    int32_t m_WindowHalfHeight;

    Utils::OrthographicCameraController camera = Utils::OrthographicCameraController(400.0f);

    bool m_IsPaused = false;

    glm::vec2 GetGameCell(glm::vec2 worldPosition);
};


