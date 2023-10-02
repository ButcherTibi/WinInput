// Notes to self:
// - migrate to using just span to answear simple queries
// - 

// Glabal Module Fragment
module;
#include "Windows.h"
#include "hidusage.h"
#undef DELETE

export module WinInput;

/*** Imports ***/

import std;
using SteadyTime = std::chrono::time_point<std::chrono::steady_clock>;
import std.compat;

import Utils;


/*** Interface ***/
export namespace VirtualKeys {
	enum : uint16_t {
		// Numbers
		NUMBER_0 = 0x30,
		NUMBER_1,
		NUMBER_2,
		NUMBER_3,
		NUMBER_4,
		NUMBER_5,
		NUMBER_6,
		NUMBER_7,
		NUMBER_8,
		NUMBER_9 = 0x39,

		// Letters
		A = 0x41,
		B,
		C,
		D,
		E,

		F,
		G,
		H,
		I,
		J,

		K,
		L,
		M,
		N,
		O,

		P,
		Q,
		R,
		S,
		T,

		U,
		V,
		W,
		X,
		Y,

		Z = 0x5A,

		// Punctuation
		SEMICOLON = VK_OEM_1,
		SINGLE_QUOTE = VK_OEM_7,
		COMMA = VK_OEM_COMMA,
		DOT = VK_OEM_PERIOD,

		// Text Editing
		BACKSPACE = VK_BACK,
		DELETE = VK_DELETE,
		SPACE = VK_SEPARATOR,
		ENTER = VK_RETURN,

		// Symbols
		SQUARE_BRACKET_OPEN = VK_OEM_4,
		SQUARE_BRACKET_CLOSE = VK_OEM_6,
		TILDA = VK_OEM_3,
		MINUS = VK_OEM_MINUS,
		PLUS = VK_OEM_PLUS,
		FORDWARD_SLASH = VK_OEM_5,
		BACK_SLASH = VK_OEM_2,

		// Mouse
		LEFT_MOUSE_BUTTON = VK_LBUTTON,
		RIGHT_MOUSE_BUTTON = VK_RBUTTON,
		MIDDLE_MOUSE_BUTTON = VK_MBUTTON,

		// Function
		TAB = VK_TAB,
		CAPS_LOCK = VK_CAPITAL,
		SHIFT = VK_SHIFT,
		CONTROL = VK_CONTROL,

		F1 = VK_F1,
		F2 = VK_F2,
		F3 = VK_F3,
		F4 = VK_F4,

		F5 = VK_F5,
		F6 = VK_F6,
		F7 = VK_F7,
		F8 = VK_F8,

		F9 = VK_F9,
		F10 = VK_F10,
		F11 = VK_F11,
		F12 = VK_F12,
	};
}

export struct KeySpan {
	bool is_down;
	bool transition;
	SteadyTime start_time;
	SteadyTime end_time;

public:
	std::chrono::nanoseconds duration();
	std::chrono::milliseconds durationMiliSec();
	std::chrono::microseconds durationMicroSec();
};

export struct KeyState {
	std::vector<KeySpan> spans;

	// Stats
	/// number of key down messages received across multiple frames since first key press
	uint64_t down_message_count;
	/// number of messages received during frame
	uint32_t frame_message_count;
};

export struct MousePosition {
	int16_t x;
	int16_t y;
};

export struct MouseDelta {
	int32_t x;
	int32_t y;
};


// Note to self:
// If hold down a key the keyboard will not send a key down event every frame, so to measure key down time
// start the timer on key down and stop only on key up event.
// DO NOT extend time on each key down event or else a weird ~500 time delay will occur as the keyboard repeat function buffer
// fills up and generates a single key down message at a later time
export struct WindowInput {
	// this list also contains non-existent, reserved, unused virtual key codes
	std::array<KeyState, 0xFF> key_list;

	// Local Mouse Position
	MousePosition mouse_pos;
	std::vector<MousePosition> mouse_pos_history;

	// Mouse Delta
	// will contain the sum total of inputs within a frame
	MouseDelta mouse_delta;
	std::vector<MouseDelta> mouse_deltas;

