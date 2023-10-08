module;
#include "Windows.h"
export module WinInput:Utils;

import std;
import std.compat;


uint16_t getLowOrder(uint32_t param)
{
	return param & 0xFFFF;
}
uint16_t getHighOrder(uint32_t param)
{
	return param >> 16;
}

int16_t getSignedLowOrder(uint32_t param)
{
	return param & 0xFFFF;
}
int16_t getSignedHighOrder(uint32_t param)
{
	return param >> 16;
}

std::wstring getLastError()
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