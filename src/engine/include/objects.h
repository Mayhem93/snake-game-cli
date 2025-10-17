#pragma once

#include <vector>
#include "screen.h"

namespace Snake
{
	/**
	 * @enum CollisionType
	 * @brief Types of collision behavior for game objects.
	 *
	 * @details
	 * - NONE: No collision (decorative objects)
	 * - SOLID: Blocks movement, causes game over
	 * - TRIGGER: Causes events but doesn't block movement
	 * - SELF: Implies SOLID, allows self-collision detection
	 */
	enum class CollisionType
	{
		NONE,	// No collision (decorative objects)
		SOLID,	// Blocks movement, causes game over
		TRIGGER,// Causes events but doesn't block movement
		SELF	// Implies SOLID, allows self-collision detection
	};

	/**
	 * @enum CollisionResult
	 * @brief Results of collision checks between game objects.
	 *
	 * @details
	 * - NONE: No collision
	 * - POINTS: e.g. food eaten
	 * - GAME_OVER: e.g. hit wall or self
	 */
	enum class CollisionResult
	{
		NONE,			// No collision
		POINTS,			// e.g. food eaten
		GAME_OVER		// e.g. hit wall or self
	};

	/**
	 * @enum Attributes
	 * @brief Attributes (used as flags) that can be assigned to game objects.
	 *
	 * @details
	 * - NONE: (Default) No special attributes
	 * - MOVABLE: Object can move
	 * - ANIMATED: Object has animation frames (Snake::Cell can change appearance over time)
	 */
	enum class Attributes : uint16_t
	{
		NONE = 0,
		MOVABLE = 1 << 0,
		ANIMATED = 1 << 1
	};

	constexpr uint16_t operator& (Attributes a, Attributes b) noexcept;
	constexpr uint16_t operator| (Attributes a, Attributes b) noexcept;
	constexpr bool operator& (uint16_t a, Attributes b) noexcept;
	constexpr bool operator& (Attributes a, uint16_t b) noexcept;
	constexpr bool operator| (uint16_t a, Attributes b) noexcept;
	constexpr bool operator| (Attributes a, uint16_t b) noexcept;

	class BaseObject {
		public:
			BaseObject(CollisionType colType = CollisionType::NONE, uint16_t attrs = 0);
			BaseObject(CollisionType colType = CollisionType::NONE, Attributes attrs = static_cast<Attributes>(0));
			virtual ~BaseObject() = default;

			const std::vector<PCellPtr>& cells() const;

			bool isMovable() const noexcept;
			bool isAnimated() const noexcept;
			void performMove();
			void performAnimate();
			PosVector getVacatedPositions() const;
			CollisionType getCollisionType() const noexcept;
			virtual CollisionResult getCollisionResult(BaseObject const& other) const = 0;
			PosVector getDetectorCellsPos() const;
		protected:
			std::vector<std::unique_ptr<PositionedCell>> m_cells;
			size_t m_animationFrame = 0;

			void addPCell(PCellPtr& pCell);
			virtual void move();
			virtual void animate();
			static CellPtr s_MakeCell(const Cell& cell);
			static PCellPtr s_MakePCell(unsigned int x, unsigned int y, CellPtr cell);

		private:
			uint16_t m_attributes;
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

		protected:
			void animate() override;

		private:
			std::vector<uint8_t> m_colorSequence;

			void generateColorSequence();
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
			Position getHeadPosition() const;
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
