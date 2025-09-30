#include <vector>

#include "objects.h"
#include "screen.h"
#include "terminal.h"

namespace Snake
{
	BaseObject::BaseObject(int width, int height) :
		m_width(width),
		m_height(height)
	{}

	Border::Border(int width, int height)
		: BaseObject(width, height)
	{
		// Top and bottom rows
		for (int x = 1; x < m_width - 1; ++x)
		{
			m_cells.push_back({x, 0, Cell{ .codepoint = TGLYPHS::HORIZ_DOUBLE_LINE }});
			m_cells.push_back({x, m_height - 1, Cell{ .codepoint = TGLYPHS::HORIZ_DOUBLE_LINE }});
		}
		// Left and right columns
		for (int y = 1; y < m_height - 1; ++y)
		{
			m_cells.push_back({0, y, Cell{ .codepoint = TGLYPHS::VERT_DOUBLE_LINE }});
			m_cells.push_back({m_width - 1, y, Cell{ .codepoint = TGLYPHS::VERT_DOUBLE_LINE }});
		}
		// Corners
		m_cells.push_back({0, 0, Cell{ .codepoint = TGLYPHS::TOP_LEFT_DOUBLE_CORNER }});
		m_cells.push_back({m_width - 1, 0, Cell{ .codepoint = TGLYPHS::TOP_RIGHT_DOUBLE_CORNER }});
		m_cells.push_back({0, m_height - 1, Cell{ .codepoint = TGLYPHS::BOTTOM_LEFT_DOUBLE_CORNER }});
		m_cells.push_back({m_width - 1, m_height - 1, Cell{ .codepoint = TGLYPHS::BOTTOM_RIGHT_DOUBLE_CORNER }});
	}

	const std::vector<PositionedCell>& Border::cells() const
	{
		return m_cells;
	}
};
