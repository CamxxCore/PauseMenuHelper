#include "stdafx.h"

#include <TlHelp32.h>

#include <winternl.h>

HANDLE ThreadLocal::GetDefaultThread()
{
	HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (hThreadSnapshot == INVALID_HANDLE_VALUE)
		return NULL;

	HANDLE hThread = NULL;

	THREADENTRY32 te32 = { 0 };

	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnapshot, &te32))
		return NULL;

	// Get the current process ID. 
	DWORD dwCurrentProcessId = GetCurrentProcessId();

	// Loop through all threads in every process. 
	do
	{
		// Only current process, please. 
		if (te32.th32OwnerProcessID != dwCurrentProcessId)
			continue;

		// Open a handle to the thread. 
		hThread = OpenThread(GENERIC_ALL, FALSE, te32.th32ThreadID);
		if (!hThread)
			return NULL;

		break;
	} while (Thread32Next(hThreadSnapshot, &te32));

	CloseHandle(hThreadSnapshot);

	return hThread;
}

TEB_T * ThreadLocal::GetTeb(HANDLE hThread)
{
	TEB_T *pTeb;

	if (hThread == GetCurrentThread())
	{
		pTeb = GetCurrentTeb();
	}

	else
	{
		_THREAD_BASIC_INFORMATION info = { 0 };

		NTQUERYINFOMATIONTHREAD NtQueryInformationThread =
			(NTQUERYINFOMATIONTHREAD)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread");

		if (NtQueryInformationThread == NULL) return 0;

		NTSTATUS status = NtQueryInformationThread(hThread, (THREADINFOCLASS)ThreadBasicInformation, &info, sizeof(info), NULL);

		if (!NT_SUCCESS(status)) return 0;

		pTeb = (TEB_T*)info.TebBaseAddress;
	}

	return pTeb;
}

TEB_T * ThreadLocal::GetCurrentTeb()
{
	return (TEB_T*)NtCurrentTeb();
}

DWORD64 ThreadLocal::GetThreadLocalStorage(HANDLE hThread)
{
	TEB_T * pTeb = GetTeb(hThread);

	if (!pTeb) return NULL;

	return pTeb->ThreadLocalStoragePointer;
}

bool ThreadLocal::ReplaceThreadTls(HANDLE hThread, DWORD64 newTls, DWORD64 *oldTls)
{
	TEB_T *pTeb = GetTeb(hThread);

	*oldTls = pTeb->ThreadLocalStoragePointer;

	pTeb->ThreadLocalStoragePointer = newTls;

	return true;
}
