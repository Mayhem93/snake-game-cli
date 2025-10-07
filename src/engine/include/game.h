#pragma once

#include <chrono>
#include <memory>
#include <utility>

#include "input.h"
#include "screen.h"
#include "terminal.h"
#include "objects.h"

namespace Snake
{
	using ObjectPairs = std::vector<std::pair<BaseObject*, BaseObject*>>;

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

			static constexpr int s_FrameTimeMs = 500;
			std::chrono::steady_clock::time_point m_lastFrameTime;
			unsigned int m_FramesElapsed = 0;

			static constexpr unsigned int s_FoodFreq = 5; // frames

			std::unique_ptr<Border> m_border;
			std::unique_ptr<Snake> m_snake;
			std::unique_ptr<Food> m_food;

			void initLogger();
			void update();
			void insertFood();
			static ObjectPairs s_GenerateUniquePairs(std::vector<BaseObject*> const& objs);
			static void setupSignalHandling();
	};
};
