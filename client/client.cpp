#define _WIN32_WINNT 0x0501
#include <stdio.h>
#include <vector>
#include <tuple>
#include <ws2tcpip.h>
#include <Windows.h>
#include "Common.h"
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

static PCSTR DEFAULT_PORT = "8080";
static PCSTR DEFAULT_ADDR = "192.168.0.199";
static char *SUPPLIED_ADDR = NULL;
static SOCKET server;

POINT gmousePos;

void processKey(unsigned char code);
void processMousePosition(void);
void processMouseButton(unsigned char code);

bool connectServer(void)
{
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	int iResult, i;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	printf("Setting up socket...\t\t\t");
	if ((iResult = getaddrinfo(SUPPLIED_ADDR == NULL ? DEFAULT_ADDR : DEFAULT_ADDR, DEFAULT_PORT, &hints, &result)) != 0)
	{
		printf("[FAIL]\t%d\n", iResult);
		return false;
	}
	printf("[DONE]\n");
	for (ptr = result, i = 0; ptr != NULL; ptr = ptr->ai_next, i++)
	{
		if (i == 0) printf("Attempting to connect...\t\t");
		else printf("Attempting to connect (%d)...\t\t", i);
		if ((ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == INVALID_SOCKET)
		{
			printf("[FATAL]\t%ld\n", WSAGetLastError());
			return false;
		}
		if ((iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen)) == SOCKET_ERROR)
		{
			printf("[FAIL]\n");
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	if (ConnectSocket != INVALID_SOCKET)
		printf("[DONE]\n");
	freeaddrinfo(result);
	printf("Finalizing connection...\t\t");
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("[FAIL]\n");
		return false;
	}
	server = ConnectSocket;
	printf("[DONE]\n");
}
void init(void)
{
	WSAData wsa;
	printf("Initialising Winsock...\t\t\t");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("[FAIL]\t%d\n", WSAGetLastError());
		exit(-1);
	}
	printf("[DONE]\n");
	if (!connectServer())
		goto fail;
	GetCursorPos(&gmousePos);
	return;
fail:
	WSACleanup();
	exit(-1);
}
void process(void)
{
	unsigned char code;
	for (;;)
	{
		if (recv(server, (char *)&code, 1, 0) == 1)
		{
			//printf("\n\nCode: 0x%02x\n", code);
			switch (code)
			{
			case A_KBDOWN:
			case A_KBUP:
				processKey(code);
				break;
			case A_MLDOWN:
			case A_MLUP:
			case A_MRDOWN:
			case A_MRUP:
				processMouseButton(code);
				break;
			case A_MMOVE:
				processMousePosition();
				break;
			case A_MWDOWN:
			case A_MWUP:
				break;
			}
		}
	}
}

void processKey(unsigned char code)
{
	unsigned char vkCode;
	if (recv(server, (char *)&vkCode, 1, 0) == 1)
	{
		switch (code)
		{
		case A_KBDOWN:
			printf("DOWN: 0x%02x\n", vkCode);
			keybd_event(vkCode, 0, 0, 0);
			break;
		case A_KBUP:
			printf("UP: 0x%02x\n", vkCode);
			keybd_event(vkCode, 0, KEYEVENTF_KEYUP, 0);
			break;
		}
	}
}
void processMousePosition(void)
{
	long x, y;
	INPUT input;
	recv(server, (char *)&x, sizeof(long), 0);
	recv(server, (char *)&y, sizeof(long), 0);
	gmousePos.x += x;
	gmousePos.y += y;
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.time = 0; //prevent screen from sleeping
	input.mi.dx = gmousePos.x *(65536 / GetSystemMetrics(SM_CXSCREEN));//x being coord in pixels
	input.mi.dy = gmousePos.y *(65536 / GetSystemMetrics(SM_CYSCREEN));//y being coord in pixels
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	SendInput(1, &input, sizeof(input));
}
void processMouseButton(unsigned char code)
{
	POINT mousePos;
	INPUT input;
	GetCursorPos(&mousePos);
	switch (code)
	{
	case A_MLDOWN:
		input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		break;
	case A_MLUP:
		input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		break;
	case A_MRDOWN:
		input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
		break;
	case A_MRUP:
		input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		break;
	}
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.time = 0; //prevent screen from sleeping
	input.mi.dx = mousePos.x *(65536 / GetSystemMetrics(SM_CXSCREEN));//x being coord in pixels
	input.mi.dy = mousePos.y *(65536 / GetSystemMetrics(SM_CYSCREEN));//y being coord in pixels
	SendInput(1, &input, sizeof(input));
}

int main(int argc, char ** argv)
{
	if (argc > 1)
	{
		SUPPLIED_ADDR = new char[strlen(argv[1] + 5)];
		strcpy(SUPPLIED_ADDR, argv[1]);
		printf("Using address: %s\n", SUPPLIED_ADDR);
	}
	init();
	process();
	printf("Exit\n");
	getchar();
	closesocket(server);
	WSACleanup();
	return 0;
}