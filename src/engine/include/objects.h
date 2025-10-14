#pragma once

#include <vector>
#include "screen.h"

namespace Snake
{
	using PosVector = std::vector<std::pair<unsigned int, unsigned int>>;

	enum class CollisionType
	{
		NONE,	// No collision (decorative objects)
		SOLID,	// Blocks movement, causes game over
		TRIGGER,// Causes events but doesn't block movement
		SELF	// Implies SOLID, allows self-collision detection
	};

	enum class CollisionResult
	{
		NONE,			// No collision
		POINTS,			// e.g. food eaten
		GAME_OVER		// e.g. hit wall or self
	};

	class BaseObject {
		public:
			BaseObject(CollisionType colType = CollisionType::NONE, bool isMovable = false);
			virtual ~BaseObject() = default;

			const std::vector<PCellPtr>& cells() const;

			bool isMovable() const;
			void performMove();
			PosVector getVacatedPositions() const;
			CollisionType getCollisionType() const;
			virtual CollisionResult getCollisionResult(BaseObject const& other) const = 0;
			PosVector getDetectorCellsPos() const;
		protected:
			std::vector<std::unique_ptr<PositionedCell>> m_cells;

			void addPCell(PCellPtr& pCell);
			virtual void move();
			static CellPtr s_MakeCell(const Cell& cell);
			static PCellPtr s_MakePCell(unsigned int x, unsigned int y, CellPtr cell);

		private:
			bool m_isMovable;
			CollisionType m_collisionType;
			PosVector m_previousPositions;
			PosVector m_newPositions;

			PosVector capturePositions();
	};

	class Border : public BaseObject
	{
		public:
			Border(unsigned int width, unsigned int height);
			CollisionResult getCollisionResult(BaseObject const &other) const override;
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
			CollisionResult getCollisionResult(BaseObject const &other) const override;
			void up();
			void down();
			void left();
			void right();
			void grow();
			void logCells() const;

		protected:
			void move() override;

		private:
			unsigned int m_length = 5;
			Direction m_currentDirection = Direction::Left;
	};

	class Food : public BaseObject
	{
		public:
			Food(unsigned int x, unsigned int y);
			CollisionResult getCollisionResult(BaseObject const &other) const override;
	};
};
