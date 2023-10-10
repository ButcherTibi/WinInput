module;
#include "Windows.h"
export module MouseExample;

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

export void mouseExample()
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
			L"Mouse Example",                  // Window text
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
		if (didMouseMove(window_input)) {

			auto [x_wp, y_wp] = getMouseWindowPosition(window_input);
			std::printf("Mouse Window Position = %d %d \n", x_wp, y_wp);			
		}

		if (didMouseMoveDelta(window_input)) {

			auto [x_sp, y_sp] = getMouseScreenPosition();
			std::printf("Mouse Screen Position = %d %d \n", x_sp, y_sp);

			auto [x_d, y_d] = getMouseDelta(window_input);
			std::printf("Mouse Delta = %d %d \n", x_d, y_d);
		}
	}
}