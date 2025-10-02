#pragma once

#include <vector>
#include "screen.h"

namespace Snake
{
	struct PositionedCell {
		int x;
		int y;
		Cell cell;
	};

	class BaseObject {
		public:
			BaseObject();
			virtual ~BaseObject() = default;
			virtual const std::vector<PositionedCell>& cells() const = 0;

		protected:
			std::vector<PositionedCell> m_cells;
	};

	class Border : public BaseObject
	{
		public:
			Border(int width, int height);

			const std::vector<PositionedCell>& cells() const;
	};
};
