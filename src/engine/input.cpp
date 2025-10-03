#include "include/input.h"

#include <csignal>
#include <unordered_map>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace Snake
{
	namespace Input
	{
		std::atomic<bool> g_exitRequested{false};
		static const std::unordered_map<int, KeyKind> g_keyMap = {
			{'A', KeyKind::ArrowUp},
			{'B', KeyKind::ArrowDown},
			{'C', KeyKind::ArrowRight},
			{'D', KeyKind::ArrowLeft}
		};

#ifndef _WIN32
		static termios g_originalTermios;
		static bool g_termiosSaved = false;
#endif

#if defined(_WIN32)
		bool initStdinRaw()
		{
			HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
			if (hIn == INVALID_HANDLE_VALUE)
				return false;

			DWORD mode = 0;
			if (!GetConsoleMode(hIn, &mode))
				return false;

			// disable line input & echo
			mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
			if (!SetConsoleMode(hIn, mode))
				return false;

			return true;
		}
#else
		bool initStdinRaw()
		{
			if (tcgetattr(STDIN_FILENO, &g_originalTermios) != 0)
				return false;

			g_termiosSaved = true;

			termios t = g_originalTermios; // Copy original
			// disable canonical mode & echo
			t.c_lflag &= ~(ICANON | ECHO);
			t.c_cc[VMIN] = 0;
			t.c_cc[VTIME] = 1;

			if (tcsetattr(STDIN_FILENO, TCSANOW, &t) != 0)
				return false;

			// make non-blocking
			int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
			fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

			return true;
		}
#endif

		KeyEvent readKey()
		{
			KeyEvent ev{KeyKind::None, 0};

#if defined(_WIN32)
			if (!_kbhit())
				return ev;
			int c = _getch();

			// arrow keys come as two‐byte codes: 0 or 0xE0, then code
			if (c == 0 || c == 0xE0)
			{
				int c2 = _getch();
				switch (c2)
				{
				case 72:
					return {KeyKind::ArrowUp, 0};
				case 80:
					return {KeyKind::ArrowDown, 0};
				case 75:
					return {KeyKind::ArrowLeft, 0};
				case 77:
					return {KeyKind::ArrowRight, 0};
				default:
					return ev;
				}
			}

			if (c == '\r' || c == '\n')
				return {KeyKind::Enter, 0};
			if (c == 27)
				return {KeyKind::EscapeKey, 0};

			// printable ASCII or extended byte
			return {KeyKind::Char, static_cast<char32_t>(c)};

#else
			unsigned char buf[3];
			ssize_t n = ::read(STDIN_FILENO, buf, 1);

			if (n <= 0)
				return ev;

			// ESC sequence?
			if (buf[0] == 0x1B)
			{
				// try reading “[?” and arrow code
				if (::read(STDIN_FILENO, buf + 1, 2) == 2 && buf[1] == '[')
				{
					auto it = g_keyMap.find(buf[2]);

					if (it != g_keyMap.end())
					{
						return { it->second, 0 };
					}
				}

				// standalone ESC
				return { KeyKind::EscapeKey, 0 };
			}

			if (buf[0] == '\r' || buf[0] == '\n')
				return { KeyKind::Enter, 0 };

			// any other byte becomes a Char
			return KeyEvent{ KeyKind::Char, static_cast<char32_t>(buf[0]) };
#endif
		}

		void signalHandler(int signal)
		{
			if (signal == SIGINT)
			{
				g_exitRequested = true;
			}
		}

		void restoreTerminal()
		{
#ifndef _WIN32
			if (g_termiosSaved)
			{
				tcsetattr(STDIN_FILENO, TCSANOW, &g_originalTermios);
			}
#endif
		}
	}
}
