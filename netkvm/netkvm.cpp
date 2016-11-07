#include <stdio.h>
#include <vector>
#include <tuple>
#include <ws2tcpip.h>
#include <Windows.h>
#include <stdlib.h>
#include "Common.h"
#define _WIN32_WINNT 0x0501
#pragma comment(lib,"ws2_32.lib")

typedef void(*combo_ft)(void);
typedef std::vector<unsigned char> combo_kt;
typedef std::tuple<combo_kt, combo_ft> combo_t;

static bool kvmactive;
static unsigned short activekvm;
static bool activeKeys[256];
static POINT mousePos;
static POINT switchMousePos;
static std::vector<combo_t> comboList;

static std::vector<SOCKET> clients;
static unsigned char expectedClients;
static const PCSTR DEFAULT_PORT = "8080";

static char * SUPPLIED_PORT = NULL;
static char * SUPPLIED_PASSWORD = NULL;
static DWORD adminThreadId;
static std::vector<DWORD> adminProcessThreadIds;

DWORD handleEvent(unsigned char vkCode, WPARAM wp);
DWORD WINAPI AdminThread(LPVOID);
DWORD WINAPI AdminProcessThread(LPVOID);

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	BOOL fEatKeystroke = FALSE;
	if (nCode == HC_ACTION)
	{
		PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
		return handleEvent(p->vkCode, wParam);
	}
	return 0;
}
combo_t makeCombo(combo_kt kbd, combo_ft fnc)
{
	return std::make_tuple(kbd, fnc);
}
combo_kt getComboKbd(combo_t c)
{
	return std::get<0>(c);
}
combo_ft getComboFnc(combo_t c)
{
	return std::get<1>(c);
}
void toggleKVM(void)
{
	activekvm = (activekvm + 1) % (clients.size() + 1);
	kvmactive = activekvm == 0;
	printf("KVM Toggled\t%01x\n", activekvm);
}
combo_t makeToggleKVM()
{
	combo_kt combo;
	combo.push_back(0xA2); // left control
	combo.push_back(0x5B); // left win
	combo.push_back(0x4B); // K
	return makeCombo(combo, toggleKVM);
}
bool waitClients(void)
{
	SOCKET listener = INVALID_SOCKET;
	SOCKET client = INVALID_SOCKET;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int iResult, clientnum;
	printf("Setting up listener...\t\t\t");
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("[FAIL]\t%d\n", iResult);
		return false;
	}
	printf("[DONE]\n");
	printf("Creating listener...\t\t\t");
	if ((listener = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == INVALID_SOCKET)
	{
		printf("[FAIL]\t%ld\n", WSAGetLastError());
		freeaddrinfo(result);
		return false;
	}
	printf("[DONE]\n");
	printf("Setting up TCP...\t\t\t");
	if ((iResult = bind(listener, result->ai_addr, (int)result->ai_addrlen)) == SOCKET_ERROR)
	{
		printf("[FAIL]\t%ld\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listener);
		return false;
	}
	printf("[DONE]\n");
	freeaddrinfo(result);
	printf("Starting listener...\t\t\t");
	if ((iResult = listen(listener, SOMAXCONN)) == SOCKET_ERROR)
	{
		printf("[FAIL] %d\n", WSAGetLastError());
		closesocket(listener);
		return false;
	}
	printf("[DONE]\n");
	for (clientnum = 0; clientnum < expectedClients; ++clientnum)
	{
		printf("Waiting for client %d...\t\t\t", clientnum + 1);
		fflush(stdout);
		if ((client = accept(listener, NULL, NULL)) == INVALID_SOCKET)
		{
			printf("[FAIL]\t%d\n", WSAGetLastError());
			continue;
		}
		clients.push_back(client);
		printf("[DONE]\n");
	}
	closesocket(listener);
	printf("\tREADY\n");
}
void init(void)
{
	int i;
	WSAData wsa;
	for (i = 0; i < 256; i++)
		activeKeys[i] = false;
	kvmactive = true;
	activekvm = 0;
	comboList.clear();
	comboList.push_back(makeToggleKVM());
	printf("Initialising Winsock...\t\t\t");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("[FAIL]\t%d\n", WSAGetLastError());
		exit(-1);
	}
	printf("[DONE]\n");
	if (!waitClients())
		goto fail;
	if (SUPPLIED_PASSWORD != NULL)
	{
		printf("Starting Administration thread\n");
		CreateThread(
			NULL,
			0,
			AdminThread,
			NULL,
			0,
			&adminThreadId);
	}
	return;
fail:
	WSACleanup();
	exit(-1);
}
bool processCombo(combo_kt c, combo_kt current)
{
	register int i, j;
	const register int isize = c.size();
	const register int csize = current.size();
	for (i = 0; i < isize; ++i)
	{
		bool exists = false;
		for (j = 0; j < csize && !exists; ++j)
		{
			if (c.at(i) == current.at(j))
				exists = true;
		}
		if (!exists)
			return false;
	}
	return true;
}
combo_kt preprocess()
{
	combo_kt ret;
	register unsigned short i;
	for (i = 0; i < 256; i++)
		if (activeKeys[i])
			ret.push_back(i);
	return ret;
}
bool postprocess(combo_kt current)
{
	combo_t check;
	int i;
	const register int size = comboList.size();
	for (i = 0; i < size; ++i)
	{
		const combo_t c = comboList.at(i);
		const combo_kt c_kbd = getComboKbd(c);
		const combo_ft c_fnc = getComboFnc(c);
		if (processCombo(c_kbd, current))
		{
			c_fnc();
			return true;
		}
	}
	return false;
}
DWORD handleEvent(unsigned char vkCode, WPARAM wp)
{
	combo_kt current;
	SOCKET client;
	bool change = false;
	unsigned char buffer[2];
	unsigned char keydirection;
	switch (wp)
	{
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		if (!activeKeys[vkCode])
		{
			printf("Activate: 0x%02x\n", vkCode);
			change = true;
		}
		keydirection = A_KBDOWN;
		activeKeys[vkCode] = true;
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		if (activeKeys[vkCode])
		{
			printf("Deactivate: 0x%02x\n", vkCode);
			change = true;
		}
		keydirection = A_KBUP;
		activeKeys[vkCode] = false;
		break;
	}
	if (change)
	{
		current = preprocess();
		if (postprocess(current))
			return 1;
		else if (!kvmactive)
		{
			return 0;
			client = clients.at(activekvm - 1);
			buffer[0] = keydirection;
			buffer[1] = vkCode;
			send(client, (const char *)buffer, 2, 0);
		}
	}
	return !kvmactive;
}
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	MOUSEHOOKSTRUCT * p = (MOUSEHOOKSTRUCT *)lParam;
	MSLLHOOKSTRUCT * pMouseStruct = (MSLLHOOKSTRUCT *)lParam;
	unsigned char buffer[16];
	SOCKET client;
	short zDelta;
	if (!kvmactive)
	{
		client = clients.at(activekvm - 1);
		switch (wParam)
		{
		case WM_LBUTTONDOWN:
			buffer[0] = A_MLDOWN;
			send(client, (const char *)buffer, 1, 0);
			break;
		case WM_LBUTTONUP:
			buffer[0] = A_MLUP;
			send(client, (const char *)buffer, 1, 0);
			break;
		case WM_RBUTTONDOWN:
			buffer[0] = A_MRDOWN;
			send(client, (const char *)buffer, 1, 0);
			break;
		case WM_RBUTTONUP:
			buffer[0] = A_MRUP;
			send(client, (const char *)buffer, 1, 0);
			break;
		case WM_MOUSEWHEEL:
			zDelta = GET_WHEEL_DELTA_WPARAM(pMouseStruct->mouseData);
			break;
		case WM_MOUSEMOVE:
			buffer[0] = A_MMOVE;
			long dx, dy;
			dx = p->pt.x - mousePos.x;
			dy = p->pt.y - mousePos.y;
			send(client, (const char *)buffer, 1, 0);
			send(client, (const char *)&dx, sizeof(long), 0);
			send(client, (const char *)&dy, sizeof(long), 0);
			break;
		}
		return 1;
	}
	mousePos = p->pt;
	return 0;
}

