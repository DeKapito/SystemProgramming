#include "stdafx.h"
#include <windows.h>

#define BUFSIZE 4096 

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;

HANDLE g_hInputFile = NULL;

int main()
{
	SECURITY_ATTRIBUTES saAttr;

	printf("\n->Start of parent execution.\n");

    return 0;
}

