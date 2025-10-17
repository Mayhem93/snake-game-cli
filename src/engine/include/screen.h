#pragma once

#include<cstdint>
#include<vector>
#include<memory>

#include "glyphs.h"

namespace Snake
{
	class BaseObject;

	/**
	 * @brief 2D position represented as (x, y) coordinates
	 *
	 * A pair of unsigned integers
	 */
	using Position = std::pair<unsigned int, unsigned int>;

	/**
	 * @brief A vector of 2D position represented as (x, y) coordinates
	 *
	 * A vector of Snake::Position
	 */
	using PosVector = std::vector<Position>;

	/**
	 * @brief Structure representing a single cell in the screen buffer. No position info.
	 *
	 * This structure holds the visual representation of a cell, including its glyph, foreground
	 * and background colors, and any attributes it may have.
	 */
	struct Cell
	{
		uint32_t codepoint = TGLYPHS::SPACE;
		uint8_t fg = 0xFF;
		uint8_t bg = 0xFF;
		uint8_t attrs = 0;
		bool default_bg = true; // true: use terminal default background, false: use specified bg color
		bool default_fg = true; // true: use terminal default foreground, false: use specified fg color

		/**
		 * @brief Indicates if this cell is used to initiate a collision (e.g. snake head)
		 *
		 * Ignored if the game object does not use collision detection.
		 */
		bool detector = false;
		constexpr bool operator==(Cell const& o) const noexcept = default;
		constexpr bool operator!=(Cell const& o) const noexcept;
	};

	/**
	 * @brief Shared pointer to a Cell
	 *
	 * Used to allow multiple references to the same Cell instance.
	 *
	 * Used by both Snake::ScreenBuffer and Snake::BaseObject.
	 *
	 * Cells are immutable in Snake::ScreenBuffer but mutable in Snake::BaseObject.
	 *
	 * This is used so that objects themselves can modify their own cells without the intervention of other classes.
	 */
	using CellPtr = std::shared_ptr<Cell>;

	/**
	 * @brief Structure representing a single cell in the screen buffer with position info.
	 *
	 * This struct is used by Snake::BaseObject to track the position of each cell it owns.
	 */
	struct PositionedCell
	{
		unsigned int x;
		unsigned int y;
		CellPtr cell;
	};

	/**
	 * @brief Unique pointer to a PositionedCell
	 *
	 * Positioned cells are owned by Snake::BaseObject instances and thus must be unique.
	 */
	using PCellPtr = std::unique_ptr<PositionedCell>;

	/**
	 * @class ScreenBuffer
	 * @brief Represents the terminal screen buffer for rendering game objects.
	 *
	 * Holds the contents of all game objects and empty cells as well
	 */
	class ScreenBuffer
	{
		public:
			/**
			 * @brief Construct a new Screen Buffer object
			 * @param width Width of the screen buffer in cells
			 * @param height Height of the screen buffer in cells
			 *
			 * Height and width should be obtained from Snake::Terminal during initialization.
			 */
			ScreenBuffer(unsigned int width, unsigned int height);

			/**
			 * @brief Get the width of the screen buffer
			 * @return unsigned int Width in cells
			 */
			unsigned int width() const noexcept;

			/**
			 * @brief Get the height of the screen buffer
			 * @return unsigned int Height in cells
			 */
			unsigned int height() const noexcept;

			/**
			 * @brief Sets the cell at (x, y) to the specified CellPtr
			 * @param x X coordinate
			 * @param y Y coordinate
			 * @param c CellPtr to set at the position
			 *
			 * This method only accepts shared pointers to Cell instances.
			 */
			void set(unsigned int x, unsigned int y, const CellPtr& c) noexcept;

			/**
			 * @brief Gets the CellPtr at (x, y)
			 * @param x X coordinate
			 * @param y Y coordinate
			 * @return CellPtr at the specified position
			 */
			CellPtr get(unsigned int x, unsigned int y) const noexcept;

			/**
			 * @brief Adds a game object to the screen buffer
			 * @param obj Pointer to the BaseObject to add
			 *
			 * `m_buffer` is updated to include the object's cells which are represented by Snake::PCellPtr.
			 */
			void addObject(BaseObject* obj);

			/**
			 * @brief Removes a game object from the screen buffer
			 * @param obj Pointer to the BaseObject to remove
			 *
			 * `m_buffer` is updated to replace the object's cells with `Snake::ScreenBuffer::m_emptyCell`.
			 */
			void removeObject(BaseObject* obj);

			/**
			 * @brief Updates the screen buffer to reflect the current positions of movable and animated objects.
			 *
			 * Only objects marked as movable or animated are updated in the buffer.
			 *
			 * The `Snake::Game` class is responsible for calling this method after updating object states.
			 */
			void updateObjects();

			/**
			 * @brief Get the list of game objects currently in the screen buffer
			 * @return const std::vector<BaseObject*>& Vector of pointers to BaseObject instances
			 *
			 * Used by Snake::Game for collision detection and other operations.
			 */
			const std::vector<BaseObject*>& getObjects() const noexcept;

			/**
			 * @brief Checks if the position (x, y) is empty (i.e., contains the empty cell `m_emptyCell`)
			 * @param x X coordinate
			 * @param y Y coordinate
			 * @return true if the position is empty, false otherwise
			 */
			bool isPositionEmpty(unsigned int x, unsigned int y) const;

			/**
			 * @brief Gets a list of positions that need to be cleared (i.e., vacated by movable objects)
			 * @return PosVector Vector of positions to clear
			 *
			 * Called by Snake::Terminal::render during rendering to determine which cells need to be redrawn as empty.
			 */
			PosVector getPositionsToClear() const;
			CellPtr getEmptyCellPtr() const noexcept;
			void clearPositions(const PosVector &positions);
			void dumpBuffer() const;

		private:
			/**
			 * @brief Shared pointer to the empty cell used to clear positions
			 *
			 * All empty cells in the buffer point to this instance.
			 */
			CellPtr m_emptyCell;
			unsigned int m_width = 0;
			unsigned int m_height = 0;
			std::vector<CellPtr> m_buffer;
			std::vector<BaseObject*> m_objects;
			static std::string s_ToUnicode(uint32_t codepoint) noexcept;

			void clear();

			inline int index(int x, int y) const noexcept;
	};
};
