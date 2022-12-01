#pragma once
#include "GL/glew.h"
#include "Application.h"

class GameOfLife :
    public Application
{
public:
    GameOfLife(const std::string& name, uint32_t width, uint32_t height);

    void Init();
    void Shutdown();

    void Update(float deltatime);
    void Draw(float deltatime);
};


