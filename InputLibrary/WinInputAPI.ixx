module;
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
export module WinInput;

// Re-exports
export import :Types;

// Imports
import std;
import std.compat;


/*
* @TODO
* - arrow keys
* - shortcuts sequential and simultaneous
*/

namespace wininput {


	///////////////////////////////////////////////////// Setup /////////////////////////////////////////////////////

	/**
	 * @brief Initialize the WindowInput with it's associated window handle
	 * @param hwnd Window handle of the window from which you want to receive input
	 * @param r_window_input WindowInput to initialize
	*/
	export void initWinInput(HWND hwnd, WinInput& r_window_input);

	/**
	* @brief Call inside your window procedure to let the library handle input
	* @param window_input To which WindowInput to handle window messages
	* @return The return value of the window message handled by the library or std::nullopt if no
	* messages which pertain to the library were received.
	*/
	export std::optional<int32_t> handleInputWindowMessages(
		WinInput& window_input, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
	);


	///////////////////////////////////////////////////// Frame /////////////////////////////////////////////////////

	/**
	* @brief Place before the code that dispatches all the window messages to the window procedure
	*/
	export void startReadingInput(WinInput& window_input);

	/**
	 * @brief Place after the code that dispatches all the window messages to the window procedure
	*/
	export void endReadingInput(WinInput& window_input);


	///////////////////////////////////////////////////// Read Keyboard (One key) /////////////////////////////////////////////////////

	/**
	 * @brief Is key held DOWN
	*/
	export bool isKeyDown(WinInput& window_input, VirtualKey key);

	/**
	 * @brief Is key held UP
	*/
	export bool isKeyUp(WinInput& window_input, VirtualKey key);

	/**
	 * @brief Did key transitioned to DOWN
	*/
	export bool keyWentDown(WinInput& window_input, VirtualKey key);

	/**
	 * @brief Did key transitioned to UP
	*/
	export bool keyWentUp(WinInput& window_input, VirtualKey key);

	/**
	 * @brief Did key transition from DOWN to UP or UP to DOWN
	*/
	export bool didKeyTransitionOccurred(WinInput& window_input, VirtualKey key);

	/**
	 * @brief Get the duration in microseconds the key is down
	*/
	export std::chrono::microseconds getKeyDownDurationMicroSecs(WinInput& window_input, VirtualKey key);

	/**
	 * @brief Get the duration in miliseconds the key is down
	*/
	export std::chrono::milliseconds getKeyDownDurationMiliSecs(WinInput& window_input, VirtualKey key);

	/**
	 * @brief Get the underline spans of a key
	*/
	export std::vector<KeySpan>& getKeySpans(WinInput& window_input, VirtualKey key);


	///////////////////////////////////////////////////// Read Keyboard (Multiple keys) /////////////////////////////////////////////////////

	/**
	 * @brief Get a list of keys that were DOWN
	*/
	export void keysDown(WinInput& window_input, std::vector<VirtualKey>& r_keys_down);

	/**
	 * @brief Get a list of keys that were UP
	*/
	export void keysUp(WinInput& window_input, std::vector<VirtualKey>& r_keys_up);

	/**
	 * @brief Get a list of key which transitioned from UP to DOWN
	 */
	export void keysWentDown(WinInput& window_input, std::vector<VirtualKey>& r_keys_down);

	/**
	 * @brief Get a list of key which transitioned from DOWN to UP
	 */
	export void keysWentUp(WinInput& window_input, std::vector<VirtualKey>& r_keys_up);


	///////////////////////////////////////////////////// Read Mouse /////////////////////////////////////////////////////

	/**
	 * @brief Did the mouse move in regard to pixel coordinates ?
	*/
	export bool didMouseMove(WinInput& window_input);

	/**
	 * @brief Did the mouse move in regard to raw input ?
	*/
	export bool didMouseMoveDelta(WinInput& window_input);

	/**
	 * @brief Did the mouse wheel rotate ?
	*/
	export bool didMouseWheelRotate(WinInput& window_input);

	/**
	 * @brief Get the current mouse screen position */
	export MousePosition getMouseScreenPosition();

	/**
	 * @brief Get the mouse position inside the window
	*/
	export MousePosition getMouseWindowPosition(WinInput& window_input);

	/**
	 * @brief Get the mouse positions inside the window
	*/
	export std::vector<MousePosition>& getMouseWindowPositions(WinInput& window_input);

	/**
	 * @brief Get the mouse delta
	*/
	export MouseDelta getMouseDelta(WinInput& window_input);

	/**
	 * @brief Get the mouse deltas
	*/
	export std::vector<MouseDelta>& getMouseDeltas(WinInput& window_input);


	/**
	 * @brief Get mouse wheel delta
	*/
	export int16_t getMouseWheelDelta(WinInput& window_input);


	///////////////////////////////////////////////////// Stats /////////////////////////////////////////////////////

	/**
		* @brief Total number of key down messages received across multiple frames since first key press
	*/
	export uint64_t getKeyDownMessageCount(WinInput& window_input, VirtualKey key);

	/**
	 * @brief Total number of messages received in a frame
	*/
	export uint32_t getKeyFrameMessageCount(WinInput& window_input, VirtualKey key);


	///////////////////////////////////////////////////// Misc /////////////////////////////////////////////////////

	export std::string getKeyName(WinInput& window_input, VirtualKey key);
}
