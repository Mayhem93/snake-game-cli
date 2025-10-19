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
			 * @callergraph
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
			 * @callergraph
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
			 * @callergraph
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

	/**
	 * @class Border
	 * @brief Represents the border of the game area.
	 *
	 * @details
	 * The Border object is a solid object that causes game over on collision.
	 * It can be animated to change colors over time.
	 */
	class Border : public BaseObject
	{
		public:
			/**
			 * @brief Constructs a Border object with specified width and height representing the game area.
			 * @callgraph
			 * @param width Width of the game area
			 * @param height Height of the game area
			 */
			Border(unsigned int width, unsigned int height);

			/**
			 * @brief Determines the result of a collision with another object
			 * @param other Reference to the other Snake::BaseObject involved in the collision
			 * @return Snake::CollisionResult Result of the collision
			 *
			 * Snake::CollisionResult::GAME_OVER is returned if the other object's collision type is SOLID or SELF.
			 * Otherwise, Snake::CollisionResult::NONE is returned.
			 */
			CollisionResult getCollisionResult(BaseObject const &other) const override;

		protected:
			/**
			 * @brief Animates the border by cycling through a color sequence
			 */
			void animate() override;

		private:
			/**
			 * @brief Sequence of colors for border animation
			 *
			 * The sequence is generated in the constructor.
			 */
			std::vector<uint8_t> m_colorSequence;

			/**
			 * @brief Generates the color sequence for border animation
			 * @callergraph
			 */
			void generateColorSequence();
	};

	/**
	 * @class Snake
	 * @brief Represents the snake controlled by the player.
	 *
	 * @details
	 * The Snake object is a movable and self-colliding object that can grow in length.
	 * It responds to direction changes and moves accordingly.
	 */
	class Snake : public BaseObject
	{
		public :
			/**
			 * @brief Constructs a Snake object starting at the specified coordinates
			 * @callgraph
			 * @param startX Starting X coordinate of the snake's head
			 * @param startY Starting Y coordinate of the snake's head
			 */
			Snake(unsigned int startX, unsigned int startY);

			/**
			 * @enum Direction
			 * @brief Possible movement directions for the snake
			 *
			 * @details
			 * - Up: Move upwards
			 * - Down: Move downwards
			 * - Left: Move leftwards. Initial direction.
			 * - Right: Move rightwards
			 */
			enum class Direction
			{
				Up,
				Down,
				Left,
				Right
			};

			/**
			 * @brief Sets the snake's movement direction
			 * @param direction New direction for the snake
			 */
			void setDirection(Direction direction);
			Position getHeadPosition() const;

			/**
			 * @brief Determines the result of a collision with another object
			 * @param other Reference to the other Snake::BaseObject involved in the collision
			 * @return Snake::CollisionResult Result of the collision
			 *
			 * - If the other object's collision type is SOLID or SELF, returns Snake::CollisionResult::GAME_OVER.
			 * - If the other object's collision type is TRIGGER, returns Snake::CollisionResult::POINTS.
			 * - Otherwise, returns Snake::CollisionResult::NONE.
			 */
			CollisionResult getCollisionResult(BaseObject const &other) const override;

			/**
			 * @brief Sets the snake's direction to up
			 */
			void up();

			/**
			 * @brief Sets the snake's direction to down
			 */
			void down();

			/**
			 * @brief Sets the snake's direction to left
			 */
			void left();

			/**
			 * @brief Sets the snake's direction to right
			 */
			void right();

			/**
			 * @brief Grows the snake by one segment
			 *
			 * Adds a new segment at the tail in the direction opposite to the current movement.
			 */
			void grow();
			void logCells() const;

		protected:
			/**
			 * @brief Moves the snake according to its current direction
			 * @callergraph
			 *
			 * Updates cell's positions starting from the head to the tail.
			 * Also updates the glyphs for the head and tail based on movement direction.
			 */
			void move() override;

		private:
			/** @brief Length of the snake*/
			unsigned int m_length = 5;

			/** @brief Current movement direction */
			Direction m_currentDirection = Direction::Left;
	};

	/**
	 * @class Food
	 * @brief Represents food items that the snake can eat.
	 *
	 * @details
	 * The Food object is a trigger object that provides points when collided with.
	 */
	class Food : public BaseObject
	{
		public:
			/** @brief Constructs a Food object at the specified coordinates
			 * 	@param x X coordinate of the food
			 *	@param y Y coordinate of the food
			 */
			Food(unsigned int x, unsigned int y);

			/**
			 * @brief Determines the result of a collision with another object
			 * @param other Reference to the other Snake::BaseObject involved in the collision
			 * @return Snake::CollisionResult Result of the collision
			 *
			 * - If typeid(other) is Snake::Snake, returns Snake::CollisionResult::POINTS.
			 * - Otherwise, returns Snake::CollisionResult::NONE.
			 */
			CollisionResult getCollisionResult(BaseObject const &other) const override;
	};
};
