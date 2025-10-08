#pragma once

#include <string>

#if defined(_WIN32)
#include <windows.h>
#endif

#include "screen.h"

namespace Snake
{
	class Terminal
	{
		public:
			Terminal();
			~Terminal();
			void clearScreen();
			void render(ScreenBuffer const& buffer);
			void hideCursor();
			void showCursor();
			void moveCursor(int row, int col);
			int width() const noexcept;
			int height() const noexcept;

		private:
			int m_width = 0;
			int m_height = 0;
			static std::string toUnicode(uint32_t codepoint) noexcept;
			void recoverFromOutputFailure();

#ifdef _WIN32
			HANDLE m_hStdin;
			HANDLE m_hStdout;
			DWORD m_originalInputMode;
			DWORD m_originalOutputMode;
#endif
	};

	namespace TSEQ
	{
		constexpr const char* ESC = "\x1b";
		constexpr const char* ALTERNATE_SCREEN = "\x1b[?1049h";
		constexpr const char* EXIT_ALTERNATE_SCREEN = "\x1b[?1049l";
		constexpr const char* CLEAR_SCREEN = "\x1b[2J";
		constexpr const char* CURSOR_HOME = "\x1b[H";
		constexpr const char* HIDE_CURSOR = "\x1b[?25l";
		constexpr const char* SHOW_CURSOR = "\x1b[?25h";
		constexpr const char* RESET_ATTRS = "\x1b[0m";
		constexpr const char* FG_COLOR_256 = "\x1b[38;5;";
		constexpr const char* BG_COLOR_256 = "\x1b[48;5;";
		constexpr const char* DEFAULT_BACKGROUND = "\x1b[49m";
		constexpr const char* DEFAULT_FOREGROUND = "\x1b[39m";
	};
};
