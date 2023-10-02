module;
#include "Windows.h"
export module Utils;

import std;
import std.compat;

export uint16_t getLowOrder(uint32_t param)
{
	return param & 0xFFFF;
}

export uint16_t getHighOrder(uint32_t param)
{
	return param >> 16;
}

export int16_t getSignedLowOrder(uint32_t param)
{
	return param & 0xFFFF;
}

export int16_t getSignedHighOrder(uint32_t param)
{
	return param >> 16;
}

export std::wstring getLastError()
{
	LPWSTR buffer;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		0,
		(LPWSTR)&buffer,
		0,
		NULL
	);

	std::wstring error_msg = buffer;

	LocalFree(buffer);

	return error_msg;
}