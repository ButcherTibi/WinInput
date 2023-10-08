// Windows
#include "Windows.h"

// std
import std;
using SteadyTime = std::chrono::time_point<std::chrono::steady_clock>;

// Library
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
		std::cerr << "Failed to create window" << std::endl;
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

			endReadingInput(window_input);
		}

		// Respond to input
		{
			std::vector<VirtualKey> keys_down;
			keysDown(window_input, keys_down);

			for (VirtualKey key : keys_down) {

				auto msg_count = getKeyMessageCount(window_input, key);

				if constexpr (true) {
					std::printf("Key %d is down, message count %lld \n", key, msg_count);
				}

				if constexpr (false) {
					if (keyWentDown(window_input, key)) {
						std::printf("Key %d went down \n", key);
					}

					if (keyWentUp(window_input, key)) {
						std::printf("Key %d went up \n", key);
					}
				}
			}

			if (didMouseMove(window_input)) {

				if constexpr (false) {
					auto [x, y] = getMouseWindowPosition(window_input);
					std::printf("Mouse Window Position = %d %d \n", x, y);
				}

				if constexpr (false) {
					auto& positions = getMouseWindowPositions(window_input);
					for (int i = 0; i < positions.size(); i++) {
						auto [x, y] = positions[i];
						std::printf("Mouse Window Positions = %d %d \n", x, y);
					}
				}

				if constexpr (false) {
					auto [x, y] = getMouseScreenPosition();
					std::printf("Mouse Screen Position = %d %d \n", x, y);
				}

				if constexpr (false) {
					auto [x, y] = getMouseDelta(window_input);
					std::printf("Mouse Delta = %d %d \n", x, y);
				}

				if constexpr (false) {
					auto& deltas = getMouseDeltas(window_input);
					for (auto& delta : deltas) {
						std::printf("Mouse Delta = %d %d \n", delta.x, delta.y);
					}
				}
			}
		}

		// Frame Rate Limit
		//std::this_thread::sleep_until(frame_start_time + std::chrono::milliseconds(16));
	}

	return 0;
}