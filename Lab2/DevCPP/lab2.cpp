#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

#define BUFSIZE 4096 

HANDLE childStd_IN_Rd = NULL;
HANDLE childStd_IN_Wr = NULL;
HANDLE childStd_OUT_Rd = NULL;
HANDLE childStd_OUT_Wr = NULL;

void ReadFromPipe()
{
	DWORD dwRead;
	CHAR chBuf[BUFSIZE];
	BOOL success = FALSE;
	//HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	while (1)
	{
		success = ReadFile(childStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		if (!success || dwRead == 0) break;

		cout.write(chBuf, dwRead);

		if (chBuf[dwRead - 1] == '>')
		{
			//cout.write(chBuf, dwRead);
			break;
		}
		/*success = WriteFile(hParentStdOut, chBuf, dwRead, &dwWritten, NULL);
		if (!success) break;*/
	}
}

int main()
{
	SECURITY_ATTRIBUTES sAttrib;

	sAttrib.nLength = sizeof(SECURITY_ATTRIBUTES);
	sAttrib.bInheritHandle = TRUE;
	sAttrib.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&childStd_OUT_Rd, &childStd_OUT_Wr, &sAttrib, 0))
		cout << "Error code (CreatePipe): " << GetLastError();

	if (!SetHandleInformation(childStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		cout << "Error code (SetHandleInformation): " << GetLastError();

	if (!CreatePipe(&childStd_IN_Rd, &childStd_IN_Wr, &sAttrib, 0))
		cout << "Error code (CreatePipe2): " << GetLastError();

	if (!SetHandleInformation(childStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
		cout << "Error code (SetHandleInformation2): " << GetLastError();

	TCHAR command[] = TEXT("cmd");
	PROCESS_INFORMATION procInfo;
	STARTUPINFO startInfo;
	BOOL success = FALSE;

	ZeroMemory(&procInfo, sizeof(PROCESS_INFORMATION));

	ZeroMemory(&startInfo, sizeof(STARTUPINFO));
	startInfo.cb = sizeof(STARTUPINFO);
	startInfo.hStdError = childStd_OUT_Wr;
	startInfo.hStdOutput = childStd_OUT_Wr;
	startInfo.hStdInput = childStd_IN_Rd;
	startInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process. 

	success = CreateProcess(TEXT("C:\\WINDOWS\\system32\\cmd.exe"),
		command,     // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&startInfo,  // STARTUPINFO pointer 
		&procInfo);  // receives PROCESS_INFORMATION 

	if (!success)
		cout << "Error code (CreateProcess): " << GetLastError();
	else
	{
		CloseHandle(childStd_OUT_Wr);;
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
	}

	//ReadFromPipe();

	//DWORD dwRead, dwWritten;
	//CHAR chBuf[BUFSIZE];

	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE];
	success = FALSE;
	//CloseHandle(childStd_IN_Wr);
	for (;;)
	{
		ReadFromPipe();
		success = ReadFile(GetStdHandle(STD_INPUT_HANDLE), chBuf, BUFSIZE, &dwRead, NULL);
		if (!success)
			cout << "Error code (ReadFile(STD_INPUT_HANDLE)): " << GetLastError();

		string str = string(chBuf).substr(0, dwRead);

		if (str.substr(0, 6) != "please")
		{
			cout << "Please, ask politely!";
		}
	
		success = WriteFile(childStd_IN_Wr, chBuf, dwRead, &dwWritten, NULL);
		if (!success)
			cout << "Error code(WriteFile): " << GetLastError();	
	}

	// Close the pipe handle so the child process stops reading. 

	if (!CloseHandle(childStd_IN_Wr))
		cout << "Error code(StdInWr CloseHandle): " << GetLastError();

	system("pause");
    return 0;
}
