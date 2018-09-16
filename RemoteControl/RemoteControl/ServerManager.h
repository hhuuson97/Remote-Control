/*
Coded by Robel Sharma
Date: 20-08-2013
If you use in any product please
make sure to write my credits

*/
#pragma once

#include "stdafx.h"
#include <string>
#include <vector>
#include <winsock2.h>
#include "Process.h"
#include "ScreenShot.h"
#include "ProcessManager.h"
 
using namespace std;

#pragma comment(lib,"ws2_32.lib")

class CRemoteControlDlg;

class ServerManager
{
public:
	ServerManager(CRemoteControlDlg* dialog);
	~ServerManager(void);

	void StartListening(int iPort);
	
	static UINT __cdecl DataThreadFunc(LPVOID pParam);
	static UINT __cdecl UpdateThreadFunc(LPVOID param);
	static void KillProcessThreadFunc(int second, DWORD processID, bool(*fun_ptr)(DWORD));
	static void clock(int second, void(*fun_ptr)());
	void ClearServer();

	WSADATA wsa;
    SOCKET s , new_socket;
	//static SOCKET sArray[100];
    struct sockaddr_in server , client;
    int c;
    //static int iCount;
	int iTempCount;
	HANDLE m_Thread_handle[100];
	CWinThread *cpTh[100];

	static void turnOffComputer();
	static bool ReadFileAndSend(SOCKET socket, string path);
	static void cmdLine(string cmd);
	static void SendType(SOCKET socket, int type);
	static bool sendProcessList(vector<Process> list, SOCKET &socket);
	static bool sendWindowList(vector<wstring> list, SOCKET &socket);
	static bool sendScreenShot(ScreenShot &screenShot, SOCKET &socket);

	static vector<string> split(string s);
	static string convertCString2String(CString st);
	static string convertWString2String(wstring st);
	static CString long2CString(long long x);
	static int string2int(string s);
};

