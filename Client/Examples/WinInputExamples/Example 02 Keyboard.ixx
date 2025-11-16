module;
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
export module WinInputExamples_Keyboard;

import std;
import WinInput;
using namespace wininput;

WinInput win_input;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto result = handleInputWindowMessages(win_input, hwnd, uMsg, wParam, lParam);
	if (result.has_value()) {
		// window message already handled by the library so return it's result
		return result.value();
	}

	// Your stuff . . .
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HWND createExampleWindow()
{
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	const wchar_t CLASS_NAME[] = L" ";

	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);

	HWND window_handle = CreateWindowEx(
		0,                                 // Optional window styles.
		CLASS_NAME,                        // Window class
		L"Keyboard Example",               // Window text
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,  // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (window_handle == NULL) {
		throw "Failed to create window";
	}

	return window_handle;
}

export void keyboardExample()
{
	HWND hwnd = createExampleWindow();

	// Initialize Window Input with the HWND to read input
	initWinInput(hwnd, win_input);

	// Frame Loop
	while (true) {

		// Read Window Messages
		{
			startReadingInput(win_input);

			MSG msg = { };
			while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			endReadingInput(win_input);
		}

		// Respond to input
		std::vector<VirtualKey> keys_down;
		keysDown(win_input, keys_down);

		for (VirtualKey key : keys_down) {

			auto key_name = getKeyName(win_input, key);
			auto duration = getKeyDownDurationMiliSecs(win_input, key);

			if (keyWentDown(win_input, key)) {
				std::printf("Key %s went down \n", key_name.c_str());
			}
			else {
				std::printf("Key %s is down, Duration = %lld miliseconds \n", key_name.c_str(), duration.count());
			}

			if (keyWentUp(win_input, key)) {
				std::printf("Key %s went up \n", key_name.c_str());
			}
		}
	}
}