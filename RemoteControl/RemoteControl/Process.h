#pragma once

#include <afxwin.h>

class Process {

public:
	CString name;
	DWORD id;

public:
	Process(CString name, DWORD id) {
		this->name = name;
		this->id = id;
	}
};