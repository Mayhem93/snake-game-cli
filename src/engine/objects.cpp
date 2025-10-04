#include <vector>

#include <boost/log/trivial.hpp>

#include "objects.h"
#include "screen.h"
#include "terminal.h"

namespace Snake
{
	BaseObject::BaseObject() = default;

	const std::vector<std::unique_ptr<PositionedCell>>& BaseObject::cells() const
	{
		return m_cells;
	}

	bool BaseObject::isCollisionEnabled() const
	{
		return m_collisionEnabled;
	}

	void BaseObject::addPCell(PCellPtr& pCell)
	{
		m_cells.push_back(std::move(pCell));
	}

	CellPtr BaseObject::s_MakeCell(const Cell& cell)
	{
	    return std::make_shared<Cell>(cell);
	}

	PCellPtr BaseObject::s_MakePCell(unsigned int x, unsigned int y, CellPtr cell)
	{
	    return std::make_unique<PositionedCell>(PositionedCell{ x, y, cell });
	}

	Border::Border(unsigned int width, unsigned int height)
		: BaseObject()
	{
		m_collisionEnabled = true;

		// Top and bottom rows
		for (unsigned int x = 1; x < width - 1; ++x)
		{
			auto pTopCell = s_MakePCell(x, 0, s_MakeCell(Cell{ .codepoint = TGLYPHS::HORIZ_DOUBLE_LINE }));
			addPCell(pTopCell);

			auto pBottomCell = s_MakePCell(x, height - 1, s_MakeCell(Cell{ .codepoint = TGLYPHS::HORIZ_DOUBLE_LINE }));
			addPCell(pBottomCell);
		}
		// Left and right columns
		for (unsigned int y = 1; y < height - 1; ++y)
		{
			auto pLeftCell = s_MakePCell(0, y, s_MakeCell(Cell{ .codepoint = TGLYPHS::VERT_DOUBLE_LINE }));
			addPCell(pLeftCell);

			auto pRightCell = s_MakePCell(width - 1, y, s_MakeCell(Cell{ .codepoint = TGLYPHS::VERT_DOUBLE_LINE }));
			addPCell(pRightCell);
		}
		// Corners
		auto pTopLeftCell = s_MakePCell(0, 0, s_MakeCell(Cell{ .codepoint = TGLYPHS::TOP_LEFT_DOUBLE_CORNER }));
		addPCell(pTopLeftCell);

		auto pTopRightCell = s_MakePCell(width - 1, 0, s_MakeCell(Cell{ .codepoint = TGLYPHS::TOP_RIGHT_DOUBLE_CORNER }));
		addPCell(pTopRightCell);

		auto pBottomLeftCell = s_MakePCell(0, height - 1, s_MakeCell(Cell{ .codepoint = TGLYPHS::BOTTOM_LEFT_DOUBLE_CORNER }));
		addPCell(pBottomLeftCell);

		auto pBottomRightCell = s_MakePCell(width - 1, height - 1, s_MakeCell(Cell{ .codepoint = TGLYPHS::BOTTOM_RIGHT_DOUBLE_CORNER }));
		addPCell(pBottomRightCell);
	}

	Snake::Snake(unsigned int startX, unsigned int startY)
		: BaseObject()
	{
		m_collisionEnabled = true;

		auto pHeadCell = s_MakePCell(startX, startY, s_MakeCell(Cell{ .codepoint = TGLYPHS::SNAKE_HEAD_LEFT }));
		addPCell(pHeadCell);

		for (unsigned int i = 1; i <= m_length - 2; ++i) {
			auto pBodyCell = s_MakePCell(startX + i, startY, s_MakeCell(Cell{ .codepoint = TGLYPHS::SNAKE_BODY }));
			addPCell(pBodyCell);
		}

		auto pTailCell = s_MakePCell(startX + m_length - 1, startY, s_MakeCell(Cell{ .codepoint = TGLYPHS::SNAKE_TAIL_RIGHT }));
		addPCell(pTailCell);
	}

	void Snake::setDirection(Direction direction)
	{
		// Prevent reversing direction
		if ((m_currentDirection == Direction::Up && direction == Direction::Down) ||
			(m_currentDirection == Direction::Down && direction == Direction::Up) ||
			(m_currentDirection == Direction::Left && direction == Direction::Right) ||
			(m_currentDirection == Direction::Right && direction == Direction::Left))
		{
			return;
		}

		m_currentDirection = direction;
	}

	void Snake::move()
	{
		if (m_cells.empty())
			return;

		// Step 1: Move each segment to follow the one in front of it (from tail to head)
		for (int i = m_cells.size() - 1; i > 0; --i)
		{
			m_cells[i]->x = m_cells[i - 1]->x;
			m_cells[i]->y = m_cells[i - 1]->y;
		}

		// Step 2: Move the head based on current direction
		switch (m_currentDirection)
		{
		case Direction::Up:
			m_cells[0]->y--;
			break;
		case Direction::Down:
			m_cells[0]->y++;
			break;
		case Direction::Left:
			m_cells[0]->x--;
			break;
		case Direction::Right:
			m_cells[0]->x++;
			break;
		}

		// Step 3: Update head glyph based on direction
		switch (m_currentDirection)
		{
		case Direction::Up:
			m_cells[0]->cell->codepoint = TGLYPHS::SNAKE_HEAD_UP;
			break;
		case Direction::Down:
			m_cells[0]->cell->codepoint = TGLYPHS::SNAKE_HEAD_DOWN;
			break;
		case Direction::Left:
			m_cells[0]->cell->codepoint = TGLYPHS::SNAKE_HEAD_LEFT;
			break;
		case Direction::Right:
			m_cells[0]->cell->codepoint = TGLYPHS::SNAKE_HEAD_RIGHT;
			break;
		}

		// Step 4: Update tail glyph based on direction (direction from second-to-last to last segment)
		if (m_cells.size() > 1)
		{
			int tailIndex = m_cells.size() - 1;
			int prevIndex = tailIndex - 1;

			int dx = m_cells[tailIndex]->x - m_cells[prevIndex]->x;
			int dy = m_cells[tailIndex]->y - m_cells[prevIndex]->y;

			if (dx > 0) // Tail is to the right of previous segment
				m_cells[tailIndex]->cell->codepoint = TGLYPHS::SNAKE_TAIL_RIGHT;
			else if (dx < 0) // Tail is to the left of previous segment
				m_cells[tailIndex]->cell->codepoint = TGLYPHS::SNAKE_TAIL_LEFT;
			else if (dy > 0) // Tail is below previous segment
				m_cells[tailIndex]->cell->codepoint = TGLYPHS::SNAKE_TAIL_DOWN;
			else if (dy < 0) // Tail is above previous segment
				m_cells[tailIndex]->cell->codepoint = TGLYPHS::SNAKE_TAIL_UP;
		}
	}

	void Snake::up()
	{
		setDirection(Direction::Up);
	}

	void Snake::down()
	{
		setDirection(Direction::Down);
	}

	void Snake::left()
	{
		setDirection(Direction::Left);
	}

	void Snake::right()
	{
		setDirection(Direction::Right);
	}

	void Snake::logCells() const
	{
		for (const auto& cell : m_cells) {
			BOOST_LOG_TRIVIAL(info) << "Cell at (" << cell->x << ", " << cell->y << ")";
		}
	}
};
