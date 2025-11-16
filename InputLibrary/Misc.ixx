module;
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#undef DELETE
export module WinInput:Misc;

import :Types;

import std;
import std.compat;

export namespace wininput {
	export std::string getKeyName(WinInput& window_input, VirtualKey key)
	{
		// Numbers
		if (VirtualKeys::NUMBER_0 <= key && key <= VirtualKeys::NUMBER_9) {
			return std::format("{}", char('0' + (key - VirtualKeys::NUMBER_0)));
		}
		// Letters
		else if (VirtualKeys::A <= key && key <= VirtualKeys::Z) {
			return std::format("{}", char('A' + key - (VirtualKeys::A)));
		}
		else switch (key) {
			// Punctuation
		case VirtualKeys::SEMICOLON: return ";:";
		case VirtualKeys::SINGLE_QUOTE: return "'\"";
		case VirtualKeys::COMMA: return ",<";
		case VirtualKeys::DOT: return ".>";
		case VirtualKeys::SPACE: return "Space";
			// Symbols
		case VirtualKeys::SQUARE_BRACKET_OPEN: return "[{";
		case VirtualKeys::SQUARE_BRACKET_CLOSE: return "]}";
		case VirtualKeys::TILDA: return "`~";
		case VirtualKeys::MINUS: return "-_";
		case VirtualKeys::PLUS: return "=+";
		case VirtualKeys::FORDWARD_SLASH: return "\\|";
		case VirtualKeys::BACK_SLASH: return "/?";
			// Left Side
		case VirtualKeys::ESCAPE:      return "Escape";
		case VirtualKeys::TAB:         return "Tab";
		case VirtualKeys::CAPS_LOCK:   return "Caps Lock";
		case VirtualKeys::SHIFT:   return "Shift";
			// Right Side
		case VirtualKeys::BACKSPACE: return "Backspacee";
		case VirtualKeys::ENTER: return "Enter";
			// Below
		case VirtualKeys::LEFT_CONTROL: return "Left Control";
		case VirtualKeys::RIGHT_CONTROL: return "Right Control";

		case VirtualKeys::LEFT_WIN: return "Left Windows";
		case VirtualKeys::RIGHT_WIN: return "Right Windows";

		case VirtualKeys::LEFT_ALT: return "Left Alt";
		case VirtualKeys::RIGHT_ALT: return "Right Alt";

		case VirtualKeys::MENU: return "Menu";

			// Function
		case VirtualKeys::F1:          return "F1";
		case VirtualKeys::F2:          return "F2";
		case VirtualKeys::F3:          return "F3";
		case VirtualKeys::F4:          return "F4";

		case VirtualKeys::F5:          return "F5";
		case VirtualKeys::F6:          return "F6";
		case VirtualKeys::F7:          return "F7";
		case VirtualKeys::F8:          return "F8";

		case VirtualKeys::F9:          return "F9";
		case VirtualKeys::F10:         return "F10";
		case VirtualKeys::F11:         return "F11";
		case VirtualKeys::F12:         return "F12";

		// Mid section
		case VirtualKeys::PRINT_SCREEN: return "Print Screen";
		case VirtualKeys::SCROLL_LOCK: return "Scroll Lock";
		case VirtualKeys::PAUSE: return "Pause";

		case VirtualKeys::INSERT: return "Insert";
		case VirtualKeys::DELETE: return "Delete";
		case VirtualKeys::HOME: return "Home";
		case VirtualKeys::END: return "End";
		case VirtualKeys::PRIOR: return "Page Up";
		case VirtualKeys::NEXT: return "Page Down";

		case VirtualKeys::LEFT: return "Left";
		case VirtualKeys::UP: return "Up";
		case VirtualKeys::RIGHT: return "Right";
		case VirtualKeys::DOWN: return "Down";

		// Mouse
		case VirtualKeys::LEFT_MOUSE_BUTTON: return "Left Mouse Button";
		case VirtualKeys::RIGHT_MOUSE_BUTTON: return "Right Mouse Button";
		case VirtualKeys::MIDDLE_MOUSE_BUTTON: return "Middle Mouse Button";
		}

		return std::format("Key Code {}", (uint16_t)key);
	}
}
