#pragma once

#ifdef _WIN32

#include <stdexcept>
#include <string>
#include <Windows.h>

#define THROW_IF_FAILED(hr)                   \
	if (FAILED(hr))                           \
	{                                         \
		std::string message("COM error: ");   \
		message.append(std::to_string(hr))    \
			.append(" (at ")                  \
			.append(__FILE__)                 \
			.append(":")                      \
			.append(std::to_string(__LINE__)) \
			.append(")")                      \
			.append("\n");                    \
		throw std::runtime_error(message);    \
	}

#endif
