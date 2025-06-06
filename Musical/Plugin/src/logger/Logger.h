#pragma once

#include <string>

#include "../../external/bakkes/logging/logging.h"

namespace Log
{
	template <typename... Args>
	void Info(std::string_view format_str, Args&&... args)
	{
		LOG(std::string("[Musical/Info]: ") + std::string(format_str), std::forward<Args>(args)...);
	}

	template <typename... Args>
	void Error(std::string_view format_str, Args&&... args)
	{
		LOG(std::string("[Musical/Error]: ") + std::string(format_str), std::forward<Args>(args)...);
	}

	template <typename... Args>
	void Warning(std::string_view format_str, Args&&... args)
	{
		LOG(std::string("[Musical/Warning]: ") + std::string(format_str), std::forward<Args>(args)...);
	}
}