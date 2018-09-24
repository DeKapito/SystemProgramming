#include "stdafx.h"
#include <windows.h>
#include <iostream>

#pragma warning(disable : 4996)

using std::cout;
using std::endl;

#define BUFFER_SIZE 32767
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

void InfoKey(HKEY hKey)
{
	TCHAR    achKey[MAX_KEY_LENGTH];
	DWORD    cbName;                    
	TCHAR    achClass[MAX_PATH] = TEXT(""); 
	DWORD    cchClassName = MAX_PATH;
	DWORD    cSubKeys = 0;          
	DWORD    cbMaxSubKey;         
	DWORD    cchMaxClass;         
	DWORD    cValues;             
	DWORD    cchMaxValue;         
	DWORD    cbMaxValueData;      
	DWORD    cbSecurityDescriptor;
	FILETIME ftLastWriteTime;      

	DWORD i, retCode;

	TCHAR  achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;

	retCode = RegQueryInfoKey(
		hKey,                 
		achClass,             
		&cchClassName,        
		NULL,                 
		&cSubKeys,            
		&cbMaxSubKey,         
		&cchMaxClass,         
		&cValues,             
		&cchMaxValue,         
		&cbMaxValueData,      
		&cbSecurityDescriptor,
		&ftLastWriteTime);    

	if (cValues)
	{
		printf("\nProgramms on autorun: %d\n", cValues);

		for (i = 0, retCode = ERROR_SUCCESS; i<cValues; i++)
		{
			cchValue = MAX_VALUE_NAME;
			retCode = RegEnumValue(hKey, i,
				achValue,
				&cchValue,
				NULL,
				NULL,
				NULL,
				NULL);

			if (retCode == ERROR_SUCCESS)
			{
				wprintf(TEXT("\t(%d) %s\n"), i + 1, achValue);
			}
		}
	}
}

void MeasureFrequency()
{
	LARGE_INTEGER	frequency;
	LARGE_INTEGER	startingTime, 
					endingTime, 
					elapsedMicroseconds;

	QueryPerformanceCounter(&startingTime);
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&endingTime);

	elapsedMicroseconds.QuadPart = endingTime.QuadPart - startingTime.QuadPart;
	elapsedMicroseconds.QuadPart *= 1000000;
	elapsedMicroseconds.QuadPart /= frequency.QuadPart;

	wprintf(TEXT("\nFrequency:   %d"), frequency.QuadPart);
	wprintf(TEXT("\nNumber of tacts:   %d"), elapsedMicroseconds.QuadPart);
}

void VolumesInfo()
{
	HANDLE	FindHandle = INVALID_HANDLE_VALUE;
	TCHAR	infoBuf[MAX_PATH];
	DWORD	bufCharCount = MAX_PATH + 1;

	FindHandle = FindFirstVolumeW(infoBuf, bufCharCount);
	wprintf(TEXT("\n\nVolume:   %s"), infoBuf);

	PWCHAR	Names = NULL;
	PWCHAR	NameIdx = NULL;
	PWCHAR	Volume = infoBuf;
	BOOL	Success = FALSE;

	while(1)
	{
		bufCharCount = MAX_PATH + 1;
		Names = (PWCHAR) new BYTE[bufCharCount * sizeof(WCHAR)];

		GetVolumePathNamesForVolumeName(Volume, Names, bufCharCount, &bufCharCount);

		for (NameIdx = Names;
			NameIdx[0] != L'\0';
			NameIdx += wcslen(NameIdx) + 1)
		{
			wprintf(TEXT("\nVolumePath:  %s"), NameIdx);
		}
		wprintf(L"\n");

		ULARGE_INTEGER freeBytes, totalBytes, tolalFreeBytes;

		GetDiskFreeSpaceEx(Volume, &freeBytes, &totalBytes, &tolalFreeBytes);
		wprintf(TEXT("\tTotal storage(MB): %d"), totalBytes.QuadPart / (1024 * 1024));
		wprintf(TEXT("\n\tFree storage(MB): %d"), freeBytes.QuadPart / (1024 * 1024));

		bufCharCount = MAX_PATH + 1;
		Success = FindNextVolumeW(FindHandle, Volume, bufCharCount);
		wprintf(TEXT("\n\nVolume:   %s"), Volume);

		if (!Success)
		{
			break;
		}
	}
}

int main()
{
	OSVERSIONINFO	osvi;
	TCHAR			infoBuf[BUFFER_SIZE];
	DWORD			bufCharCount = BUFFER_SIZE;
	DWORD			Error = ERROR_SUCCESS;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);
	cout << "OS version: " << osvi.dwBuildNumber;
	
	GetSystemDirectory(infoBuf, BUFFER_SIZE);
	wprintf( TEXT("\nSystem Directory:   %s"), infoBuf );

	bufCharCount = BUFFER_SIZE;
	GetComputerName(infoBuf, &bufCharCount);
	wprintf(TEXT("\nComputer name:   %s"), infoBuf);

	bufCharCount = BUFFER_SIZE;
	GetUserName(infoBuf, &bufCharCount);
	wprintf(TEXT("\nUser name:   %s"), infoBuf);

	VolumesInfo();

	HKEY hKey;
	LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, 
								TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\"), 
								0, KEY_READ | KEY_WOW64_64KEY, 
								&hKey);
	InfoKey(hKey);

	MeasureFrequency();

	system("pause");

    return 0;
}



