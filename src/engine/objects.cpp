#include <vector>

#include <boost/log/trivial.hpp>

#include "objects.h"
#include "screen.h"
#include "terminal.h"

namespace Snake
{
	BaseObject::BaseObject(CollisionType colType)
		: m_collisionType(colType) {}

	const std::vector<std::unique_ptr<PositionedCell>>& BaseObject::cells() const
	{
		return m_cells;
	}

	CollisionType BaseObject::getCollisionType() const
	{
		return m_collisionType;
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

	Border::Border(unsigned int width, unsigned int height) :
		BaseObject(CollisionType::SOLID)
	{
		// Top and bottom rows
		for (unsigned int x = 1; x < width - 1; ++x)
		{
			PCellPtr pTopCell = s_MakePCell(x, 0, s_MakeCell(Cell{ .codepoint = TGLYPHS::HORIZ_DOUBLE_LINE }));
			addPCell(pTopCell);

			PCellPtr pBottomCell = s_MakePCell(x, height - 1, s_MakeCell(Cell{ .codepoint = TGLYPHS::HORIZ_DOUBLE_LINE }));
			addPCell(pBottomCell);
		}

		// Left and right columns
		for (unsigned int y = 1; y < height - 1; ++y)
		{
			PCellPtr pLeftCell = s_MakePCell(0, y, s_MakeCell(Cell{ .codepoint = TGLYPHS::VERT_DOUBLE_LINE }));
			addPCell(pLeftCell);

			PCellPtr pRightCell = s_MakePCell(width - 1, y, s_MakeCell(Cell{ .codepoint = TGLYPHS::VERT_DOUBLE_LINE }));
			addPCell(pRightCell);
		}

		// Corners
		PCellPtr pTopLeftCell = s_MakePCell(0, 0, s_MakeCell(Cell{ .codepoint = TGLYPHS::TOP_LEFT_DOUBLE_CORNER }));
		addPCell(pTopLeftCell);

		PCellPtr pTopRightCell = s_MakePCell(width - 1, 0, s_MakeCell(Cell{ .codepoint = TGLYPHS::TOP_RIGHT_DOUBLE_CORNER }));
		addPCell(pTopRightCell);

		PCellPtr pBottomLeftCell = s_MakePCell(0, height - 1, s_MakeCell(Cell{ .codepoint = TGLYPHS::BOTTOM_LEFT_DOUBLE_CORNER }));
		addPCell(pBottomLeftCell);

		PCellPtr pBottomRightCell = s_MakePCell(width - 1, height - 1, s_MakeCell(Cell{ .codepoint = TGLYPHS::BOTTOM_RIGHT_DOUBLE_CORNER }));
		addPCell(pBottomRightCell);
	}

	CollisionResult Border::getCollisionResult(BaseObject const& other) const
	{
		if (other.getCollisionType() == CollisionType::SELF ||
			other.getCollisionType() == CollisionType::SOLID)
		{
			return CollisionResult::GAME_OVER; // e.g. hit wall
		}

		return CollisionResult::NONE; // No collision
	}

	Snake::Snake(unsigned int startX, unsigned int startY)
		: BaseObject(CollisionType::SELF)
	{
		PCellPtr pHeadCell = s_MakePCell(startX, startY, s_MakeCell(Cell{ .codepoint = TGLYPHS::SNAKE_HEAD_LEFT }));
		addPCell(pHeadCell);

		for (unsigned int i = 1; i <= m_length - 2; ++i) {
			PCellPtr pBodyCell = s_MakePCell(startX + i, startY, s_MakeCell(Cell{ .codepoint = TGLYPHS::SNAKE_BODY }));
			addPCell(pBodyCell);
		}

		PCellPtr pTailCell = s_MakePCell(startX + m_length - 1, startY, s_MakeCell(Cell{ .codepoint = TGLYPHS::SNAKE_TAIL_RIGHT }));
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

	std::pair<unsigned int, unsigned int> Snake::getHeadPosition() const
	{
		if (!m_cells.empty())
		{
			return { m_cells[0]->x, m_cells[0]->y };
		}

		return {0, 0}; // Fallback (shouldn't happen)
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

		// Step 2: Update head glyph based on direction and move head position
		switch (m_currentDirection)
		{
			case Direction::Up:
				m_cells[0]->y--;
				m_cells[0]->cell->codepoint = TGLYPHS::SNAKE_HEAD_UP;

				break;
			case Direction::Down:
				m_cells[0]->y++;
				m_cells[0]->cell->codepoint = TGLYPHS::SNAKE_HEAD_DOWN;

				break;
			case Direction::Left:
				m_cells[0]->x--;
				m_cells[0]->cell->codepoint = TGLYPHS::SNAKE_HEAD_LEFT;

				break;
			case Direction::Right:
				m_cells[0]->x++;
				m_cells[0]->cell->codepoint = TGLYPHS::SNAKE_HEAD_RIGHT;

				break;
		}

		// Step 3: Update tail glyph based on direction (direction from second-to-last to last segment)
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

	void Snake::grow()
	{
		if (m_cells.size() < 2)
		{
			// Edge case: snake too small to grow properly
			BOOST_LOG_TRIVIAL(warning) << "Snake too small to grow";

			return;
		}

		// Get the current tail position (last segment)
		int tailIndex = m_cells.size() - 1;
		unsigned int tailX = m_cells[tailIndex]->x;
		unsigned int tailY = m_cells[tailIndex]->y;

		// Create new body segment at current tail position
		PCellPtr newBodyCell = s_MakePCell(tailX, tailY, s_MakeCell(Cell{.codepoint = TGLYPHS::SNAKE_BODY}));

		// Insert the new body segment before the tail
		m_cells.insert(m_cells.end() - 1, std::move(newBodyCell));

		// Now move the tail one position back (opposite to the direction it came from)
		int prevIndex = m_cells.size() - 2;	   // Second to last (the new body segment we just added)
		int newTailIndex = m_cells.size() - 1; // Still the tail

		// Calculate direction from new body segment to where tail should go
		int dx = m_cells[newTailIndex]->x - m_cells[prevIndex]->x;
		int dy = m_cells[newTailIndex]->y - m_cells[prevIndex]->y;

		// Move tail one step further in the same direction
		if (dx > 0)
		{
			m_cells[newTailIndex]->x++;
			m_cells[newTailIndex]->cell->codepoint = TGLYPHS::SNAKE_TAIL_RIGHT;
		}
		else if (dx < 0)
		{
			m_cells[newTailIndex]->x--;
			m_cells[newTailIndex]->cell->codepoint = TGLYPHS::SNAKE_TAIL_LEFT;
		}
		else if (dy > 0)
		{
			m_cells[newTailIndex]->y++;
			m_cells[newTailIndex]->cell->codepoint = TGLYPHS::SNAKE_TAIL_DOWN;
		}
		else if (dy < 0)
		{
			m_cells[newTailIndex]->y--;
			m_cells[newTailIndex]->cell->codepoint = TGLYPHS::SNAKE_TAIL_UP;
		}

		m_length++; // Update length counter

		BOOST_LOG_TRIVIAL(info) << "Snake grew! New length: " << m_length;
	}

	CollisionResult Snake::getCollisionResult(BaseObject const& other) const
	{
		// checking for object's collision type is very generic
		// in the future we might want to check for specific object types
		switch (other.getCollisionType())
		{
			case CollisionType::NONE:
				return CollisionResult::NONE; // No collision
			case CollisionType::TRIGGER:
				return CollisionResult::POINTS; // e.g. food eaten
			case CollisionType::SOLID:
			case CollisionType::SELF:
				return CollisionResult::GAME_OVER; // e.g. hit wall or self
			default:
				return CollisionResult::NONE; // No collision
		}
	}

	void Snake::logCells() const
	{
		for (const auto& cell : m_cells) {
			BOOST_LOG_TRIVIAL(info) << "Cell at (" << cell->x << ", " << cell->y << ")";
		}
	}

	Food::Food(unsigned int x, unsigned int y)
		: BaseObject(CollisionType::TRIGGER)
	{
		PCellPtr pFoodCell = s_MakePCell(x, y, s_MakeCell(Cell{ .codepoint = TGLYPHS::FOOD }));
		addPCell(pFoodCell);
	}

	CollisionResult Food::getCollisionResult(BaseObject const& other) const
	{
		if (typeid(other) == typeid(Snake))
		{
			return CollisionResult::POINTS; // e.g. food eaten
		}

		return CollisionResult::NONE; // No collision
	}
};
