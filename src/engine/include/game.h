#pragma once

#include <chrono>
#include <memory>
#include <utility>

#include "input.h"
#include "screen.h"
#include "terminal.h"
#include "objects.h"

/**
 * @namespace Snake
 * @brief Main namespace for the Snake game engine.
 */
namespace Snake
{
	/**
	 * @brief Alias to `std::vector<std::pair<BaseObject*, BaseObject*>>`
	 */
	using ObjectPairs = std::vector<std::pair<BaseObject*, BaseObject*>>;

	/**
	 * @brief Main game class controling game state, input, and rendering.
	 */
	class Game
	{
		public:
			/**
			 * @brief Construct a new Game object
			 *
			 * Initializes terminal, screen buffer, game objects, and logger.
			 */
			explicit Game();

			/**
			 * @brief Default destructor
			 */
			~Game();

			/**
			 * @brief Runs the main game loop until exit is requested.
			 */
			void run();

		private:
			/**
			 * @brief Game area width resolved by `Snake::Terminal`
			*/
			unsigned int m_width;

			/**
			 * @brief Game area height resolved by `Snake::Terminal`
			*/
			unsigned int m_height;

			/**
			 * @brief Latest input key to be processed in the next frame
			 *
			 * Multiple key presses within a single frame will be ignored; only the last one is kept.
			 */
			Input::KeyKind m_pendingInput = Input::KeyKind::None; // Store latest key

			/**
			 * @brief `Snake::Terminal` instance
			 */
			Terminal m_terminal;

			/**
			 * @brief `Snake::ScreenBuffer` instance
			 */
			ScreenBuffer m_buffer;

			/**
			 * @brief Target frame time in milliseconds (250ms = 4 FPS)
			 *
			 * Lower values will make the game more difficult but suitable for big terminal windows.
			 *
			 * Higher values will make the game easier but suitable for small terminal windows.
			 */
			static constexpr unsigned int s_FrameTimeMs = 250;

			/**
			 * @brief Time point of the last rendered frame
			 *
			 * Used to calculate delta time for frame rate control.
			 */
			std::chrono::steady_clock::time_point m_lastFrameTime;

			/**
			 * @brief Number of frames elapsed since game start
			 *
			 * At the moment only used to determine when to spawn food.
			 */
			unsigned int m_FramesElapsed = 0;

			/**
			 * @brief Frequency of food appearance
			 *
			 * Used to calculate when to spawn food based on `m_FramesElapsed` ( every `s_FoodFreq` frames ).
			 */
			static constexpr unsigned int s_FoodFreq = 5; // frames

			std::unique_ptr<Border> m_border;
			std::unique_ptr<Snake> m_snake;
			std::unique_ptr<Food> m_food;

			/**
			 * @brief Initializes the logging system using `Boost::log`
			 * @throws std::runtime_error if `logging.ini` cannot be opened
			 *
			 * Tries to open `logging.ini` in the executable directory for configuration.
			 */
			void initLogger();

			/**
			 * @brief Updates game state for the current frame
			 * @callgraph
			 *
			 * Updates game objects based on input
			 */
			void update();

			/**
			 * @brief Inserts food at a random empty position in the game area
			 * @callgraph
			 *
			 * Adds a Snake::Food object to `Snake::Game::m_buffer`.
			 */
			void insertFood();

			/**
			 * @brief Removes the current food object from the game area
			 * @callgraph
			 *
			 * Removes the `Snake::Food` object from `Snake::Game::m_buffer`.
			 */
			void removeFood();

			/**
			 * @brief Generates all unique pairs of game objects for collision detection
			 * @callgraph
			 * @param objs Vector of Snake::BaseObject pointers
			 * @return ObjectPairs Vector of unique object pairs
			 *
			 * Also adds self-collision pairs for objects that can collide with themselves (e.g., Snake).
			 */
			static ObjectPairs s_GenerateUniquePairs(std::vector<BaseObject*> const &objs);

			/**
			 * @brief Checks for collisions among object pairs and self-collisions
			 * @callgraph
			 * @param pairs Snake::ObjectPairs Vector of unique object pairs
			 * @return Snake::CollisionResult Result of the first detected collision, or Snake::CollisionResult::NONE if no collisions
			 */
			static CollisionResult s_CheckCollisions(ObjectPairs const &pairs);

			/**
			 * @brief Checks for self-collisions of a single object
			 * @callgraph
			 * @param obj Snake::BaseObject Pointer to the game object to check
			 * @return Snake::CollisionResult Result of the self-collision, or Snake::CollisionResult::NONE if no collision
			 *
			 * Self collision is based on the detector cells of the object
			 */
			static CollisionResult s_CheckSelfCollisions(BaseObject* const &obj);

			/**
			 * @brief Handles the result of a collision
			 * @callgraph
			 * @param result Snake::CollisionResult to handle
			 *
			 * For example, if food is eaten, grow the snake and remove the food.
			 *
			 * If game over, set exit request flag.
			 */
			void handleCollisionResult(CollisionResult result);

			/**
			 * @brief Sets up signal handling for graceful termination on SIGINT
			 *
			 * Registers a signal handler to catch SIGINT (Ctrl+C) and set the exit request flag.
			 *
			 * This has no effect in Windows as signal handling is different.
			 */
			static void s_setupSignalHandling();
	};
};
