#pragma once

#include "input.h"
#include "screen.h"
#include "terminal.h"

namespace Snake
{
	class Game
	{
		public:
			Game();
			~Game();

			int width() const noexcept;
			int height() const noexcept;

			void run();

		private:
			int m_width;
			int m_height;
			ScreenBuffer m_buffer;
			Terminal m_terminal;

			void drawBorder();
	};
};
