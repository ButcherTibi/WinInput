module;
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
export module WinInput:MouseInput;

import :Types;
import :Utils;
import std;


export namespace wininput {
	export bool didMouseMove(WinInput& s)
	{
		return s.wm_mousemove_received;
	}

	export bool didMouseMoveDelta(WinInput& s)
	{
		return s.mouse_deltas.size() > 0;
	}

	export bool didMouseWheelRotate(WinInput& s)
	{
		return s.wm_mousewheel_received;
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

		return MousePosition{
			point.x, point.y
		};
	}

	export MousePosition getMouseWindowPosition(WinInput& s)
	{
		return s.mouse_pos_last_frame;
	}

	export std::vector<MousePosition>& getMouseWindowPositions(WinInput& s)
	{
		return  s.mouse_pos_history;
	}

	export MouseDelta getMouseDelta(WinInput& s)
	{
		return s.mouse_delta;
	}

	export std::vector<MouseDelta>& getMouseDeltas(WinInput& s)
	{
		return s.mouse_deltas;
	}

	export int16_t getMouseWheelDelta(WinInput& s)
	{
		return s.mouse_wheel_delta;
	}
}