DWORD WINAPI AdminThread(LPVOID)
{
	SOCKET listener = INVALID_SOCKET;
	SOCKET client = INVALID_SOCKET;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int iResult;
	DWORD threadId;
	printf("[ADMIN] Setting up listener...\t\t\t");
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("[FAIL]\t%d\n", iResult);
		return false;
	}
	printf("[DONE]\n");
	printf("[ADMIN] Creating listener...\t\t\t");
	if ((listener = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == INVALID_SOCKET)
	{
		printf("[FAIL]\t%ld\n", WSAGetLastError());
		freeaddrinfo(result);
		return false;
	}
	printf("[DONE]\n");
	printf("[ADMIN] Setting up TCP...\t\t\t");
	if ((iResult = bind(listener, result->ai_addr, (int)result->ai_addrlen)) == SOCKET_ERROR)
	{
		printf("[FAIL]\t%ld\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listener);
		return false;
	}
	printf("[DONE]\n");
	freeaddrinfo(result);
	printf("[ADMIN] Starting listener...\t\t\t");
	if ((iResult = listen(listener, SOMAXCONN)) == SOCKET_ERROR)
	{
		printf("[FAIL] %d\n", WSAGetLastError());
		closesocket(listener);
		return false;
	}
	printf("[READY]\n");
	for (;;)
	{
		fflush(stdout);
		if ((client = accept(listener, NULL, NULL)) == INVALID_SOCKET)
			continue;
		CreateThread(
			NULL,
			0,
			AdminProcessThread,
			(LPVOID)client,
			0,
			&threadId);
		adminProcessThreadIds.push_back(threadId);
		printf("[ADMIN] Accepted client\n");
	}
	return 0;
}
DWORD WINAPI AdminProcessThread(LPVOID arg)
{
	SOCKET client = (SOCKET)arg;
	return 0;
}

int main(int argc, char ** argv)
{
	MSG msg;
	HHOOK hhkLowLevelKybd, hhkLowLevelMouse;
	expectedClients = 1;
	if (argc > 1)
	{
		//parse out port
		SUPPLIED_PORT = strdup(argv[1]);
		if (argc > 2)
		{
			//parse out password
			SUPPLIED_PASSWORD = strdup(argv[2]);
		}
	}
	init();
	hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
	hhkLowLevelMouse = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, 0, 0);
	while (!GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnhookWindowsHookEx(hhkLowLevelKybd);
	UnhookWindowsHookEx(hhkLowLevelMouse);
	return 0;
}