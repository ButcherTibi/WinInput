module;
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
export module WinInput:MouseInput;

import :Types;
import :Utils;
import std;


export bool didMouseMove(WindowInput& s)
{
	return s.wm_mousemove_received;
}

export bool didMouseMoveDelta(WindowInput& s)
{
	return s.mouse_deltas.size() > 0;
}

export MousePosition getMouseScreenPosition()
{
	POINT point{};

	auto result = GetCursorPos(&point);
	if (result == 0) {
		std::cerr << "Could not get mouse screen position" << std::endl;
		std::wcerr << getLastError() << std::endl;
		return {};
	}

	return MousePosition {
		point.x, point.y
	};
}

export MousePosition getMouseWindowPosition(WindowInput& s)
{
	return s.mouse_pos_last_frame;
}

export std::vector<MousePosition>& getMouseWindowPositions(WindowInput& s)
{
	return  s.mouse_pos_history;
}

export MouseDelta getMouseDelta(WindowInput& s)
{
	return s.mouse_delta;
}

export std::vector<MouseDelta>& getMouseDeltas(WindowInput& s)
{
	return s.mouse_deltas;
}