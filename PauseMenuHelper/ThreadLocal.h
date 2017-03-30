#pragma once

class ThreadLocal sealed
{
public:
	static HANDLE GetDefaultThread();

	static TEB_T * GetCurrentTeb();

	static uintptr_t GetThreadLocalStorage(HANDLE hTargetThread);

	static bool ReplaceThreadTls(HANDLE hThread, DWORD64 newTls, DWORD64 *oldTls);
};

