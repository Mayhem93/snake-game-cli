#pragma once

#include <cstdint>

namespace Snake
{
	/**
	 * @namespace Snake::TGLYPHS
	 * @brief Contains glyph constants to render game elements in the terminal.
	 */
	namespace TGLYPHS
	{
		constexpr uint32_t SPACE = 0x20;						// ' '
		// BORDER
		constexpr uint32_t HORIZ_DOUBLE_LINE = 0x2550;			// ═
		constexpr uint32_t VERT_DOUBLE_LINE = 0x2551;			// ║
		constexpr uint32_t TOP_LEFT_DOUBLE_CORNER = 0x2554;		// ╔
		constexpr uint32_t TOP_RIGHT_DOUBLE_CORNER = 0x2557;	// ╗
		constexpr uint32_t BOTTOM_LEFT_DOUBLE_CORNER = 0x255A;	// ╚
		constexpr uint32_t BOTTOM_RIGHT_DOUBLE_CORNER = 0x255D; // ╝
		// SNAKE
		constexpr uint32_t SNAKE_BODY = 0x25A3; // ▣
		constexpr uint32_t SNAKE_HEAD_UP = 0x25B2;	 // ▲
		constexpr uint32_t SNAKE_HEAD_DOWN = 0x25BC; // ▼
		constexpr uint32_t SNAKE_HEAD_LEFT = 0x25C0;	 // ◀
		constexpr uint32_t SNAKE_HEAD_RIGHT = 0x25B6;	 // ▶
		constexpr uint32_t SNAKE_TAIL_UP = 0x25B3;	 // △
		constexpr uint32_t SNAKE_TAIL_DOWN = 0x25BD; // ▽
		constexpr uint32_t SNAKE_TAIL_LEFT = 0x25C1; // ◁
		constexpr uint32_t SNAKE_TAIL_RIGHT = 0x25B7; // ▷
		// FOOD
		constexpr uint32_t FOOD = 0x25CE; // ◎
	};
};
