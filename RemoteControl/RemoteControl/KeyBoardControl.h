#pragma once

#include <afxwin.h>

class KeyBoardControl {
private:
	int dd[255] = { 0 };
public:
	KeyBoardControl() {
	}

	void OnKeyPressed(WORD key, bool extended) {
		// This structure will be used to create the keyboard
		// input event.
		INPUT ip;

		// Set up a generic keyboard event.
		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = 0; // hardware scan code for key
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;

		ip.ki.wVk = key; // virtual-key code for the key
		ip.ki.dwFlags = 0; // 0 for key press
		SendInput(1, &ip, sizeof(INPUT));
		dd[key] = true;
	}

	void OnKeyReleased(WORD key, bool extended) {
		// This structure will be used to create the keyboard
		// input event.
		INPUT ip;

		// Set up a generic keyboard event.
		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = 0; // hardware scan code for key
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;

		// Release the key
		ip.ki.wVk = key; // virtual-key code for the key
		ip.ki.dwFlags = 2; // KEYEVENTF_KEYUP for key release
		SendInput(1, &ip, sizeof(INPUT));
		dd[key] = false;
	}

	void onAction(WORD keyCode) {
		if ((160 <= keyCode && keyCode <= 163) || keyCode == 18) {
			if (dd[keyCode]) OnKeyReleased(keyCode, true);
			else OnKeyPressed(keyCode, true);
		}
		else {
			OnKeyPressed(keyCode, false);
			OnKeyReleased(keyCode, false);
		}
	}

};