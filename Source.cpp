#include "GameOfLife.h"
int main(int argc, char* argv[])
{
	int initArray[4] = { 800, 800, 400, 400 };

	if (argc > 1)
	{
		if (argc < 5)
		{
			for (int i = 1; i < argc; i++)
			{
				initArray[i - 1] = std::stoi(argv[i]);
			}
		}
	}

	std::unique_ptr<GameOfLife> app = std::make_unique<GameOfLife>("Game Of Life", initArray[0], initArray[1], initArray[2], initArray[3]);
	app->Run();

	return 0;
}