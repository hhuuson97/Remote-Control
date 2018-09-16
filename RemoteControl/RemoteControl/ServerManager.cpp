#include "StdAfx.h"
#include "ServerManager.h"
#include <thread>
#include <stdlib.h>
#include <Windows.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <fstream>

#include "KeyBoardControl.h"
#include "MouseControl.h"
#include "ScreenShot.h"
#include "ProcessManager.h"
#include "WindowManager.h"

#include "RemoteControlDlg.h"

static CRemoteControlDlg* m_pDialog;
static SOCKET sClient;
static CWinThread *cTh = nullptr;
static CWinThread *cTh2 = nullptr;
static KeyBoardControl keyBoardControl;
static MouseControl mouseControl;
static ScreenShot screenShot;
static ProcessManager processManager;
static WindowManager windowManager;

ServerManager::ServerManager(CRemoteControlDlg *dialog)
{
	m_pDialog = dialog;
}


ServerManager::~ServerManager()
{
	
	closesocket(s);
    WSACleanup();
}

void ServerManager::ClearServer()
{
	closesocket(s);
    WSACleanup();

	/*
	for(int i=1;i<=iCount;++i)
	{
		DWORD dwCode;  
        GetExitCodeThread(cpTh[i]->m_hThread, &dwCode);  
        delete cpTh[i];
		//CloseHandle(m_Thread_handle[i]);
	}*/
}

void ServerManager::StartListening(int iPort)
{
	printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return;
    }
     
    printf("Initialised.\n");
    
    //Create a socket
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
 
    printf("Socket created.\n");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(iPort);
     
    //Bind
    if (::bind(s, (struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
     
    puts("Bind done");
 
    // Listen to incoming connections
    listen(s, 1);
	// m_pDialog->AddInfo(_T("Waiting for incoming connections..."));
    c = sizeof(struct sockaddr_in);
     
    while((new_socket = accept(s , (struct sockaddr *)&client, &c)) != INVALID_SOCKET)
    {
		puts("Connection accepted");
		m_pDialog->AddInfo(_T("Connection accepted\n"));
		//Reply to the client
		socklen_t len;
		struct sockaddr_storage addr;
		char ipstr[INET6_ADDRSTRLEN];
		int port;

		len = sizeof addr;
		getpeername(new_socket, (struct sockaddr*)&addr, &len);

		// deal with IPv4:
		if (addr.ss_family == AF_INET) {
			struct sockaddr_in *s = (struct sockaddr_in *)&addr;
			port = ntohs(s->sin_port);
			inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
		}

		printf("Peer IP address: %s\n", ipstr);
		if (cTh != nullptr) {
			WaitForSingleObject(cTh, INFINITE);
		}
		cTh = AfxBeginThread(DataThreadFunc, (LPVOID)new_socket);
		cTh2 = AfxBeginThread(UpdateThreadFunc, (LPVOID)new_socket);
		sClient = new_socket;
    }
     
    if (new_socket == INVALID_SOCKET)
    {
        printf("accept failed with error code : %d" , WSAGetLastError());
        return;
    }
}

UINT __cdecl ServerManager::DataThreadFunc(LPVOID pParam)
{
	SOCKET socket = reinterpret_cast<SOCKET>(pParam);

	char reply[2000];
    int recv_size;
	string str;
	vector<string> param;

	while((recv_size = recv(socket, reply , 2000 , 0)) != SOCKET_ERROR)
	{
		str = string(reply, recv_size);
		m_pDialog->AddInfo(CString(str.c_str()));
		param = split(str);

		if (param[0].compare("viewSize") == 0) {
			mouseControl.setScreen(atoi(param[1].c_str()), atoi(param[2].c_str()));
		}

		if (param[0].compare("transfer") == 0) {
			for (int i = 2; i < param.size(); i++)
				param[1] += " " + param[i];
			SendType(socket, 1);
			ReadFileAndSend(socket, param[1]);
		}

		if (param[0].compare("mouse") == 0)
			mouseControl.onAction(atoi(param[1].c_str()), atoi(param[2].c_str()));

		if (param[0].compare("key") == 0) {
			int keyCode = string2int(param[1]);
			keyBoardControl.onAction(keyCode);
		}
		
		if (param[0].compare("changeState") == 0)
			mouseControl.changeState();

		if (param[0].compare("windowView") == 0) {
			for (int i = 2; i < param.size(); i++)
				param[1] += " " + param[i];
			screenShot.setWindow(param[1]);
		}

		if (param[0].compare("updateTime") == 0)
			screenShot.setUpdateTime(atoi(param[1].c_str()));

		if (param[0].compare("turnOffComputer") == 0) {
			thread t(clock, atoi(param[1].c_str()), turnOffComputer);
			t.join();
		}

		if (param[0].compare("cmdLine") == 0) {
			for (int i = 2; i < param.size(); i++)
				param[1] += " " + param[i];
			cmdLine(param[1]);
		}

		if (param[0].compare("killProcess") == 0) {
			thread t(KillProcessThreadFunc, atoi(param[2].c_str()), atoi(param[1].c_str()), &ProcessManager::killProcessByID);
			t.join();
		}

		if (param[0].compare("processList") == 0) {
			vector<Process> list = processManager.ListProcessName();
			SendType(socket, 2);
			sendProcessList(list, socket);
		}

		if (param[0].compare("windowList") == 0) {
			vector<wstring> list = windowManager.ListWindowName();
			SendType(socket, 4);
			sendWindowList(list, socket);
		}
	}
    return 0;
}

int ServerManager::string2int(string s) {
	int x = 0;
	for (int i = 0; i < s.size(); i++)
		x = x * 10 + s[i] - '0';
	return x;
}

bool ServerManager::sendProcessList(vector<Process> list, SOCKET &socket)
{
	size_t len = list.size();
	send(socket, (char *)&len, sizeof(len), 0);
	for (int i = 0; i < list.size(); i++) {
		wstring tmp(list[i].name);
		len = tmp.size();
		send(socket, (char *)&len, sizeof(len), 0);
		len *= 2;
		char *tm = new char[len];
		for (int i = 0; i < len; i += 2) {
			tm[i + 1] = tmp[i / 2] & 255;
			tm[i] = tmp[i / 2] >> 8;
		}
		send(socket, tm, len, 0);
		delete[]tm;
		send(socket, (char *)&list[i].id, sizeof(DWORD), 0);
	}
	return true;
}

bool ServerManager::sendWindowList(vector<wstring> list, SOCKET &socket)
{
	size_t len = list.size();
	send(socket, (char *)&len, sizeof(len), 0);
	for (int i = 0; i < list.size(); i++) {
		wstring tmp(list[i]);
		len = tmp.size();
		send(socket, (char *)&len, sizeof(len), 0);
		len *= 2;
		char *tm = new char[len];
		for (int i = 0; i < len; i += 2) {
			tm[i + 1] = tmp[i / 2] & 255;
			tm[i] = tmp[i / 2] >> 8;
		}
		send(socket, tm, len, 0);
		delete []tm;
	}
	return true;
}

bool ServerManager::sendScreenShot(ScreenShot &screenShot, SOCKET &socket)
{
	string path = "screenShot.jpg";
	screenShot.TakeScreenShot2(path);
	SendType(socket, 3);
	ReadFileAndSend(socket, path);
	return true;
}

CString ServerManager::long2CString(long long x) {
	string s;
	s = "";
	if (x == 0) return _T("0");
	while (x > 0) {
		s = (char)('0' + x % 10) + s;
		x /= 10;
	}
	return CString(s.c_str());
}

vector<string> ServerManager::split(string s) {
	vector<string> p;
	p.push_back("");
	int j = 0;
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == ' ') {
			p.push_back("");
			j++;
		}
		else p[j] += s[i];
	}
	return p;
}

