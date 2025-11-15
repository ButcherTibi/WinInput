module;
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "hidusage.h"
#undef DELETE
export module WinInput:Types;

import std;
using SteadyTime = std::chrono::time_point<std::chrono::steady_clock>;
import std.compat;


// Shared Types

export typedef uint16_t VirtualKey;
export namespace VirtualKeys {
	enum : VirtualKey {
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
		COLLON = VK_OEM_1,
		
		SINGLE_QUOTE = VK_OEM_7,
		DOUBLE_QUOTE = VK_OEM_7,
		
		COMMA = VK_OEM_COMMA,
		LESS = VK_OEM_COMMA,
		
		DOT = VK_OEM_PERIOD,
		MORE = VK_OEM_PERIOD,

		SPACE = VK_SEPARATOR,

		// Symbols
		SQUARE_BRACKET_OPEN = VK_OEM_4,
		CURLY_OPEN = VK_OEM_4,

		SQUARE_BRACKET_CLOSE = VK_OEM_6,
		CURLY_CLOSE = VK_OEM_6,

		GRAVE = VK_OEM_3,
		TILDA = VK_OEM_3,

		MINUS = VK_OEM_MINUS,
		UNDER_LINE = VK_OEM_MINUS,

		EQUALS = VK_OEM_PLUS,
		PLUS = VK_OEM_PLUS,

		FORDWARD_SLASH = VK_OEM_5,
		QUESTION_MARK = VK_OEM_5,

		BACK_SLASH = VK_OEM_2,
		VERTICAL_BAR = VK_OEM_2,

		// Left Side
		ESCAPE = VK_ESCAPE,
		TAB = VK_TAB,
		CAPS_LOCK = VK_CAPITAL,
		SHIFT = VK_SHIFT,
		//LEFT_SHIFT = 302,

		// Right Side
		BACKSPACE = VK_BACK,
		ENTER = VK_RETURN,
		//RIGHT_SHIFT = 303,

		// Below
		LEFT_CONTROL = 300,
		RIGHT_CONTROL = 301,
		
		LEFT_WIN = VK_LWIN,
		RIGHT_WIN = VK_RWIN,

		LEFT_ALT = VK_LMENU,
		RIGHT_ALT = VK_RMENU,

		MENU = VK_APPS,

		// Function
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

		// Above Arrows
		PRINT_SCREEN = VK_PRINT,
		SCROLL_LOCK = VK_SCROLL,
		PAUSE = VK_PAUSE,

		INSERT = VK_INSERT,
		DELETE = VK_DELETE,
		HOME = VK_HOME,
		END = VK_END,

		PAGE_UP = VK_PRIOR,
		PRIOR = VK_PRIOR,

		PAGE_DOWN = VK_NEXT,
		NEXT = VK_NEXT,

		// Mouse
		LEFT_MOUSE_BUTTON = VK_LBUTTON,
		RIGHT_MOUSE_BUTTON = VK_RBUTTON,
		MIDDLE_MOUSE_BUTTON = VK_MBUTTON,
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
	bool reset_down_msg_count;

	/// number of messages received during frame
	uint32_t frame_message_count;
};

export struct MousePosition {
	int32_t x;
	int32_t y;
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
export struct WinInput {
	// Setup
	bool rawinput_ok;

	// this list also contains non-existent, reserved, unused virtual key codes
	// and fake ones
	std::array<KeyState, 512> key_list;

	// Local Mouse Position
	bool wm_mousemove_received;
	MousePosition mouse_pos_last_frame;
	std::vector<MousePosition> mouse_pos_history;

	// Mouse Delta
	MouseDelta mouse_delta;
	std::vector<MouseDelta> mouse_deltas;

	std::vector<uint8_t> rawinput_data;
	const uint32_t max_rawinputs = 128;
	std::vector<uint8_t> rawinput_buffer;

	// Mouse Wheel Delta
	bool wm_mousewheel_received;
	int16_t mouse_wheel_delta;
};