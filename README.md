# WinInput
 
Small library for reading keyboard and mouse input using [Win32](https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-keydown) written in C++20 using [modules](https://learn.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-170).

## Features
- reads keyboard input
  - key down
  - key up
  - key transition
  - duration of key held down
- reads mouse position
- reads mouse delta using [RawInput](https://learn.microsoft.com/en-us/windows/win32/inputdev/about-raw-input)
- all public API is documented (see [`WinInput.ixx`](.InputLibrary/WinInput.ixx))

## Example

```C++
// File: GettingStarted.ixx
module;
#include "Windows.h"
export module GettingStarted;

import std;
import WinInput;

WindowInput window_input;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto result = handleInputWindowMessages(window_input, hwnd, uMsg, wParam, lParam);
	if (result.has_value()) {
		// window message already handled by the library so return it's result
		return result.value();
	}

	// Your stuff . . .
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

export void gettingStarted()
{
	// Create Window
	HWND hwnd = NULL;
	{
		HINSTANCE hInstance = GetModuleHandle(nullptr);
		const wchar_t CLASS_NAME[] = L" ";

		WNDCLASS wc = { };
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = CLASS_NAME;
		RegisterClass(&wc);

		hwnd = CreateWindowEx(
			0,                                 // Optional window styles.
			CLASS_NAME,                        // Window class
			L"Getting Started",                // Window text
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,  // Window style

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

			NULL,       // Parent window    
			NULL,       // Menu
			hInstance,  // Instance handle
			NULL        // Additional application data
		);

		if (hwnd == NULL) {
			std::cerr << "Failed to create window" << std::endl;
			return;
		}
	}
	
	// Initialize Window Input with the HWND to read input
	initWindowInput(hwnd, window_input);

	// Frame Loop
	while (true) {

		// Read Window Messages
		{
			startReadingInput(window_input);

			MSG msg = { };
			while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			endReadingInput(window_input);
		}

		// Respond to input
		{
			std::vector<VirtualKey> keys_down;
			keysDown(window_input, keys_down);

			for (VirtualKey key : keys_down) {
				std::printf("Key %d is down \n", key);
			}
		}
	}
}
```