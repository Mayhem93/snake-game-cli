#include <iostream>

#include <string>

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "include/terminal.h"
#include "include/screen.h"

namespace Snake
{
	Terminal::Terminal()
	{
#if defined(_WIN32)
		m_hStdin = GetStdHandle(STD_INPUT_HANDLE);
		m_hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

		// Save original console modes
		GetConsoleMode(m_hStdin, &m_originalInputMode);
		GetConsoleMode(m_hStdout, &m_originalOutputMode);

		// Set up console for raw input and proper output
		SetConsoleMode(m_hStdin, ENABLE_VIRTUAL_TERMINAL_INPUT);
		SetConsoleMode(m_hStdout, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

		// Get terminal size
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (GetConsoleScreenBufferInfo(m_hStdout, &csbi))
		{
			m_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
			m_height = csbi.srWindow.Bottom - csbi.srWindow.Top;
		}
#else
		struct winsize w;
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
		{
			m_width = w.ws_col;
			m_height = w.ws_row - 1;
		}
#endif
		clearScreen();
		hideCursor();
		std::cout << TSEQ::ALTERNATE_SCREEN; // Enter alternate screen buffer
	}

	Terminal::~Terminal()
	{
		showCursor();
		std::cout << TSEQ::RESET_ATTRS;
#ifdef _WIN32
		SetConsoleMode(m_hStdin, m_originalInputMode);
		SetConsoleMode(m_hStdout, m_originalOutputMode);
#else
		clearScreen();
		std::cout << TSEQ::EXIT_ALTERNATE_SCREEN; // Exit alternate screen buffer
		std::cout.flush();
#endif
	}

	int Terminal::width() const noexcept
	{
		return m_width;
	}

	int Terminal::height() const noexcept
	{
		return m_height;
	}

	void Terminal::clearScreen()
	{
		std::cout << TSEQ::CLEAR_SCREEN << TSEQ::CURSOR_HOME;
	}

	void Terminal::hideCursor()
	{
		std::cout << TSEQ::HIDE_CURSOR;
	}

	void Terminal::showCursor()
	{
		std::cout << TSEQ::SHOW_CURSOR;
	}

	void Terminal::moveCursor(int row, int col)
	{
		std::cout << "\033[" << (row + 1) << ";" << (col + 1) << "H";
	}

	std::string Terminal::toUnicode(uint32_t codepoint) noexcept
	{
		std::string out;

		if (codepoint < 0x80)
		{
			out += static_cast<char>(codepoint);
		}
		else if (codepoint < 0x800)
		{
			out += static_cast<char>(0xC0 | (codepoint >> 6));
			out += static_cast<char>(0x80 | (codepoint & 0x3F));
		}
		else if (codepoint < 0x10000)
		{
			out += static_cast<char>(0xE0 | (codepoint >> 12));
			out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
			out += static_cast<char>(0x80 | (codepoint & 0x3F));
		}
		else
		{
			out += static_cast<char>(0xF0 | (codepoint >> 18));
			out += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
			out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
			out += static_cast<char>(0x80 | (codepoint & 0x3F));
		}

		return out;
	}

	void Terminal::render(ScreenBuffer const& buf)
	{
		for (int y = 0; y < m_height; ++y)
		{
			moveCursor(y, 0);

			for (int x = 0; x < m_width; ++x)
			{
				const Cell& cell = buf.get(x, y);

				if (cell.default_fg) {
				    std::cout << TSEQ::DEFAULT_FOREGROUND; // default foreground
				} else {
				    std::cout << TSEQ::FG_COLOR_256 << static_cast<int>(cell.fg) << "m";
				}

				if (cell.default_bg) {
				    std::cout << TSEQ::DEFAULT_BACKGROUND; // default background
				} else {
				    std::cout << TSEQ::BG_COLOR_256 << static_cast<int>(cell.bg) << "m";
				}

				std::cout << toUnicode(cell.codepoint);
			}
		}

		hideCursor();
		std::cout.flush();
	}
}
