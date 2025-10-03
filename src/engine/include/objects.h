#pragma once

#include <vector>
#include "screen.h"

namespace Snake
{
	class BaseObject {
		public:
			BaseObject();
			virtual ~BaseObject() = default;
			const std::vector<PositionedCell>& cells() const;
			bool isCollisionEnabled() const;

		protected:
			std::vector<PositionedCell> m_cells;
			bool m_collisionEnabled = true;
	};

	class Border : public BaseObject
	{
		public:
			Border(unsigned int width, unsigned int height);

			const std::vector<PositionedCell>& cells() const;
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

		private:
			unsigned int m_length = 5;
			Direction m_currentDirection = Direction::Left;
	};
};