	const uint32_t max_rawinput_buffers = 128;
	std::vector<uint8_t> rawinput_buffer;

	// Mouse Wheel Delta
	int16_t mouse_wheel_delta;
};


/* Setup */

export void initWindowInput(HWND hwnd, WindowInput& r_window_input);

export void expandWindowProcedure(WindowInput& window_input, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


/* Frame */

export void startReadingInput(WindowInput& window_input);
export void endReadingInput(WindowInput& window_input);


/* Read Keyboard */

export bool wasKeyDown(WindowInput& window_input, uint16_t key);
export bool didKeyTransitionOccurred(WindowInput& window_input, uint16_t key);
export std::vector<KeySpan>& getKeySpans(WindowInput& window_input, uint16_t key);


/* Read Mouse */

export bool didMouseMove(WindowInput& window_input);
//export MousePosition getMouseScreenPosition(WindowInput& window_input);
export MousePosition getMouseWindowPosition(WindowInput& window_input);
//export std::vector<MousePosition>& getMouseWindowPositions(WindowInput& window_input);
export MouseDelta getMouseDelta(WindowInput& window_input);
//export std::vector<MouseDeltas>& getMouseDeltas(WindowInput& window_input);


/* Stats */

export uint64_t getKeyMessageCount(WindowInput& window_input, uint16_t key);
export uint32_t getKeyFrameMessageCount(WindowInput& window_input, uint16_t key);


/*** Implementation ***/

export std::chrono::nanoseconds KeySpan::duration()
{
	return start_time < end_time ? end_time - start_time : std::chrono::milliseconds(0);
}

export std::chrono::milliseconds KeySpan::durationMiliSec()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(duration());
}

export std::chrono::microseconds KeySpan::durationMicroSec()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(duration());
}

export void initWindowInput(HWND hwnd, WindowInput& s)
{
	// Get Device List
	std::vector<RAWINPUTDEVICELIST> devices;
	{
		uint32_t device_count;

		if (GetRawInputDeviceList(NULL, &device_count, sizeof(RAWINPUTDEVICELIST)) == -1 && device_count == 0) {
			__debugbreak();
		}

		devices.resize(device_count);

		if (GetRawInputDeviceList(devices.data(), &device_count, sizeof(RAWINPUTDEVICELIST)) == -1) {
			__debugbreak();
		}
	}

	// Get Device Infos
	{
		
	}

	// Raw Device Input for Mouse
	{
		RAWINPUTDEVICE raw_input_dev;
		raw_input_dev.usUsagePage = HID_USAGE_PAGE_GENERIC;  // Generic Desktop Controls
		raw_input_dev.usUsage = HID_USAGE_GENERIC_MOUSE; // Mouse
		raw_input_dev.dwFlags = RIDEV_DEVNOTIFY;
		raw_input_dev.hwndTarget = hwnd;

		if (!RegisterRawInputDevices(&raw_input_dev, 1, sizeof(RAWINPUTDEVICE))) {
			__debugbreak();
		};
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

	if (is_down) {
		key.down_message_count += 1;
	}
	else {
		key.down_message_count = 0;
	}

	key.frame_message_count += 1;
}

export void expandWindowProcedure(WindowInput& input, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	// Keyboard
	case WM_KEYDOWN: {
		setKeyState(input, wParam, true);
		break;
	}
	case WM_KEYUP: {
		setKeyState(input, wParam, false);
		break;
	}
	// Mouse Buttons
	case WM_LBUTTONDOWN: {
		setKeyState(input, VirtualKeys::LEFT_MOUSE_BUTTON, true);
	}
	case WM_LBUTTONUP: {
		setKeyState(input, VirtualKeys::LEFT_MOUSE_BUTTON, false);
	}
	case WM_RBUTTONDOWN: {
		setKeyState(input, VirtualKeys::RIGHT_MOUSE_BUTTON, true);
	}
	case WM_RBUTTONUP: {
		setKeyState(input, VirtualKeys::RIGHT_MOUSE_BUTTON, false);
	}
	case WM_MBUTTONDOWN: {
		setKeyState(input, VirtualKeys::MIDDLE_MOUSE_BUTTON, true);
	}
	case WM_MBUTTONUP: {
		setKeyState(input, VirtualKeys::MIDDLE_MOUSE_BUTTON, false);
	}
	case WM_MOUSEWHEEL: {
		input.mouse_wheel_delta += (int16_t)getHighOrder((uint32_t)wParam);
	}

	// Mouse Movement
	case WM_MOUSEMOVE: {
		auto& new_pos = input.mouse_pos_history.emplace_back();
		new_pos.x = getLowOrder((uint32_t)lParam);;
		new_pos.y = getHighOrder((uint32_t)lParam);

		input.mouse_pos.x = new_pos.x;
		input.mouse_pos.y = new_pos.y;
	}
	// Raw Input
	case WM_INPUT: {
		
		uint32_t stride = sizeof(RAWINPUT);
		GetRawInputBuffer(NULL, &stride, sizeof(RAWINPUTHEADER));

		uint32_t buffer_size = stride * input.max_rawinput_buffers;
		input.rawinput_buffer.resize(buffer_size);

		uint32_t count = GetRawInputBuffer((RAWINPUT*)input.rawinput_buffer.data(), &buffer_size, sizeof(RAWINPUTHEADER));
		if (count == (uint32_t)-1) {
			break;
		}

		for (uint32_t i = 0; i < count; i++) {
			RAWINPUT* raw = (RAWINPUT*)(input.rawinput_buffer.data() + i * stride);

			auto& new_mouse_delta = input.mouse_deltas.emplace_back();
			new_mouse_delta.x = (int32_t)raw->data.mouse.lLastX;
			new_mouse_delta.y = (int32_t)raw->data.mouse.lLastY;

			input.mouse_delta.x += new_mouse_delta.x;
			input.mouse_delta.y += new_mouse_delta.y;

			// printf("%d %d \n", (int32_t)raw->data.mouse.lLastX, (int32_t)raw->data.mouse.lLastY);
		}
	}
	}
}