void ServerManager::turnOffComputer() {
	cmdLine("shutdown -s");
}

void ServerManager::cmdLine(string cmd) {
	system(cmd.c_str());
}

string ServerManager::convertCString2String(CString st) {
	CT2CA pszConvertedAnsiString(st);
	string res(pszConvertedAnsiString);
	return res;
}

string ServerManager::convertWString2String(wstring st) {
	string res(st.begin(), st.end());
	return res;
}

void ServerManager::clock(int second, void(*fun_ptr)()) {
	for (int i = second; i > 0; i--)
		this_thread::sleep_for(std::chrono::seconds(1));
	fun_ptr();
}

UINT __cdecl ServerManager::UpdateThreadFunc(LPVOID param) {
	SOCKET socket = reinterpret_cast<SOCKET>(param);
	while (socket != INVALID_SOCKET) {
		sendScreenShot(screenShot, socket);
		for (int i = screenShot.updateTime; i > 0; i--)
			this_thread::sleep_for(std::chrono::seconds(1));
	}
	return 0;
}

void ServerManager::KillProcessThreadFunc(int second, DWORD processID, bool(*fun_ptr)(DWORD))
{
	for (int i = second; i > 0; i--)
		this_thread::sleep_for(std::chrono::seconds(1));
	fun_ptr(processID);
}

void ServerManager::SendType(SOCKET socket, int type) {
	send(socket, (char *)&type, sizeof(int), 0);
}

bool ServerManager::ReadFileAndSend(SOCKET socket, string path) {
	ifstream file(path.c_str(), ios::in | ios::binary);

	if (file.is_open() == false)
		return false;

	file.seekg(0, file.end);
	streamoff size = file.tellg();
	file.seekg(0, file.beg);
	send(socket, (char *)&size, sizeof(streamoff), 0);
	string name = "";
	for (int i = path.size() - 1; i >= 0 && path[i] != '/'; i--)
		name = path[i] + name;
	int len = name.size();
	streamsize glen;
	send(socket, (char *)&len, sizeof(len), 0);
	send(socket, name.c_str(), len, 0);
	char *memlock = new char[102400];
	streamoff gsize = 0;
	while (!file.eof()) {
		glen = file.read(memlock, 102400).gcount();
		gsize += glen;
		send(socket, memlock, glen, 0);
	}
	delete memlock;
	file.close();
	return true;
}