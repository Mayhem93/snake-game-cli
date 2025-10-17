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
		bool initStdinRaw();
		void restoreTerminal();
		void signalHandler(int signal);

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