export void startReadingInput(WindowInput& s)
{
	s.mouse_pos.x = 0;
	s.mouse_pos.y = 0;
	s.mouse_pos_history.clear();

	s.mouse_delta.x = 0;
	s.mouse_delta.y = 0;
	s.mouse_deltas.clear();
	s.mouse_wheel_delta = 0;

	for (uint16_t virtual_key = 0; virtual_key < s.key_list.size(); virtual_key++) {

		KeyState& key = s.key_list[virtual_key];

		// Spans
		key.spans[0] = key.spans[key.spans.size() - 1];
		key.spans.resize(1);

		key.spans[0].transition = false;

		// Stats
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

				current.end_time = next.start_time;
			}
		}
		
		auto& last = key.spans.back();
		last.end_time = now;
	}
}

export bool wasKeyDown(WindowInput& s, uint16_t key)
{
	for (auto& span : s.key_list[key].spans) {
		if (span.is_down) {
			return true;
		}
	}
	return false;
}

export bool didKeyTransitionOccurred(WindowInput& s, uint16_t key)
{
	for (auto& span : s.key_list[key].spans) {
		if (span.transition) {
			return true;
		}
	}
	return false;
}

export std::vector<KeySpan>& getKeySpans(WindowInput& s, uint16_t key)
{
	return s.key_list[key].spans;
}

export bool didMouseMove(WindowInput& s)
{
	return s.mouse_delta.x != 0 || s.mouse_delta.y != 0;
}

//export MousePosition getScreenMousePosition(WindowInput& window_input)
//{
//	return window_input.mouse_pos;
//}

export MousePosition getMouseWindowPosition(WindowInput& window_input)
{
	return window_input.mouse_pos;
}

export MouseDelta getMouseDelta(WindowInput& window_input)
{
	return window_input.mouse_delta;
}

export uint64_t getKeyMessageCount(WindowInput& window_input, uint16_t key)
{
	return window_input.key_list[key].down_message_count;
}

export uint32_t getKeyFrameMessageCount(WindowInput& window_input, uint16_t key)
{
	return window_input.key_list[key].frame_message_count;
}
