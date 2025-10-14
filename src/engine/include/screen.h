#pragma once

#include<cstdint>
#include<vector>
#include<memory>

#include "glyphs.h"

namespace Snake
{
	class BaseObject;
	using PosVector = std::vector<std::pair<unsigned int, unsigned int>>;

	struct Cell
	{
		uint32_t codepoint = TGLYPHS::SPACE;
		uint8_t fg = 0xFF;
		uint8_t bg = 0xFF;
		uint8_t attrs = 0;
		bool default_bg = true; // true: use terminal default background, false: use specified bg color
		bool default_fg = true; // true: use terminal default foreground, false: use specified fg color
		bool detector = false; // true: this cell is used for collision detection
		constexpr bool operator==(Cell const& o) const noexcept = default;
		constexpr bool operator!=(Cell const& o) const noexcept;
	};

	using CellPtr = std::shared_ptr<Cell>;

	struct PositionedCell
	{
		unsigned int x;
		unsigned int y;
		CellPtr cell;
	};

	using PCellPtr = std::unique_ptr<PositionedCell>;

	class ScreenBuffer
	{
		public:
			ScreenBuffer() = default;
			ScreenBuffer(int width, int height);

			int width() const noexcept;
			int height() const noexcept;

			void clear();

			void set(int x, int y, const CellPtr& c) noexcept;
			CellPtr get(int x, int y) const noexcept;

			void addObject(BaseObject* obj);
			void removeObject(BaseObject* obj);
			void updateObjects();
			bool isPositionEmpty(unsigned int x, unsigned int y) const;
			PosVector getPositionsToClear() const;
			void clearPositions(const PosVector& positions);
			void dumpBuffer() const;

		private:
			CellPtr m_emptyCell;
			int m_width = 0;
			int m_height = 0;
			std::vector<CellPtr> m_buffer;
			std::vector<BaseObject*> m_objects;
			static std::string s_ToUnicode(uint32_t codepoint) noexcept;

			inline int index(int x, int y) const noexcept;
	};
};
