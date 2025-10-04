#pragma once

#include <vector>
#include "screen.h"

namespace Snake
{
	class BaseObject {
		public:
			BaseObject();
			virtual ~BaseObject() = default;
			const std::vector<std::unique_ptr<PositionedCell>>& cells() const;
			bool isCollisionEnabled() const;
			void addPCell(PCellPtr& pCell);
			static CellPtr s_MakeCell(const Cell& cell);
			static PCellPtr s_MakePCell(unsigned int x, unsigned int y, CellPtr cell);

		protected:
			std::vector<std::unique_ptr<PositionedCell>> m_cells;
			bool m_collisionEnabled = true;
	};

	class Border : public BaseObject
	{
		public:
			Border(unsigned int width, unsigned int height);
	};

	class Snake : public BaseObject
	{
		public :
			Snake(unsigned int startX, unsigned int startY);

			enum class Direction
			{
				Up,
				Down,
				Left,
				Right
			};

			void setDirection(Direction direction);
			void move();
			void grow();
			void logCells() const;

		private:
			unsigned int m_length = 5;
			Direction m_currentDirection = Direction::Left;
	};
};
