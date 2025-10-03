#pragma once

#include<cstdint>
#include<string_view>
#include<vector>

#include "glyphs.h"

namespace Snake
{
	class BaseObject;

	struct Cell
	{
		uint32_t codepoint = TGLYPHS::SPACE;
		uint8_t fg = 0xFF;
		uint8_t bg = 0xFF;
		uint8_t attrs = 0;
		bool default_bg = true; // true: use terminal default background, false: use specified bg color
		bool default_fg = true; // true: use terminal default foreground, false: use specified fg color
		constexpr bool operator==(Cell const& o) const noexcept = default;
		constexpr bool operator!=(Cell const& o) const noexcept;
	};

	struct PositionedCell
	{
		unsigned int x;
		unsigned int y;
		Cell cell;
	};

	class ScreenBuffer
	{
		public:
			ScreenBuffer() = default;
			ScreenBuffer(int width, int height);

			int width() const noexcept;
			int height() const noexcept;

			void clear();

			void set(int x, int y, Cell c) noexcept;
			const Cell& get(int x, int y) const noexcept;

			// Cell& atIndex(int idx) const noexcept;

			void addObject(const BaseObject* obj);

		private:
			int m_width = 0;
			int m_height = 0;
			std::vector<Cell> m_buffer;

			inline int index(int x, int y) const noexcept;
	};
};
