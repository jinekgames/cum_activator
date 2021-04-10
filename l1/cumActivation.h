#pragma once

#include <string>


namespace cumActivation {

	static bool isPlaying = false;

	static std::string buf("aaaaa");

	inline void push(char c) noexcept {
		int i;
		for (i = 0; i < 4; i++) {
			buf[i] = buf[i + 1];
		}
		buf[i] = c;
	}

	inline bool check(std::string text) noexcept {
		// debug
		//OutputDebugStringA(buf.c_str());
		//OutputDebugStringA("\n");

		int i = text.length() - 1;
		int k = buf.length() - 1;

		if (i > k) {
			return false;
		}

		for ( ; i > 0 ; i--, k--) {

			if (text[i] != buf[k]) {
				return false;
			}

		}

		return true;

		/*if ( buf == text ) {
			return true;
		}
		return false;*/
	}

}