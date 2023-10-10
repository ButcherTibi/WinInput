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

bool wasKeyInState(WindowInput& s, uint16_t key, bool state)
{
	for (auto& span : s.key_list[key].spans) {
		if (span.is_down == state) {
			return true;
		}
	}
	return false;
}

export bool wasKeyDown(WindowInput& s, uint16_t key)
{
	return wasKeyInState(s, key, true);
}

export bool wasKeyUp(WindowInput& s, VirtualKey key)
{
	return wasKeyInState(s, key, false);
}

bool wasKeyInTransition(WindowInput& s, VirtualKey key, bool is_down)
{
	for (auto& span : s.key_list[key].spans) {
		if (span.is_down == is_down && span.transition) {
			return true;
		}
	}
	return false;
}

export bool keyWentDown(WindowInput& s, VirtualKey key)
{
	return wasKeyInTransition(s, key, true);
}

export bool keyWentUp(WindowInput& s, VirtualKey key)
{
	return wasKeyInTransition(s, key, false);
}

void keysInState(WindowInput& s, bool state, std::vector<VirtualKey>& r_keys)
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

export void keysDown(WindowInput& s, std::vector<VirtualKey>& r_keys_down)
{
	return keysInState(s, true, r_keys_down);
}

export void keysUp(WindowInput& s, std::vector<VirtualKey>& r_keys_up)
{
	return keysInState(s, false, r_keys_up);
}

export bool didKeyTransitionOccurred(WindowInput& s, uint16_t key)
{
	for (auto& span : s.key_list[key].spans) {
		if (span.transition) {
			return true;
		}
	}
	return false;
}

std::chrono::nanoseconds getKeyStateDuration(WindowInput& s, VirtualKey key, bool is_down)
{
	std::chrono::nanoseconds sum(0);

	for (auto& span : s.key_list[key].spans) {
		if (span.is_down) {
			sum += span.duration();
		}
	}
	return sum;
}

export std::chrono::milliseconds getKeyDownDuration(WindowInput& window_input, VirtualKey key)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(getKeyStateDuration(window_input, key, true));
}

export std::vector<KeySpan>& getKeySpans(WindowInput& s, uint16_t key)
{
	return s.key_list[key].spans;
}

export uint64_t getKeyDownMessageCount(WindowInput& window_input, uint16_t key)
{
	return window_input.key_list[key].down_message_count;
}

export uint32_t getKeyFrameMessageCount(WindowInput& window_input, uint16_t key)
{
	return window_input.key_list[key].frame_message_count;
}