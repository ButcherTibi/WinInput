module;
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
export module WinInput;


// Imports
export import :Types;

import std;
import std.compat;


/*
* @TODO
* - controller input
* - shortcuts maybe
*/

/* Setup */

/**
 * @brief Initialize the WindowInput with it's associated window handle
 * @param hwnd Window handle of the window from which you want to receive input
 * @param r_window_input WindowInput to initialize
*/
export void initWindowInput(HWND hwnd, WindowInput& r_window_input);

/**
 * @brief Call inside your window procedure to let the library handle input
 * @param window_input To which WindowInput to handle window messages
 * @return The return value of the window message handled by the library or std::nullopt if no
 * messages which pertain to the library were received.
*/
export std::optional<int32_t> handleInputWindowMessages(
	WindowInput& window_input, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);


/* Frame */

/**
 * @brief Place before the code that dispatches window messages to the window procedure
*/
export void startReadingInput(WindowInput& window_input);

/**
 * @brief Place after the code that dispatches window messages to the window procedure
*/
export void endReadingInput(WindowInput& window_input);


/* Read Keyboard (One key) */

/**
 * @brief Is key held DOWN
*/
export bool wasKeyDown(WindowInput& window_input, VirtualKey key);

/**
 * @brief Is key held UP
*/
export bool wasKeyUp(WindowInput& window_input, VirtualKey key);

/**
 * @brief Did key transitioned to DOWN
*/
export bool keyWentDown(WindowInput& window_input, VirtualKey key);

/**
 * @brief Did key transitioned to UP
*/
export bool keyWentUp(WindowInput& window_input, VirtualKey key);

/**
 * @brief Did key transition from DOWN to UP or UP to DOWN 
*/
export bool didKeyTransitionOccurred(WindowInput& window_input, VirtualKey key);

/**
 * @brief Get the duration of time the key is down
*/
export std::chrono::milliseconds getKeyDownDuration(WindowInput& window_input, VirtualKey key);

/**
 * @brief Get the underline spans of a key 
*/
export std::vector<KeySpan>& getKeySpans(WindowInput& window_input, VirtualKey key);


/* Read Keyboard (Multiple keys) */

/**
 * @brief Get a list of keys that were DOWN
*/
export void keysDown(WindowInput& window_input, std::vector<VirtualKey>& r_keys_down);

/**
 * @brief Get a list of keys that were UP
*/
export void keysUp(WindowInput& window_input, std::vector<VirtualKey>& r_keys_up);


/* Read Mouse */

/**
 * @brief Did the mouse move in regard to pixel coordinates
*/
export bool didMouseMove(WindowInput& window_input);

/**
 * @brief Did the mouse move in regard to raw input
*/
export bool didMouseMoveDelta(WindowInput& window_input);

/**
 * @brief Get the current mouse screen position */
export MousePosition getMouseScreenPosition();

/**
 * @brief Get the mouse position inside the window
*/
export MousePosition getMouseWindowPosition(WindowInput& window_input);

/**
 * @brief  Get the mouse positions inside the window
*/
export std::vector<MousePosition>& getMouseWindowPositions(WindowInput& window_input);

/**
 * @brief Get the mouse delta
*/
export MouseDelta getMouseDelta(WindowInput& window_input);

/**
 * @brief Get the mouse deltas
*/
export std::vector<MouseDelta>& getMouseDeltas(WindowInput& window_input);


/* Stats */

/**
 * @brief Total number of key down messages received
*/
export uint64_t getKeyDownMessageCount(WindowInput& window_input, VirtualKey key);

/**
 * @brief Total number of messages received in a frame 
*/
export uint32_t getKeyFrameMessageCount(WindowInput& window_input, VirtualKey key);
