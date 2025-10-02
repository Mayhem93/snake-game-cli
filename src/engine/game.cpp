#include <iostream>

#include "include/game.h"
#include "include/terminal.h"
#include "include/input.h"
#include "include/objects.h"

#include <thread>
#include <chrono>
#include <csignal>

namespace Snake
{
	Game::Game() :
		m_terminal()
	{
		m_width = m_terminal.width();
		m_height = m_terminal.height();
		m_buffer = ScreenBuffer(m_width, m_height);

		m_border = std::make_unique<Border>(m_width, m_height);

		m_buffer.addObject(m_border.get());
	}

	Game::~Game() = default;

	void Game::run()
	{
		if (!Input::initStdinRaw())
		{
			std::cerr << "Failed to initialize stdin in raw mode" << std::endl;
			exit(1);
		}

		std::signal(SIGINT, Input::signalHandler);

		while (!Input::g_exitRequested)
		{
			auto currentTime = std::chrono::steady_clock::now();
			auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastFrameTime).count();
			auto key = Input::readKey();

			if (key.kind == Input::KeyKind::Enter)
				break;

			if (deltaTime >= FRAME_TIME_MS)
			{
				update();
				m_terminal.render(m_buffer);
				m_lastFrameTime = currentTime;
			}

			// Optionally, sleep for a short time to avoid busy-waiting
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	void Game::update()
	{
		m_buffer.clear();
		m_buffer.addObject(m_border.get());
	}
};
