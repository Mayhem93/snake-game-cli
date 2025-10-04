#pragma once

#include <vector>
#include "screen.h"

namespace Snake
{
	class BaseObject {
		public:
			enum class CollisionType
			{
				None,	// No collision (decorative objects)
				Solid,	// Blocks movement, causes game over
				Trigger // Causes events but doesn't block movement
			};

			BaseObject();
			virtual ~BaseObject() = default;

			const std::vector<std::unique_ptr<PositionedCell>>& cells() const;
			CollisionType getCollisionType() const;
			virtual void onCollision(BaseObject* other) {};
			void addPCell(PCellPtr& pCell);
			static CellPtr s_MakeCell(const Cell& cell);
			static PCellPtr s_MakePCell(unsigned int x, unsigned int y, CellPtr cell);

		protected:
			std::vector<std::unique_ptr<PositionedCell>> m_cells;
			CollisionType m_collisionType = CollisionType::None;
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
			std::pair<unsigned int, unsigned int> getHeadPosition() const;
			void move();
			void up();
			void down();
			void left();
			void right();
			void grow();
			void logCells() const;

		private:
			unsigned int m_length = 5;
			Direction m_currentDirection = Direction::Left;
	};
};
