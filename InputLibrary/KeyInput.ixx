export module WinInput:KeyInput;

// Partions
import :Types;

// std
import std;
import std.compat;


export std::chrono::nanoseconds KeySpan::duration()
{
	return start_time < end_time ? end_time - start_time : std::chrono::milliseconds(0);
}

export std::chrono::milliseconds KeySpan::durationMiliSec()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(duration());
}

export std::chrono::microseconds KeySpan::durationMicroSec()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(duration());
}

export namespace wininput {
	
	bool wasKeyInState(WinInput& s, uint16_t key, bool state)
	{
		for (auto& span : s.key_list[key].spans) {
			if (span.is_down == state) {
				return true;
			}
		}
		return false;
	}

	export bool isKeyDown(WinInput& s, uint16_t key)
	{
		return wasKeyInState(s, key, true);
	}

	export bool isKeyUp(WinInput& s, VirtualKey key)
	{
		return wasKeyInState(s, key, false);
	}

	bool wasKeyInTransition(WinInput& s, VirtualKey key, bool is_down)
	{
		for (auto& span : s.key_list[key].spans) {
			if (span.is_down == is_down && span.transition) {
				return true;
			}
		}
		return false;
	}

	export bool keyWentDown(WinInput& s, VirtualKey key)
	{
		return wasKeyInTransition(s, key, true);
	}

	export bool keyWentUp(WinInput& s, VirtualKey key)
	{
		return wasKeyInTransition(s, key, false);
	}

	export bool didKeyTransitionOccurred(WinInput& s, uint16_t key)
	{
		for (auto& span : s.key_list[key].spans) {
			if (span.transition) {
				return true;
			}
		}
		return false;
	}

	std::chrono::nanoseconds getKeyStateDuration(WinInput& s, VirtualKey key, bool is_down)
	{
		std::chrono::nanoseconds sum(0);

		for (auto& span : s.key_list[key].spans) {
			if (span.is_down) {
				sum += span.duration();
			}
		}
		return sum;
	}

	export std::chrono::microseconds getKeyDownDurationMicroSecs(WinInput& window_input, VirtualKey key)
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(getKeyStateDuration(window_input, key, true));
	}

	export std::chrono::milliseconds getKeyDownDurationMiliSecs(WinInput& window_input, VirtualKey key)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(getKeyStateDuration(window_input, key, true));
	}

	export std::vector<KeySpan>& getKeySpans(WinInput& s, uint16_t key)
	{
		return s.key_list[key].spans;
	}

	void keysInState(WinInput& s, bool state, std::vector<VirtualKey>& r_keys)
	{
		for (VirtualKey key = 0; key < s.key_list.size(); key++) {

			KeyState& key_state = s.key_list[key];

			for (auto& span : key_state.spans) {
				if (span.is_down == state) {
					r_keys.push_back(key);
					break;
				}
			}
		}
	}

	export void keysDown(WinInput& s, std::vector<VirtualKey>& r_keys_down)
	{
		return keysInState(s, true, r_keys_down);
	}

	export void keysUp(WinInput& s, std::vector<VirtualKey>& r_keys_up)
	{
		return keysInState(s, false, r_keys_up);
	}

	export void keysWentDown(WinInput& s, std::vector<VirtualKey>& r_keys_down)
	{
		for (VirtualKey key = 0; key < s.key_list.size(); key++) {

			KeyState& key_state = s.key_list[key];

			for (auto& span : key_state.spans) {
				if (span.is_down and span.transition) {
					r_keys_down.push_back(key);
					break;
				}
			}
		}
	}

	export void keysWentUp(WinInput& s, std::vector<VirtualKey>& r_keys_up)
	{
		for (VirtualKey key = 0; key < s.key_list.size(); key++) {

			KeyState& key_state = s.key_list[key];

			for (auto& span : key_state.spans) {
				if (span.is_down == false and span.transition) {
					r_keys_up.push_back(key);
					break;
				}
			}
		}
	}

	export uint64_t getKeyDownMessageCount(WinInput& window_input, uint16_t key)
	{
		return window_input.key_list[key].down_message_count;
	}

	export uint32_t getKeyFrameMessageCount(WinInput& window_input, uint16_t key)
	{
		return window_input.key_list[key].frame_message_count;
	}
}
