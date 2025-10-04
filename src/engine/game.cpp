#include <iostream>

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <boost/log/trivial.hpp>

#include "include/game.h"
#include "include/terminal.h"
#include "include/input.h"
#include "include/objects.h"

#include <thread>
#include <chrono>
#include <csignal>
#include <fstream>
#include <filesystem>

namespace Snake
{
	Game::Game() :
		m_terminal()
	{
		initLogger();
		m_width = m_terminal.width();
		m_height = m_terminal.height();
		m_buffer = ScreenBuffer(m_width, m_height);

		m_border = std::make_unique<Border>(m_width, m_height);
		m_snake = std::make_unique<Snake>(m_width / 2, m_height / 2);

		m_snake->logCells();

		m_buffer.addObject(m_border.get());
		m_buffer.addObject(m_snake.get());
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
			Input::KeyEvent key = Input::readKey();

			if (key.kind == Input::KeyKind::Enter) // alternative exit
				break;

			if (key.kind != Input::KeyKind::None)
			{
				m_pendingInput = key.kind;
			}

			if (deltaTime >= FRAME_TIME_MS)
			{
				update(m_pendingInput);
				m_pendingInput = Input::KeyKind::None;
				m_buffer.updateObjects();
				m_terminal.render(m_buffer);
				m_lastFrameTime = currentTime;
			}

			// Optionally, sleep for a short time to avoid busy-waiting
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
	}

	void Game::update(Input::KeyKind input)
	{
		BOOST_LOG_TRIVIAL(info) << "Game update with input: " << static_cast<int>(input);

		switch (input)
		{
			case Input::KeyKind::ArrowUp:
				m_snake->up();
				break;
			case Input::KeyKind::ArrowDown:
				m_snake->down();
				break;
			case Input::KeyKind::ArrowLeft:
				m_snake->left();
				break;
			case Input::KeyKind::ArrowRight:
				m_snake->right();
				break;
			default:
				// No action for other keys
				break;
		}

		m_snake->move();
		// m_snake->logCells();
		// m_buffer.dumpBuffer();
	}

	void Game::initLogger()
	{
		boost::log::add_common_attributes();

		std::filesystem::path exePath = std::filesystem::canonical("/proc/self/exe");
		std::filesystem::path iniPath = exePath.parent_path() / "logging.ini";

		std::ifstream cfg(iniPath);

		if (!cfg.is_open())
			throw std::runtime_error("Failed to open logging.ini");

		boost::log::init_from_stream(cfg);

		BOOST_LOG_TRIVIAL(info) << "Logger initialized";
	}
};
