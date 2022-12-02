#pragma once
#include "GL.h"
#include "GLUtils.h"

class GameOfLife :
    public Application
{
public:
    GameOfLife(const std::string& name, uint32_t width, uint32_t height);

    void Init();
    void Shutdown();

    void Update(float deltatime);
    void Draw(float deltatime);

private:
};


