#include "GameOfLife.h"
int main(int argc, char* argv[])
{
	int initArray[4] = { 800, 800, 800, 800 };

	if (argc > 1)
	{
		if (argc < 6)
		{
			int cacheArray[4];
			for (int i = 1; i < argc; i++)
			{
				try
				{
					int argValue = std::stoi(argv[i]);
					if (argValue > 0)
					{
						cacheArray[i - 1] = argValue;
					}
					else
					{
						throw std::invalid_argument("Negative value used");
					}
				}
				catch (const std::exception e)
				{
					std::cout << "ERROR: Argv element(s) included non-integer or negative values, using default values.\n";
					for (int i = 0; i < 4; i++)
						cacheArray[i] = initArray[i];
					break;
				}
			}
			for (int i = 0; i < 4; i++)
				initArray[i] = cacheArray[i];
		}
	}

	std::unique_ptr<GameOfLife> app = std::make_unique<GameOfLife>("Game Of Life", initArray[0], initArray[1], initArray[2], initArray[3]);
	app->Run();

	return 0;
}