module;
#include "Windows.h"
#include "hidusage.h"  // for RAWINPUT registration
#include "windowsx.h"  // GET_X_LPARAM GET_Y_LPARAM

export module WinInput:Setup;

import :Types;
import :Utils;

import std;
using SteadyTime = std::chrono::time_point<std::chrono::steady_clock>;
import std.compat;

// for NEXTRAWINPUTBLOCK 
typedef unsigned __int64 QWORD;



export void initWindowInput(HWND hwnd, WindowInput& s)
{
	// Get Device List
	/*std::vector<RAWINPUTDEVICELIST> devices;
	{
		uint32_t device_count;

		if (GetRawInputDeviceList(NULL, &device_count, sizeof(RAWINPUTDEVICELIST)) == -1 && device_count == 0) {
			__debugbreak();
		}

		devices.resize(device_count);

		if (GetRawInputDeviceList(devices.data(), &device_count, sizeof(RAWINPUTDEVICELIST)) == -1) {
			__debugbreak();
		}
	}*/

	// Raw Device Input for Mouse
	{
		RAWINPUTDEVICE raw_input_dev;
		raw_input_dev.usUsagePage = HID_USAGE_PAGE_GENERIC;  // Generic Desktop Controls
		raw_input_dev.usUsage = HID_USAGE_GENERIC_MOUSE; // Mouse
		raw_input_dev.dwFlags = RIDEV_DEVNOTIFY;
		raw_input_dev.hwndTarget = hwnd;

		if (!RegisterRawInputDevices(&raw_input_dev, 1, sizeof(RAWINPUTDEVICE))) {
			s.rawinput_ok = false;
			std::cerr << "Failed to register RAWINPUT devices" << std::endl;
			std::wcerr << getLastError() << std::endl;
		}
		else {
			s.rawinput_ok = true;
		}
	}

	// Init Keys
	auto now = std::chrono::steady_clock::now();

	for (auto& key : s.key_list) {
		auto& span = key.spans.emplace_back();
		span.is_down = false;
		span.start_time = now;
		span.end_time = now;
		span.transition = false;

		key.down_message_count = 0;
		key.frame_message_count = 0;
	}

	// Init Mouse
	auto& last_mouse_pos = s.mouse_pos_history.emplace_back();
	last_mouse_pos.x = 0;
	last_mouse_pos.y = 0;
}

void setKeyState(WindowInput& s, uint64_t wParam, bool is_down)
{
	KeyState& key = s.key_list[wParam];

	auto* prev = &key.spans.back();

	// Extension
	if (prev->is_down == is_down) {
		// do nothing
	}
	// Transition
	else {
		auto& new_span = key.spans.emplace_back();
		prev = &key.spans[key.spans.size() - 2];

		new_span.is_down = is_down;
		new_span.transition = true;
		new_span.start_time = std::chrono::steady_clock::now();
	}

	if (is_down == true) {
		key.down_message_count += 1;
	}
	else {
		key.reset_down_msg_count = true;
	}

	key.frame_message_count += 1;
}

