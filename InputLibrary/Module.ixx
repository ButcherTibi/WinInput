// Glabal Module Fragment
module;
#include "Windows.h"
#include "hidusage.h"
#undef DELETE

export module Module;

/*** Imports ***/
import std;
using SteadyTime = std::chrono::time_point<std::chrono::steady_clock>;
import std.compat;

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

export struct KeyState {
	bool is_down;
	SteadyTime start_time;
	SteadyTime end_time;
	bool transition;

	// Stats
	uint32_t messages_count;
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
export void endReadingInput(WindowInput& window_input,
	std::chrono::time_point<std::chrono::steady_clock> frame_start_time
);


/* Read Keyboard */

export bool isKeyDown(WindowInput& window_input, uint16_t key);
//export void getKeysDown(WindowInput& window_input, std::vector<uint16_t>& r_keys_down);
export bool isKeyInTransition(WindowInput& window_input);
//export void getKeyTransitions(WindowInput& window_input, , std::vector<uint16_t>& r_keys_transitioned);
export std::chrono::nanoseconds getKeyDuration(WindowInput& window_input, uint16_t key);
export std::chrono::milliseconds getKeyDurationMs(WindowInput& window_input, uint16_t key);


/* Read Mouse */

export bool didMouseMove(WindowInput& window_input);
//export MousePosition getMouseScreenPosition(WindowInput& window_input);
export MousePosition getMouseWindowPosition(WindowInput& window_input);
//export std::vector<MousePosition>& getMouseWindowPositions(WindowInput& window_input);
export MouseDelta getMouseDelta(WindowInput& window_input);
//export std::vector<MouseDeltas>& getMouseDeltas(WindowInput& window_input);


/*** Implementation ***/
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

	auto now = std::chrono::steady_clock::now();

	for (auto& key : s.key_list) {
		key.is_down = false;
		key.start_time = now;
		key.end_time = now;
		key.transition = false;
	}
}

void setKeyDownState(WindowInput& s, uint64_t wParam)
{
	KeyState& key = s.key_list[wParam];

	// DOWN transition
	if (key.is_down == false) {
		key.is_down = true;
		key.transition = true;
		key.start_time = std::chrono::steady_clock::now();
	}

	// key was already DOWN do nothing
}

void setKeyUpState(WindowInput& s, uint64_t wParam)
{
	KeyState& key = s.key_list[wParam];

	// UP transition
	if (key.is_down) {
		key.is_down = false;
		key.transition = true;
		key.start_time = std::chrono::steady_clock::now();
	}

	// key was already UP do nothing
}

export void expandWindowProcedure(WindowInput& input, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	// Keyboard
	case WM_KEYDOWN: {
		setKeyDownState(input, wParam);
		break;
	}
	case WM_KEYUP: {
		setKeyUpState(input, wParam);
		break;
	}
	// Mouse Buttons
	case WM_LBUTTONDOWN: {
		setKeyDownState(input, VirtualKeys::LEFT_MOUSE_BUTTON);
	}
	case WM_LBUTTONUP: {
		setKeyUpState(input, VirtualKeys::LEFT_MOUSE_BUTTON);
	}
	case WM_RBUTTONDOWN: {
		setKeyDownState(input, VirtualKeys::RIGHT_MOUSE_BUTTON);
	}
	case WM_RBUTTONUP: {
		setKeyUpState(input, VirtualKeys::RIGHT_MOUSE_BUTTON);
	}
	case WM_MBUTTONDOWN: {
		setKeyDownState(input, VirtualKeys::MIDDLE_MOUSE_BUTTON);
	}
	case WM_MBUTTONUP: {
		setKeyUpState(input, VirtualKeys::MIDDLE_MOUSE_BUTTON);
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
		key.transition = false;
	}
}

export void endReadingInput(WindowInput& s, SteadyTime frame_start_time)
{
	for (auto& key : s.key_list) {
		key.end_time = frame_start_time;
	}
}

export bool isKeyDown(WindowInput& s, uint16_t key)
{
	return s.key_list[key].is_down;
}

export bool isKeyInTransition(WindowInput& s, uint16_t key)
{
	return s.key_list[key].transition;
}

export std::chrono::nanoseconds getKeyDuration(WindowInput& s, uint16_t key)
{
	auto start = s.key_list[key].start_time;
	auto end = s.key_list[key].end_time;
	return start < end ? end - start : std::chrono::milliseconds(0);
}

export std::chrono::milliseconds getKeyDurationMs(WindowInput& s, uint16_t key)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(getKeyDuration(s, key));
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