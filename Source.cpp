#include "GameOfLife.h"
int main()
{
	std::unique_ptr<GameOfLife> app = std::make_unique<GameOfLife>("Game Of Life", 800, 800, 40, 40);
	app->Run();

	return 0;
}