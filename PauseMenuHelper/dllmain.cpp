// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#pragma unmanaged
BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		initializeGame();
		break;
	case DLL_PROCESS_DETACH:
		unload();
	}
	return TRUE;
}

