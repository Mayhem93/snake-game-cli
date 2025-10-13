#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <fstream>
#include <filesystem>

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <boost/log/trivial.hpp>

#include "include/game.h"
#include "include/terminal.h"
#include "include/input.h"
#include "include/objects.h"
#include "include/utils.h"

namespace Snake
{
	Game::Game()
	{
		initLogger();

		Game::setupSignalHandling(); // it's not a real cli program if we don't handle SIGINT; does nothing under Windows

		m_terminal = Terminal();

		m_width = m_terminal.width();
		m_height = m_terminal.height();
		m_buffer = ScreenBuffer(m_width, m_height);

		BOOST_LOG_TRIVIAL(info) << "Terminal size: " << m_width << " x " << m_height;

		m_border = std::make_unique<Border>(m_width, m_height);
		m_snake = std::make_unique<Snake>(static_cast<unsigned int>(m_width / 2), static_cast<unsigned int>(m_height / 2));

		m_buffer.addObject(m_border.get());
		m_buffer.addObject(m_snake.get());
	}

	Game::~Game() = default;

	void Game::run()
	{
		while (!Input::g_exitRequested)
		{
			auto currentTime = std::chrono::steady_clock::now();
			auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastFrameTime).count();
			Input::KeyEvent key = Input::readKey();

			if (key.kind == Input::KeyKind::Enter) // alternative exit
				Input::g_exitRequested = true;

			if (key.kind != Input::KeyKind::None)
			{
				m_pendingInput = key.kind;
			}

			if (deltaTime >= s_FrameTimeMs)
			{
				update();
				m_pendingInput = Input::KeyKind::None;
				// At the moment we're hardcoding this vector of objects to check for collisions
				// In future we might want to maintain a list of all active objects in the Game
				std::vector<BaseObject*> testVector;
				testVector.push_back(m_snake.get());
				testVector.push_back(m_border.get());
				if (m_food != nullptr)
				{
					testVector.push_back(m_food.get());
				}

				ObjectPairs testPairs = s_GenerateUniquePairs(testVector);

				handleCollisionResult(s_CheckCollisions(testPairs));

				m_buffer.updateObjects();
				m_terminal.render(m_buffer);

				m_lastFrameTime = currentTime;
				++m_FramesElapsed;
			}

			// avoid busy-waiting
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
	}

	void Game::update()
	{
		if (m_FramesElapsed != 0 && m_FramesElapsed % s_FoodFreq == 0 && m_food == nullptr)
		{
			insertFood();
		}

		switch (m_pendingInput)
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

		m_snake->performMove(); // keep snake continuously moving with current direction
		// m_snake->logCells();
		// m_buffer.dumpBuffer();
	}

	void Game::insertFood()
	{
		// Place food at a random empty position
		unsigned int foodX, foodY;
		do
		{
			foodX = rand() % (m_width - 2) + 1;	 // avoid border
			foodY = rand() % (m_height - 2) + 1; // avoid border
		} while (!m_buffer.isPositionEmpty(foodX, foodY));

		m_food = std::make_unique<Food>(foodX, foodY);
		m_buffer.addObject(m_food.get());
	}

	void Game::removeFood()
	{
		if (m_food != nullptr)
		{
			m_buffer.removeObject(m_food.get());
			m_food = nullptr;
		}
	}

	ObjectPairs Game::s_GenerateUniquePairs(std::vector<BaseObject*> const &objs)
	{
		ObjectPairs pairs;

		for (size_t i = 0; i < objs.size(); ++i)
		{
			for (size_t j = i + 1; j < objs.size(); ++j)
			{
				if (objs[i]->getCollisionType() == CollisionType::NONE &&
					objs[j]->getCollisionType() == CollisionType::NONE)
				{
					continue; // Skip pairs where either object has no collision
				}

				pairs.emplace_back(objs[i], objs[j]);
			}
		}

		return pairs;
	}

	CollisionResult Game::s_CheckCollisions(ObjectPairs const &pairs)
	{
		for (const auto &[obj1, obj2] : pairs)
		{
			// Check if any cells from obj1 overlap with any cells from obj2
			for (const auto &cell1 : obj1->cells())
			{
				for (const auto &cell2 : obj2->cells())
				{
					if (cell1->x == cell2->x && cell1->y == cell2->y)
					{
						// Collision detected! Ask obj1 what should happen
						CollisionResult result = obj1->getCollisionResult(*obj2);

						if (result != CollisionResult::NONE)
						{
							BOOST_LOG_TRIVIAL(info) << "Collision detected! Result: " << static_cast<int>(result);

							return result; // Stop at first collision
						}
					}
				}
			}
		}

		return CollisionResult::NONE; // No collisions detected
	}

	void Game::handleCollisionResult(CollisionResult result)
	{
		// Handle collision results
		switch (result)
		{
			case CollisionResult::POINTS:
				BOOST_LOG_TRIVIAL(info) << "Snake ate food!";

				removeFood();
				m_snake.get()->grow();

				break;

			case CollisionResult::GAME_OVER:
				BOOST_LOG_TRIVIAL(info) << "Game Over!";

				Input::g_exitRequested = true; // End the game
				break;

			case CollisionResult::NONE:
				// Continue normally
				break;
		}
	}

	void Game::initLogger()
	{
		boost::log::add_common_attributes();

		std::filesystem::path exePath = Utils::getExecutablePath();
		std::filesystem::path iniPath = exePath.parent_path() / "logging.ini";

		std::ifstream cfg(iniPath);

		if (!cfg.is_open())
			throw std::runtime_error("Failed to open logging.ini");

		boost::log::init_from_stream(cfg);

		BOOST_LOG_TRIVIAL(info) << "Logger initialized";
	}

	void Game::setupSignalHandling()
	{
		std::signal(SIGINT, Input::signalHandler);
	}
};
