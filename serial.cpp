#include "serial.h"
#include "stdlib.h"

#include <TCHAR.H>  
#include <TCHAR.H>   
#include "string.h"

using namespace std;

HANDLE hCom;

LPCWSTR stringToLPCWSTR(string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}

int serial_open(char* comname, int baudRate)
{
	char COMx[20] = "\\\\.\\";

	strcat(COMx, comname);

	string com = ("%s", COMx);

	hCom = CreateFile(stringToLPCWSTR(com),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (hCom == INVALID_HANDLE_VALUE)
	{
		printf("serial: open err!\n");
		return FALSE;
	}

	SetupComm(hCom, 1024*4, 1024);

	//…Ë∂®∂¡–¥≥¨ ± 
	COMMTIMEOUTS TimeOuts;
	TimeOuts.ReadIntervalTimeout = 10;
	TimeOuts.ReadTotalTimeoutMultiplier = 0;
	TimeOuts.ReadTotalTimeoutConstant = 0;
	TimeOuts.WriteTotalTimeoutMultiplier = 500;
	TimeOuts.WriteTotalTimeoutConstant = 2000;
	SetCommTimeouts(hCom, &TimeOuts);
	
	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = baudRate;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	SetCommState(hCom, &dcb);

	PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);

	return TRUE;
}

int serial_read(uint8_t* data, int size)
{
	DWORD rCount;
	BOOL bReadStat;

	bReadStat = ReadFile(hCom, data, size, &rCount, NULL);
	if (!bReadStat)
	{
		printf("serial: read err!\n");
		return FALSE;
	}
	return rCount;
}

int serial_write(char lpOutBuffer[])
{
	DWORD dwBytesWrite = sizeof(lpOutBuffer);
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;

	ClearCommError(hCom, &dwErrorFlags, &ComStat);
	bWriteStat = WriteFile(hCom, lpOutBuffer, dwBytesWrite, &dwBytesWrite, NULL);
	if (!bWriteStat)
	{
		printf("serial: write err!\n");
		return FALSE;
	}

	PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	return TRUE;
}

void serial_close(void)
{
	CloseHandle(hCom);
}
