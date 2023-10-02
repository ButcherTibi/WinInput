// Windows
#include "Windows.h"

// std
import std;
using SteadyTime = std::chrono::time_point<std::chrono::steady_clock>;

// Library
import Module;


WindowInput window_input;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	expandWindowProcedure(window_input, hwnd, uMsg, wParam, lParam);
	return DefWindowProc(hwnd, uMsg, wParam, lParam);;
}
  

int main()
{
	// Create Window
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,                                 // Optional window styles.
		CLASS_NAME,                        // Window class
		L"Input Library Test",             // Window text
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,  // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL) {
		return 0;
	}

	initWindowInput(hwnd, window_input);

	// Frame Loop
	while (true) {

		SteadyTime frame_start_time = std::chrono::steady_clock::now();

		// Read Window Messages
		{
			startReadingInput(window_input);

			MSG msg = { };
			while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			endReadingInput(window_input, frame_start_time);
		}

		// Respond to input
		{
			// do stuff . . .

			if (isKeyDown(window_input, VirtualKeys::A)) {

				if (isKeyInTransition(window_input, VirtualKeys::A)) {
					std::printf("DOWN transition \n");
				}

				auto duration = getKeyDurationMs(window_input, VirtualKeys::A);
				std::printf("DOWN %lld \n", duration.count());
			}
			else {
				if (isKeyInTransition(window_input, VirtualKeys::A)) {
					std::printf("UP transition \n");
				}
			}

			if (didMouseMove(window_input)) {
				
				if (false) {
					auto [x, y] = getMouseWindowPosition(window_input);
					std::printf("Mouse Window Position = %d %d \n", x, y);
				}

				if (true) {
					auto [x, y] = getMouseDelta(window_input);
					std::printf("Mouse Delta = %d %d \n", x, y);
				}
			}
		}

		// Frame Rate Limit
		std::this_thread::sleep_until(frame_start_time + std::chrono::milliseconds(16));
	}

	return 0;
}