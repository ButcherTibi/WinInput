module;
#include "Windows.h"
export module KeyExample;

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

export void keyExample()
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
			L"Key Example",                    // Window text
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

		auto frame_start_time = std::chrono::steady_clock::now();

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
		std::vector<VirtualKey> keys_down;
		keysDown(window_input, keys_down);

		for (VirtualKey key : keys_down) {

			auto duration = getKeyDownDuration(window_input, key);
			auto msg_count = getKeyDownMessageCount(window_input, key);

			if (keyWentDown(window_input, key)) {
				std::printf("Key %d went down \n", key);
			}

			std::printf("Key %d is down, Duration = %lld ms, Message Count = %lld \n", key, duration.count(), msg_count);

			if (keyWentUp(window_input, key)) {
				std::printf("Key %d went up \n", key);
			}
		}

		// Frame Rate Limit
		auto frame_duration = std::chrono::milliseconds(1000 / 60);  // 60 FPS
		// If we finish early yield back time to the OS
		// Comment the below line for running as fast as possible (max usage of a single thread, see Task Manager)
		std::this_thread::sleep_until(frame_start_time + frame_duration);
	}
}