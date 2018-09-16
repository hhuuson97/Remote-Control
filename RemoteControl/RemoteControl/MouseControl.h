#pragma once

#include <afxwin.h>

class MouseControl {

public:
	bool onLeftClick, onRightClick;
	int screenWidth, screenHeight;
	bool state;
	INPUT buffer[1];

public:
	void MouseSetup() {
		buffer->type = INPUT_MOUSE;
		buffer->mi.dx = (0 * (0xFFFF / screenWidth));
		buffer->mi.dy = (0 * (0xFFFF / screenHeight));
		buffer->mi.mouseData = 0;
		buffer->mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
		buffer->mi.time = 0;
		buffer->mi.dwExtraInfo = 0;
	}

	MouseControl() {
		onLeftClick = onRightClick = false;
		screenWidth = 1024;
		screenHeight = 800;
		state = false;
		MouseSetup();
	}

	void setScreen(LONG width, LONG height) {
		screenWidth = width;
		screenHeight = height;
	}

	void onAction(int x, int y) {
		MouseMoveAbsolute(x, y);
		if (!state) {
			if (onLeftClick) MouseLeftUp();
			else MouseLeftDown();
		}
		else {
			if (onRightClick) MouseRightUp();
			else MouseRightDown();
		}
	}

	void changeState() {
		state = !state;
		if (onLeftClick) MouseLeftUp();
		if (onRightClick) MouseRightUp();
	}

	void MouseMoveAbsolute(int x, int y) {
		buffer->mi.dx = (x * (0xFFFF / screenWidth));
		buffer->mi.dy = (y * (0xFFFF / screenHeight));
		buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE);

		SendInput(1, buffer, sizeof(INPUT));
	}

	void MouseLeftDown() {
		buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN);
		SendInput(1, buffer, sizeof(INPUT));
		onLeftClick = true;
	}

	void MouseLeftUp() {
		buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP);
		SendInput(1, buffer, sizeof(INPUT));
		onLeftClick = false;
	}

	void MouseRightDown() {
		buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN);
		SendInput(1, buffer, sizeof(INPUT));
		onRightClick = true;
	}

	void MouseRightUp() {
		buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP);
		SendInput(1, buffer, sizeof(INPUT));
		onRightClick = false;
	}

};