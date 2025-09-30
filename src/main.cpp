#include <iostream>

#include "engine/include/game.h"

int main ()
{
	Snake::Game g = Snake::Game();
	// Snake::Border border(g.width(), g.height());

	g.run();

	return 0;
}
