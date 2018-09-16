#pragma once

#include <vector>
#include <Windows.h>

using namespace std;

class WindowManager {

public:
	WindowManager() {}

public:
	vector<wstring> ListWindowName() {
		vector<wstring> windowName;
		EnumWindows(ListWindow, reinterpret_cast<LPARAM>(&windowName));
		return windowName;
	}

	static BOOL CALLBACK ListWindow(HWND hwnd, LPARAM lParam) {
		const DWORD TITLE_SIZE = 1024;
		WCHAR windowTitle[TITLE_SIZE];

		GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

		int length = ::GetWindowTextLength(hwnd);
		wstring title(&windowTitle[0]);
		if (!IsWindowVisible(hwnd) || length == 0 || title == L"Program Manager") {
			return TRUE;
		}

		// Retrieve the pointer passed into this callback, and re-'type' it.
		// The only way for a C API to pass arbitrary data is by means of a void*.
		std::vector<std::wstring>& titles =
			*reinterpret_cast<std::vector<std::wstring>*>(lParam);
		titles.push_back(title);

		return TRUE;
	}
};