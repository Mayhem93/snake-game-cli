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

	/**
	 * @brief Bitwise AND operator for Attributes enum
	 */
	constexpr uint16_t operator& (Attributes a, Attributes b) noexcept;

	/**
	 * @brief Bitwise OR operator for Attributes enum
	 */
	constexpr uint16_t operator| (Attributes a, Attributes b) noexcept;

	/**
	 * @brief Bitwise OR operator between uint16_t and Attributes enum
	 */
	constexpr bool operator& (uint16_t a, Attributes b) noexcept;

	/**
	 * @brief Bitwise AND operator between Attributes enum and uint16_t
	 */
	constexpr bool operator& (Attributes a, uint16_t b) noexcept;

	/**
	 * @brief Bitwise OR operator between uint16_t and Attributes enum
	 */
	constexpr bool operator| (uint16_t a, Attributes b) noexcept;

	/**
	 * @brief Bitwise OR operator between Attributes enum and uint16_t
	 */
	constexpr bool operator| (Attributes a, uint16_t b) noexcept;

	/**
	 * @class BaseObject
	 * @brief Abstract base class for all game objects (e.g., Snake, Border, Food).
	 *
	 * @details
	 * Provides common functionality for game objects, including position management,
	 * movement, animation, and collision handling.
	 */
	class BaseObject {
		public:
			/**
			 * @brief Constructs a BaseObject with specified collision type and attributes
			 * @param colType CollisionType of the object. Default is Snake::CollisionType::NONE.
			 * @param attrs Attributes (flags) of the object. Default is 0 (no attributes).
			 */
			BaseObject(CollisionType colType = CollisionType::NONE, uint16_t attrs = 0);

			/**
			 * @brief Constructs a BaseObject with specified collision type and attributes
			 * @param colType CollisionType of the object. Default is Snake::CollisionType::NONE.
			 * @param attrs Attributes (flags) of the object. Default is Snake::Attributes::NONE.
			 */
			BaseObject(CollisionType colType = CollisionType::NONE, Attributes attrs = static_cast<Attributes>(0));

			/**
			 * @brief Default virtual destructor
			 */
			virtual ~BaseObject() = default;

			/**
			 * @brief Gets the positioned cells that make up this object
			 * @return Vector of Snake::PCellPtr
			 */
			const std::vector<PCellPtr>& cells() const noexcept;

			/**
			 * @brief Checks if the object is movable
			 * @return true if the object has the MOVABLE attribute, false otherwise
			 *
			 * Checks `m_attributes` for the MOVABLE flag.
			 */
			bool isMovable() const noexcept;

			/**
			 * @brief Checks if the object is animated
			 * @return true if the object has the ANIMATED attribute, false otherwise
			 *
			 * Checks `m_attributes` for the ANIMATED flag.
			 */
			bool isAnimated() const noexcept;

			/**
			 * @brief Updates the object's position based on its movement logic
			 * @callgraph
			 *
			 * Calls the implemented `BaseObject::move` method to update positions.
			 *
			 * Captures previous and new positions for tracking.
			 *
			 * Does nothing if the object is not movable.
			 */
			void performMove();

			/**
			 * @brief Updates the object's animation frame
			 * @callgraph
			 *
			 * Calls the implemented `BaseObject::animate` method to update animation frame.
			 *
			 * Does nothing if the object is not animated.
			 */
			void performAnimate();

			/**
			 * @brief Gets positions vacated by the object after movement
			 * @return PosVector Positions that were occupied before the last move but are now empty
			 *
			 * Does a difference between `m_previousPositions` and `m_newPositions`.
			 */
			PosVector getVacatedPositions() const;

			/**
			 * @brief Gets the object's collision type
			 * @return CollisionType of the object
			 */
			CollisionType getCollisionType() const noexcept;

			/**
			 * @brief Determines the result of a collision with another object
			 * @param other Reference to the other Snake::BaseObject involved in the collision
			 * @return Snake::CollisionResult Result of the collision
			 *
			 * Pure virtual method to be implemented by derived classes.
			 */
			virtual CollisionResult getCollisionResult(BaseObject const& other) const = 0;

			/**
			 * @brief Gets positions of detector cells for self-collision detection
			 * @return PosVector Vector of positions occupied by detector cells
			 *
			 * Iterates through the object's cells and collects positions of those marked as detectors (Snake::Cell::detector = true).
			 */
			PosVector getDetectorCellsPos() const;
		protected:
			/**
			 * @brief Positioned cells constituting this object
			 */
			std::vector<PCellPtr> m_cells;

			/**
			 * @brief Animation frame counter
			 */
			size_t m_animationFrame = 0;

			/**
			 * @brief Adds a positioned cell to the object
			 * @param pCell Reference to the Snake::PCellPtr to add
			 */
			void addPCell(PCellPtr& pCell);

			/**
			 * @brief Used by derived classes to move it's cells according to its logic
			 *
			 * Does nothing if the object is not movable (eg: subclasses do not override this function).
			 */
			virtual void move();

			/**
			 * @brief Used by derived classes to animate it's cells according to its logic
			 *
			 * Does nothing if the object cannot be animated (eg: subclasses do not override this function).
			 */
			virtual void animate();

			/**
			 * @brief Helper function that creates a shared pointer to a Cell
			 * @param cell Cell to create a shared pointer for
			 * @return Snake::CellPtr Shared pointer to the created Cell
			 *
			 * This is used so that classes have access to the same Cell instance for mutation purposes.
			 */
			static CellPtr s_MakeCell(const Cell& cell);

			/**
			 * @brief Helper function that creates a unique pointer to a PositionedCell
			 * @param x X coordinate of the PositionedCell
			 * @param y Y coordinate of the PositionedCell
			 * @param cell Snake::CellPtr Shared pointer to the Cell for this PositionedCell
			 * @return Snake::PCellPtr Unique pointer to the created PositionedCell
			 *
			 * These pointers are owned by the game objects (e.g., Snake, Border, Food). This means only them can update the positions directly.
			 */
			static PCellPtr s_MakePCell(unsigned int x, unsigned int y, CellPtr cell);

		private:
			/**
			 * @brief Attributes (flags) of the object
			 *
			 * Represents a combination of Snake::Attributes values.
			 */
			uint16_t m_attributes;

			/**
			 * @brief Collision type of the object
			 *
			 * Default: Snake::CollisionType::NONE
			 */
			CollisionType m_collisionType;

			/**
			 * @brief Positions occupied by the object's cells before the last move
			 *
			 * Generated by GameObject::performMove
			 */
			PosVector m_previousPositions;

			/**
			 * @brief Positions occupied by the object's cells after the last move
			 *
			 * Generated by GameObject::performMove
			 */
			PosVector m_newPositions;

			/**
			 * @brief Captures current positions of the object's cells
			 * @return PosVector with current positions
			 */
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
