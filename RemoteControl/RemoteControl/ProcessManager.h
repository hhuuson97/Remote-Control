#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include "Process.h"

class ProcessManager {
	
public:
	ProcessManager() {}

	static bool killProcessByID(DWORD processID) {
		HANDLE tmpHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, processID);
		if (NULL != tmpHandle)
		{
			TerminateProcess(tmpHandle, 0);
			return true;
		}
		return false;
	}

	vector<Process> ListProcessName()
	{
		vector<DWORD> listProcess = ListProcess();
		vector<Process> listName;

		for (int i = 0; i < listProcess.size(); i++) {
			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

			// Get a handle to the process.

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
				PROCESS_VM_READ,
				FALSE, listProcess[i]);

			// Get the process name.

			if (NULL != hProcess)
			{
				HMODULE hMod;
				DWORD cbNeeded;

				if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
					&cbNeeded))
				{
					GetModuleBaseName(hProcess, hMod, szProcessName,
						sizeof(szProcessName) / sizeof(TCHAR));

					listName.push_back(Process(CString(szProcessName), listProcess[i]));
				}
			}

			CloseHandle(hProcess);
		}

		return listName;
	}

	vector<DWORD> ListProcess() {
		// Get the list of process identifiers.

		DWORD aProcesses[1024], cbNeeded, cProcesses;
		vector<DWORD> list;
		unsigned int i;

		if (EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		{
			// Calculate how many process identifiers were returned.
			cProcesses = cbNeeded / sizeof(DWORD);

			// Print the name and process identifier for each process.
			for (i = 0; i < cProcesses; i++)
			{
				if (aProcesses[i] != 0)
				{
					list.push_back(aProcesses[i]);
				}
			}
		}

		return list;
	}
};