export std::optional<int32_t> handleInputWindowMessages(WindowInput& input, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	// Keyboard
	case WM_KEYDOWN: {
		setKeyState(input, wParam, true);
		return 0;
	}
	case WM_KEYUP: {
		setKeyState(input, wParam, false);
		return 0;
	}
	// Mouse Buttons
	case WM_LBUTTONDOWN: {
		setKeyState(input, VirtualKeys::LEFT_MOUSE_BUTTON, true);
		return 0;
	}
	case WM_LBUTTONUP: {
		setKeyState(input, VirtualKeys::LEFT_MOUSE_BUTTON, false);
		return 0;
	}
	case WM_RBUTTONDOWN: {
		setKeyState(input, VirtualKeys::RIGHT_MOUSE_BUTTON, true);
		return 0;
	}
	case WM_RBUTTONUP: {
		setKeyState(input, VirtualKeys::RIGHT_MOUSE_BUTTON, false);
		return 0;
	}
	case WM_MBUTTONDOWN: {
		setKeyState(input, VirtualKeys::MIDDLE_MOUSE_BUTTON, true);
	}
	case WM_MBUTTONUP: {
		setKeyState(input, VirtualKeys::MIDDLE_MOUSE_BUTTON, false);
		return 0;
	}
	case WM_MOUSEWHEEL: {
		input.mouse_wheel_delta += (int16_t)getHighOrder((uint32_t)wParam);
		return 0;
	}
	// Mouse Movement
	case WM_MOUSEMOVE: {
		input.wm_mousemove_received = true;

		auto& new_pos = input.mouse_pos_history.emplace_back();
		new_pos.x = GET_X_LPARAM(lParam);
		new_pos.y = GET_Y_LPARAM(lParam);

		input.mouse_pos_last_frame = new_pos;

		if constexpr (false) {
			std::printf("Mouse Window Position = %d %d \n", new_pos.x, new_pos.y);
		}
		return 0;
	}
	// Raw Input
	case WM_INPUT: {

		if constexpr (false) {
			std::printf("WM_INPUT received \n");
		}

		// tolerate raw input not working
		if (input.rawinput_ok == false) {
			return 0;
		}

		auto addMouseDelta = [&](RAWINPUT* rawinput) {
			auto& new_mouse_delta = input.mouse_deltas.emplace_back();
			new_mouse_delta.x = (int32_t)rawinput->data.mouse.lLastX;
			new_mouse_delta.y = (int32_t)rawinput->data.mouse.lLastY;

			input.mouse_delta.x += new_mouse_delta.x;
			input.mouse_delta.y += new_mouse_delta.y;
		};

		// Standard Read
		if constexpr (true) {

			uint32_t data_size;

			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &data_size, sizeof(RAWINPUTHEADER))) {
				if constexpr (true) {
					std::wcerr << getLastError() << std::endl;
				}
				return 0;
			}

			input.rawinput_data.resize(data_size);
			

			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, input.rawinput_data.data(), &data_size, sizeof(RAWINPUTHEADER)) != data_size) {
				if constexpr (true) {
					std::wcerr << getLastError() << std::endl;
				}
				return 0;
			}

			RAWINPUT* rawinput = (RAWINPUT*)input.rawinput_data.data();

			if (rawinput->header.dwType == RIM_TYPEMOUSE) {
				addMouseDelta(rawinput);
			}
		}
		// Buffered Read (does not work)
		else {
			uint32_t entry_size;
			if (GetRawInputBuffer(NULL, &entry_size, sizeof(RAWINPUTHEADER))) {
				if constexpr (true) {
					std::wcerr << getLastError() << std::endl;
				}
				return 0;
			}
			if (entry_size == 0) {
				return 0;
			}

			uint32_t buffer_size = entry_size * input.max_rawinputs;
			input.rawinput_buffer.resize(buffer_size);

			uint32_t block_count = 1;

			while (block_count > 0) {

				uint32_t block_size = buffer_size;
				block_count = GetRawInputBuffer(
					(RAWINPUT*)input.rawinput_buffer.data(),
					&block_size,
					sizeof(RAWINPUTHEADER)
				);
				if (block_count == (uint32_t)-1) {

					if constexpr (true) {
						std::wcerr << getLastError() << std::endl;
					}
					return 0;
				}

				RAWINPUT* rawinput = (RAWINPUT*)input.rawinput_buffer.data();

				for (uint32_t i = 0; i < block_count; i++) {
					addMouseDelta(rawinput);
				}
			}
		}

		return 0;
	}
	}

	return std::nullopt;
}

export void startReadingInput(WindowInput& s)
{
	s.wm_mousemove_received = false;
	s.mouse_pos_history.clear();

	s.mouse_deltas.clear();
	s.mouse_delta.x = 0;
	s.mouse_delta.y = 0;
	s.mouse_wheel_delta = 0;

	for (uint16_t virtual_key = 0; virtual_key < s.key_list.size(); virtual_key++) {

		KeyState& key = s.key_list[virtual_key];

		// Spans
		key.spans[0] = key.spans[key.spans.size() - 1];
		key.spans.resize(1);

		key.spans[0].transition = false;

		// Stats
		if (key.reset_down_msg_count) {
			key.down_message_count = 0;
			key.reset_down_msg_count = false;
		}
		key.frame_message_count = 0;
	}
}

export void endReadingInput(WindowInput& s)
{
	auto now = std::chrono::steady_clock::now();

	for (auto& key : s.key_list) {

		if (key.spans.size() > 1) {

			for (uint32_t i = 0; i < key.spans.size() - 1; i++) {
				auto& current = key.spans[i];
				auto& next = key.spans[i + 1];

				// fill the in between end time 
				current.end_time = next.start_time;
			}
		}

		auto& last = key.spans.back();
		last.end_time = now;
	}
}