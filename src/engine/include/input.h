#pragma once

#include <cstdint>
#include <atomic>

namespace Snake
{
	/**
	 * @namespace Snake::Input
	 * @brief Namespace for handling user input.
	 */
	namespace Input
	{
		/**
		 * @brief Attempts to set stdin to raw mode for non-blocking input reading
		 * @return true if successful, false otherwise
		 */
		bool initStdinRaw();

		/**
		 * @brief Restores stdin to its original mode
		 *
		 * Uses termios to restore terminal settings on Unix-like systems.
		 */
		void restoreTerminal();

		/**
		 * @brief Signal handler for graceful termination on SIGINT
		 * @param signal Signal number received
		 *
		 * Sets the exit request flag when SIGINT is received. Only works on Unix-like systems.
		 */
		void signalHandler(int signal);

		/**
		 * @brief Global flag indicating if an exit has been requested
		 *
		 * Set to true when SIGINT is received or when the user requests exit via input.
		 */
		extern std::atomic<bool> g_exitRequested;

		enum class KeyKind : uint8_t
		{
			None = 0,
			Char = 1, // printable ASCII/UTF-8 byte
			Enter = 2,
			EscapeKey = 3,
			ArrowUp = 4,
			ArrowDown = 5,
			ArrowLeft = 6,
			ArrowRight = 7,
			// add more as needed: Home, End, F1…F12, etc.
		};

		struct KeyEvent
		{
			KeyKind kind;
			char32_t codepoint;
		};

		KeyEvent readKey();
	}
};
