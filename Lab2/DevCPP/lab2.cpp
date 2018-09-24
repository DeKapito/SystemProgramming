#include "stdafx.h"
#include <windows.h>
#include <iostream>

using namespace std;

#define BUFSIZE 4096 

HANDLE childStd_IN_Rd = NULL;
HANDLE childStd_IN_Wr = NULL;
HANDLE childStd_OUT_Rd = NULL;
HANDLE childStd_OUT_Wr = NULL;

void ReadFromPipe()
{
	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE];
	BOOL success = FALSE;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	while (1)
	{
		success = ReadFile(childStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		if (!success || dwRead == 0) break;

		/*if (chBuf[dwRead - 1] == '>')
		{
			cout.write(chBuf, dwRead);
			break;
		}*/
		success = WriteFile(hParentStdOut, chBuf,
		dwRead, &dwWritten, NULL);
		if (!success) break;
	}
}

int main()
{
	SECURITY_ATTRIBUTES sAttrib;

	sAttrib.nLength = sizeof(SECURITY_ATTRIBUTES);
	sAttrib.bInheritHandle = TRUE;
	sAttrib.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&childStd_OUT_Rd, &childStd_OUT_Wr, &sAttrib, 0))
		cout << "Error (CreatePipe)";

	if (!SetHandleInformation(childStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		cout << "Error (SetHandleInformation)";

	if (!CreatePipe(&childStd_IN_Rd, &childStd_IN_Wr, &sAttrib, 0))
		cout << "Error (CreatePipe2)";

	if (!SetHandleInformation(childStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
		cout << "Error (SetHandleInformation2)";

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
	startInfo.dwFlags |= STARTF_USESTDHANDLES | STARTF_USECOUNTCHARS;

	// Create the child process. 

	success = CreateProcess(NULL,
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
		cout << "Error (CreateProcess)" << GetLastError();
	else
	{
		// Close handles to the child process and its primary thread.
		// Some applications might keep these handles to monitor the status
		// of the child process, for example. 

		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
	}

	CloseHandle(childStd_IN_Wr);
	CloseHandle(childStd_IN_Rd);
	///////////////
	ReadFromPipe();

	//DWORD dwRead, dwWritten;
	//CHAR chBuf[BUFSIZE];

	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE];
	success = FALSE;
	//CloseHandle(childStd_IN_Wr);
	for (;;)
	{
		//ReadFile(childStd_IN_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		//if (success || dwRead == 0) break;
		//cin >> chBuf;
		cin.read(chBuf, 5);
		//strcat_s(chBuf, "\n");
		
		//cout.write(chBuf, dwRead);
	
		/*success = WriteFile(childStd_IN_Wr, chBuf, dwRead, &dwWritten, NULL);
		if (!success) break;*/

		ReadFromPipe();
	}

	cout << "Test";

	// Close the pipe handle so the child process stops reading. 

	if (!CloseHandle(childStd_IN_Wr))
		cout << "StdInWr CloseHandle";

	////////////////////


	system("pause");
    return 0;
}
