#pragma once

#include <chrono>
#include <memory>

#include "input.h"
#include "screen.h"
#include "terminal.h"
#include "objects.h"

namespace Snake
{
	class Game
	{
		public:
			Game();
			~Game();

			void run();

		private:
			int m_width;
			int m_height;
			Input::KeyKind m_pendingInput = Input::KeyKind::None; // Store latest key
			ScreenBuffer m_buffer;
			Terminal m_terminal;

			static constexpr int FRAME_TIME_MS = 1000;
			std::chrono::steady_clock::time_point m_lastFrameTime;

			std::unique_ptr<Border> m_border;
			std::unique_ptr<Snake> m_snake;

			void initLogger();
			void update(Input::KeyKind input);
	};
};
