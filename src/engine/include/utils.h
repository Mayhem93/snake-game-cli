#pragma once

#include <filesystem>

namespace Snake
{
	/**
	 * @namespace Snake::Utils
	 * @brief Namespace for utility functions.
	 */
	namespace Utils
	{
		std::filesystem::path getExecutablePath();
	};
};
