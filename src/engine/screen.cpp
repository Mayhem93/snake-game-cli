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
		m_height(height)
	{
		m_buffer.resize(width * height);
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
		std::fill(m_buffer.begin(), m_buffer.end(), Cell{});
	}

	inline int ScreenBuffer::index(int x, int y) const noexcept
	{
		return y * m_width + x;
	}

	void ScreenBuffer::set(int x, int y, Cell c) noexcept
	{
		if (x < 0 || y < 0 || x >= m_width || y >= m_height) { return; }

		m_buffer[index(x, y)] = c;
	}

	const Cell& ScreenBuffer::get(int x, int y) const noexcept
	{
		// assert(x >= 0 && y >= 0 && x < width_ && y < height_);

		return m_buffer[index(x, y)];
	}

	void ScreenBuffer::addObject(const BaseObject& obj) {
	    for (const auto& cwp : obj.cells()) {
	        set(cwp.x, cwp.y, cwp.cell);
	    }
	}

	/* static const char* utf8_next(const char *ptr, const char *end, uint32_t &out_cp)
	{
		if (ptr >= end)
		{
			out_cp = 0;

			return ptr;
		}

		unsigned char c0 = static_cast<unsigned char>(*ptr);
		// 1-byte (ASCII)
		if (c0 < 0x80)
		{
			out_cp = c0;

			return ptr + 1;
		}

		// determine sequence length and initial bits
		unsigned cp = 0;
		int needed = 0;

		if ((c0 & 0xE0) == 0xC0)
		{
			cp = c0 & 0x1F;
			needed = 1;
		}
		else if ((c0 & 0xF0) == 0xE0)
		{
			cp = c0 & 0x0F;
			needed = 2;
		}
		else if ((c0 & 0xF8) == 0xF0)
		{
			cp = c0 & 0x07;
			needed = 3;
		}
		else
		{
			// invalid leading byte
			out_cp = 0xFFFDu;

			return ptr + 1;
		}

		// consume continuation bytes
		const char *p = ptr + 1;

		for (int i = 0; i < needed; ++i)
		{
			if (p >= end)
			{
				out_cp = 0xFFFDu;

				return end;
			}
			unsigned char cc = static_cast<unsigned char>(*p++);
			if ((cc & 0xC0) != 0x80)
			{
				out_cp = 0xFFFDu;

				return p;
			}

			cp = (cp << 6) | (cc & 0x3F);
		}

		out_cp = cp;

		return p;
	} */

	/* void ScreenBuffer::drawText(int x, int y, std::string_view utf8, Cell c)
	{
		if (y < 0 || y >= m_height)
		{
			return;
		}

		const char *ptr = utf8.data();
		const char *end = ptr + utf8.size();
		int cx = x;

		while (ptr < end && cx < m_width)
		{
			uint32_t cp = 0;
			ptr = utf8_next(ptr, end, cp);

			if (cp == 0)
				continue;

			set(cx++, y, Cell{cp, c.fg, c.bg, c.attrs});
		}
	} */
}
