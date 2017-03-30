#pragma once

#include "stdafx.h"

typedef NTSTATUS(WINAPI *NTQUERYINFOMATIONTHREAD)(HANDLE, LONG, PVOID, ULONG, PULONG);

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

extern "C" NTSYSAPI VOID NTAPI RtlAcquirePebLock();
extern "C" NTSYSAPI VOID NTAPI RtlReleasePebLock();

template <typename T>
struct _LIST_ENTRY_T
{
	T Flink;
	T Blink;
};

template <typename T>
struct _UNICODE_STRING_T
{
	WORD Length;
	WORD MaximumLength;
	T Buffer;
};

template <typename T>
struct _NT_TIB_T
{
	T ExceptionList;
	T StackBase;
	T StackLimit;
	T SubSystemTib;
	T FiberData;
	T ArbitraryUserPointer;
	T Self;
};

template <typename T>
struct _CLIENT_ID_T
{
	T UniqueProcess;
	T UniqueThread;
};

template <typename T>
struct _GDI_TEB_BATCH_T
{
	DWORD Offset;
	T HDC;
	DWORD Buffer[310];
};


template <typename T>
struct _TEB_T
{
	typedef T type;

	_NT_TIB_T<T> NtTib;
	T EnvironmentPointer;
	_CLIENT_ID_T<T> ClientId;
	T ActiveRpcHandle;
	T ThreadLocalStoragePointer;
	T ProcessEnvironmentBlock;
	DWORD LastErrorValue;
	DWORD CountOfOwnedCriticalSections;
	T CsrClientThread;
	T Win32ThreadInfo;
	DWORD User32Reserved[26];
	T UserReserved[5];
	T WOW32Reserved;
	DWORD CurrentLocale;
	DWORD FpSoftwareStatusRegister;
	T SystemReserved1[54];
	DWORD ExceptionCode;
	T ActivationContextStackPointer;
	BYTE SpareBytes[36];
	DWORD TxFsContext;
	_GDI_TEB_BATCH_T<T> GdiTebBatch;
	_CLIENT_ID_T<T> RealClientId;
	T GdiCachedProcessHandle;
	DWORD GdiClientPID;
	DWORD GdiClientTID;
	T GdiThreadLocalInfo;
	T Win32ClientInfo[62];
	T glDispatchTable[233];
	T glReserved1[29];
	T glReserved2;
	T glSectionInfo;
	T glSection;
	T glTable;
	T glCurrentRC;
	T glContext;
	DWORD LastStatusValue;
	_UNICODE_STRING_T<T> StaticUnicodeString;
	wchar_t StaticUnicodeBuffer[261];
	T DeallocationStack;
	T TlsSlots[64];
	_LIST_ENTRY_T<T> TlsLinks;
	T Vdm;
	T ReservedForNtRpc;
	T DbgSsReserved[2];
	DWORD HardErrorMode;
	T Instrumentation[11];
	_GUID ActivityId;
	T SubProcessTag;
	T PerflibData;
	T EtwTraceData;
	T WinSockData;
	DWORD GdiBatchCount;
	DWORD IdealProcessorValue;
	DWORD GuaranteedStackBytes;
	T ReservedForPerf;
	T ReservedForOle;
	DWORD WaitingOnLoaderLock;
	T SavedPriorityState;
	T ReservedForCodeCoverage;
	T ThreadPoolData;
	T TlsExpansionSlots;
	T DeallocationBStore;
	T BStoreLimit;
	DWORD MuiGeneration;
	DWORD IsImpersonating;
	T NlsCache;
	T pShimData;
	USHORT HeapVirtualAffinity;
	USHORT LowFragHeapDataSlot;
	T CurrentTransactionHandle;
	T ActiveFrame;
	T FlsData;
	T PreferredLanguages;
	T UserPrefLanguages;
	T MergedPrefLanguages;
	DWORD MuiImpersonation;
	USHORT CrossTebFlags;
	USHORT SameTebFlags;
	T TxnScopeEnterCallback;
	T TxnScopeExitCallback;
	T TxnScopeContext;
	DWORD LockCount;
	DWORD SpareUlong0;
	T ResourceRetValue;
	T ReservedForWdf;
};

template <typename T, typename NGF, int A>
struct _PEB_T
{
	typedef T type;

