#include <functional>

#include <boost/log/trivial.hpp>

#include "include/screen.h"
#include "include/objects.h"

namespace Snake
{
	constexpr bool Cell::operator!=(Cell const& o) const noexcept
	{
		return !(*this == o);
	}

	ScreenBuffer::ScreenBuffer(int width, int height) :
		m_width(width),
		m_height(height),
		m_emptyCell(CellPtr(new Cell())) // Shared empty cell
	{
		m_buffer.reserve(width * height);
		clear();
	}

	int ScreenBuffer::width() const noexcept
	{
		return m_width;
	}

	int ScreenBuffer::height() const noexcept
	{
		return m_height;
	}

	void ScreenBuffer::clear()
	{
		m_buffer.clear();

		for (int i = 0 ; i < m_width * m_height; ++i) {
			m_buffer.push_back(m_emptyCell);
		}
	}

	inline int ScreenBuffer::index(int x, int y) const noexcept
	{
		return y * m_width + x;
	}

	void ScreenBuffer::set(int x, int y, const CellPtr& c) noexcept
	{
		if (x < 0 || y < 0 || x >= m_width || y >= m_height) { return; }

		m_buffer[index(x, y)] = c;
	}

	CellPtr ScreenBuffer::get(int x, int y) const noexcept
	{
		// assert(x >= 0 && y >= 0 && x < width_ && y < height_);

		return m_buffer[index(x, y)];
	}

	void ScreenBuffer::addObject(BaseObject* obj) {
		m_objects.push_back(obj);

	    for (const auto& cwp : obj->cells()) {
	        set(cwp->x, cwp->y, cwp->cell);
	    }
	}

	void ScreenBuffer::removeObject(BaseObject* obj) {
		auto it = std::find(m_objects.begin(), m_objects.end(), obj);

		if (it != m_objects.end()) {
			m_objects.erase(it);
		}

		// add empty cells where the object was
	    for (const auto& cwp : obj->cells()) {
	        set(cwp->x, cwp->y, m_emptyCell);
	    }
	}

	void ScreenBuffer::updateObjects()
	{
		for (int i = 0; i < m_buffer.size(); ++i)
		{
			m_buffer[i] = m_emptyCell; // Point to shared empty cell
		}

		// Update buffer to point to current object cell positions
		for (auto *obj : m_objects)
		{
			for (const auto &posCell : obj->cells())
			{
				if (posCell->x >= 0 && posCell->x < m_width &&
					posCell->y >= 0 && posCell->y < m_height)
				{
					m_buffer[index(posCell->x, posCell->y)] = posCell->cell;
				}
			}
		}
	}

	bool ScreenBuffer::isPositionEmpty(unsigned int x, unsigned int y) const
	{
		if (x < 0 || y < 0 || x >= m_width || y >= m_height)
		{
			return false;
		}

		return get(x, y) == m_emptyCell;
	}

	std::string ScreenBuffer::s_ToUnicode(uint32_t codepoint) noexcept
	{
		std::string out;

		if (codepoint < 0x80)
		{
			out += static_cast<char>(codepoint);
		}
		else if (codepoint < 0x800)
		{
			out += static_cast<char>(0xC0 | (codepoint >> 6));
			out += static_cast<char>(0x80 | (codepoint & 0x3F));
		}
		else if (codepoint < 0x10000)
		{
			out += static_cast<char>(0xE0 | (codepoint >> 12));
			out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
			out += static_cast<char>(0x80 | (codepoint & 0x3F));
		}
		else
		{
			out += static_cast<char>(0xF0 | (codepoint >> 18));
			out += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
			out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
			out += static_cast<char>(0x80 | (codepoint & 0x3F));
		}

		return out;
	}

	void ScreenBuffer::dumpBuffer() const
	{
		std::string dump;
		for (int y = 0; y < m_height; ++y) {
			for (int x = 0; x < m_width; ++x) {
				dump += s_ToUnicode(get(x, y)->codepoint);
			}
			dump += '\n';
		}

		BOOST_LOG_TRIVIAL(info) << dump;
	}
}
