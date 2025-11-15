module;
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
export module MouseExample;

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
		L"Getting Started Example",               // Window text
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

export void mouseExample()
{
	HWND window_handle = createExampleWindow();

	// Initialize Window Input with the HWND to read input
	initWinInput(window_handle, win_input);

	// Frame Loop
	while (true) {

		auto frame_start_time = std::chrono::steady_clock::now();

		// Read Window Messages
		{
			startReadingInput(win_input);

			MSG msg = { };
			while (PeekMessage(&msg, window_handle, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			endReadingInput(win_input);
		}

		// Respond to input
		if (didMouseMove(win_input)) {

			auto [x_wp, y_wp] = getMouseWindowPosition(win_input);
			std::printf("Mouse Window Position = %d %d \n", x_wp, y_wp);			
		}

		if (didMouseMoveDelta(win_input)) {

			auto [x_sp, y_sp] = getMouseScreenPosition();
			std::printf("Mouse Screen Position = %d %d \n", x_sp, y_sp);

			auto [x_d, y_d] = getMouseDelta(win_input);
			std::printf("Mouse Delta = %d %d \n", x_d, y_d);
		}

		if (didMouseWheelRotate(win_input)) {
			std::printf("Mouse Wheel Delta = %d \n", getMouseWheelDelta(win_input));
		}

		// Frame Rate Limit
		auto frame_duration = std::chrono::milliseconds(1000 / 300);  // 300 FPS
		// If we finish early yield back time to the OS
		// Comment the below line for running as fast as possible (max usage of a single thread, see Task Manager)
		std::this_thread::sleep_until(frame_start_time + frame_duration);
	}
}