	union
	{
		struct
		{
			BYTE InheritedAddressSpace;
			BYTE ReadImageFileExecOptions;
			BYTE BeingDebugged;
			BYTE BitField;
		};
		T dummy01;
	};
	T Mutant;
	T ImageBaseAddress;
	T Ldr;
	T ProcessParameters;
	T SubSystemData;
	T ProcessHeap;
	T FastPebLock;
	T AtlThunkSListPtr;
	T IFEOKey;
	T CrossProcessFlags;
	T UserSharedInfoPtr;
	DWORD SystemReserved;
	DWORD AtlThunkSListPtr32;
	T ApiSetMap;
	T TlsExpansionCounter;
	T TlsBitmap;
	DWORD TlsBitmapBits[2];
	T ReadOnlySharedMemoryBase;
	T HotpatchInformation;
	T ReadOnlyStaticServerData;
	T AnsiCodePageData;
	T OemCodePageData;
	T UnicodeCaseTableData;
	DWORD NumberOfProcessors;
	union
	{
		DWORD NtGlobalFlag;
		NGF dummy02;
	};
	LARGE_INTEGER CriticalSectionTimeout;
	T HeapSegmentReserve;
	T HeapSegmentCommit;
	T HeapDeCommitTotalFreeThreshold;
	T HeapDeCommitFreeBlockThreshold;
	DWORD NumberOfHeaps;
	DWORD MaximumNumberOfHeaps;
	T ProcessHeaps;
	T GdiSharedHandleTable;
	T ProcessStarterHelper;
	T GdiDCAttributeList;
	T LoaderLock;
	DWORD OSMajorVersion;
	DWORD OSMinorVersion;
	WORD OSBuildNumber;
	WORD OSCSDVersion;
	DWORD OSPlatformId;
	DWORD ImageSubsystem;
	DWORD ImageSubsystemMajorVersion;
	T ImageSubsystemMinorVersion;
	T ActiveProcessAffinityMask;
	T GdiHandleBuffer[A];
	T PostProcessInitRoutine;
	T TlsExpansionBitmap;
	DWORD TlsExpansionBitmapBits[32];
	T SessionId;
	ULARGE_INTEGER AppCompatFlags;
	ULARGE_INTEGER AppCompatFlagsUser;
	T pShimData;
	T AppCompatInfo;
	_UNICODE_STRING_T<T> CSDVersion;
	T ActivationContextData;
	T ProcessAssemblyStorageMap;
	T SystemDefaultActivationContextData;
	T SystemAssemblyStorageMap;
	T MinimumStackCommit;
	T FlsCallback;
	_LIST_ENTRY_T<T> FlsListHead;
	T FlsBitmap;
	DWORD FlsBitmapBits[4];
	T FlsHighIndex;
	T WerRegistrationData;
	T WerShipAssertPtr;
	T pContextData;
	T pImageHeaderHash;
	T TracingFlags;
	T CsrServerReadOnlySharedMemoryBase;
};

typedef struct _CLIENT_ID
{
	PVOID UniqueProcess;
	PVOID UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _THREAD_BASIC_INFORMATION
{
	LONG                 ExitStatus;
	PVOID                   TebBaseAddress;
	CLIENT_ID               ClientId;
	KAFFINITY               AffinityMask;
	DWORD               Priority;
	DWORD               BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef enum _THREADINFOCLASS2
{
	ThreadBasicInformation,
	ThreadTimes,
	ThreadPriority,
	ThreadBasePriority,
	ThreadAffinityMask,
	ThreadImpersonationToken,
	ThreadDescriptorTableEntry,
	ThreadEnableAlignmentFaultFixup,
	ThreadEventPair_Reusable,
	ThreadQuerySetWin32StartAddress,
	ThreadZeroTlsCell,
	ThreadPerformanceCount,
	ThreadAmILastThread,
	ThreadIdealProcessor,
	ThreadPriorityBoost,
	ThreadSetTlsArrayAddress,
	_ThreadIsIoPending,
	ThreadHideFromDebugger,
	ThreadBreakOnTermination,
	MaxThreadInfoClass
}   THREADINFOCLASS2;

typedef _PEB_T<DWORD, DWORD64, 34> _PEB32;
typedef _PEB_T<DWORD64, DWORD, 30> _PEB64;

typedef _PEB_T<DWORD, DWORD64, 34> _PEB32;

typedef _TEB_T<DWORD>     _TEB32;
typedef _TEB_T<DWORD64>   _TEB64;
typedef _TEB_T<DWORD_PTR>  TEB_T;

typedef _PEB32 PEB_T;
