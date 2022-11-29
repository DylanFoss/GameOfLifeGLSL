#include "GameOfLife.h"
#include <iostream>

void GameOfLife::Update(float deltaTime)
{
	std::cout << deltaTime << '\n';
}

void GameOfLife::Draw(float deltaTime)
{
	std::cout << "Drawn!" << '\n';
}
