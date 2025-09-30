#include <iostream>

#include "include/game.h"
#include "include/terminal.h"
#include "include/input.h"
#include "include/objects.h"

#include <thread>
#include <chrono>

namespace Snake
{
	Game::Game()
	{
		if (!initStdinRaw())
		{
			std::cerr << "Failed to initialize stdin in raw mode" << std::endl;
			exit(1);
		}

		m_terminal = Terminal();

		m_width = m_terminal.width();
		m_height = m_terminal.height();
		m_buffer = ScreenBuffer(m_width, m_height);

		const Border border(m_width, m_height);

		m_buffer.addObject(border);
		m_terminal.render(m_buffer);
	}

	Game::~Game()
	{

	}

	int Game::width() const noexcept
	{
		return m_width;
	}

	int Game::height() const noexcept
	{
		return m_height;
	}

	void Game::run()
	{
		while (true)
		{
			auto key = Snake::readKey();

			if (key.kind == Snake::KeyKind::Enter)
				break;
			// Optionally, sleep for a short time to avoid busy-waiting
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
};
