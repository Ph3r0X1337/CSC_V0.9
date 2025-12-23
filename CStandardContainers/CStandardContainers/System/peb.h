#ifndef PEB_H
#define PEB_H

#include "ntdll.h"

typedef DWORD64 QWORD;

#pragma pack(push)
#pragma pack(1)

/*
typedef struct _LIST_ENTRY32
{
    DWORD fLink;
    DWORD bLink;
} LIST_ENTRY32;

typedef struct _LIST_ENTRY64
{
    QWORD fLink;
    QWORD bLink;
} LIST_ENTRY64;
*/


typedef struct _UNICODE_STRING32
{
    union
    {
        struct
        {
            WORD length;
            WORD maximumLength;
        };
        DWORD dummy;
    };
    DWORD buffer;
} UNICODE_STRING32;

typedef struct _UNICODE_STRING64
{
    union
    {
        struct
        {
            WORD length;
            WORD maximumLength;
        };
        QWORD dummy;
    };
    QWORD buffer;
} UNICODE_STRING64;

typedef struct _UNICODE_STRING_WOW64 
{
    USHORT Length;
    USHORT MaximumLength;
    PVOID64 Buffer;
} UNICODE_STRING_WOW64;


typedef struct _PEB32
{
    union
    {
        struct
        {
            BYTE InheritedAddressSpace;
            BYTE ReadImageFileExecOptions;
            BYTE BeingDebugged;
            BYTE BitField;
        };
        DWORD dummy01;
    };
    DWORD Mutant;
    DWORD ImageBaseAddress;
    DWORD Ldr;
    DWORD ProcessParameters;
    DWORD SubSystemData;
    DWORD ProcessHeap;
    DWORD FastPebLock;
    DWORD AtlThunkSListPtr;
    DWORD IFEOKey;
    DWORD CrossProcessFlags;
    DWORD UserSharedInfoPtr;
    DWORD SystemReserved;
    DWORD AtlThunkSListPtr32;
    DWORD ApiSetMap;
    DWORD TlsExpansionCounter;
    DWORD TlsBitmap;
    DWORD TlsBitmapBits[2];
    DWORD ReadOnlySharedMemoryBase;
    DWORD HotpatchInformation;
    DWORD ReadOnlyStaticServerData;
    DWORD AnsiCodePageData;
    DWORD OemCodePageData;
    DWORD UnicodeCaseTableData;
    DWORD NumberOfProcessors;
    union
    {
        DWORD NtGlobalFlag;
        QWORD dummy02;
    };
    LARGE_INTEGER CriticalSectionTimeout;
    DWORD HeapSegmentReserve;
    DWORD HeapSegmentCommit;
    DWORD HeapDeCommitTotalFreeThreshold;
    DWORD HeapDeCommitFreeBlockThreshold;
    DWORD NumberOfHeaps;
    DWORD MaximumNumberOfHeaps;
    DWORD ProcessHeaps;
    DWORD GdiSharedHandleTable;
    DWORD ProcessStarterHelper;
    DWORD GdiDCAttributeList;
    DWORD LoaderLock;
    DWORD OSMajorVersion;
    DWORD OSMinorVersion;
    WORD OSBuildNumber;
    WORD OSCSDVersion;
    DWORD OSPlatformId;
    DWORD ImageSubsystem;
    DWORD ImageSubsystemMajorVersion;
    DWORD ImageSubsystemMinorVersion;
    DWORD ActiveProcessAffinityMask;
    DWORD GdiHandleBuffer[34];
    DWORD PostProcessInitRoutine;
    DWORD TlsExpansionBitmap;
    DWORD TlsExpansionBitmapBits[32];
    DWORD SessionId;
    ULARGE_INTEGER AppCompatFlags;
    ULARGE_INTEGER AppCompatFlagsUser;
    DWORD pShimData;
    DWORD AppCompatInfo;
    UNICODE_STRING32 CSDVersion;
    DWORD ActivationContextData;
    DWORD ProcessAssemblyStorageMap;
    DWORD SystemDefaultActivationContextData;
    DWORD SystemAssemblyStorageMap;
    DWORD MinimumStackCommit;
    DWORD FlsCallback;
    LIST_ENTRY32 FlsListHead;
    DWORD FlsBitmap;
    DWORD FlsBitmapBits[4];
    DWORD FlsHighIndex;
    DWORD WerRegistrationData;
    DWORD WerShipAssertPtr;
    DWORD pContextData;
    DWORD pImageHeaderHash;
    DWORD TracingFlags;
} PEB32;

typedef struct _PEB64
{
    union
    {
        struct
        {
            BYTE InheritedAddressSpace;
            BYTE ReadImageFileExecOptions;
            BYTE BeingDebugged;
            BYTE BitField;
        };
        QWORD dummy01;
    };
    QWORD Mutant;
    QWORD ImageBaseAddress;
    QWORD Ldr;
    QWORD ProcessParameters;
    QWORD SubSystemData;
    QWORD ProcessHeap;
    QWORD FastPebLock;
    QWORD AtlThunkSListPtr;
    QWORD IFEOKey;
    QWORD CrossProcessFlags;
    QWORD UserSharedInfoPtr;
    DWORD SystemReserved;
    DWORD AtlThunkSListPtr32;
    QWORD ApiSetMap;
    QWORD TlsExpansionCounter;
    QWORD TlsBitmap;
    DWORD TlsBitmapBits[2];
    QWORD ReadOnlySharedMemoryBase;
    QWORD HotpatchInformation;
    QWORD ReadOnlyStaticServerData;
    QWORD AnsiCodePageData;
    QWORD OemCodePageData;
    QWORD UnicodeCaseTableData;
    DWORD NumberOfProcessors;
    DWORD NtGlobalFlag;
    LARGE_INTEGER CriticalSectionTimeout;
    DWORD HeapSegmentReserve;
    DWORD HeapSegmentCommit;
    DWORD HeapDeCommitTotalFreeThreshold;
    DWORD HeapDeCommitFreeBlockThreshold;
    DWORD NumberOfHeaps;
    DWORD MaximumNumberOfHeaps;
    QWORD ProcessHeaps;
    QWORD GdiSharedHandleTable;
    QWORD ProcessStarterHelper;
    QWORD GdiDCAttributeList;
    QWORD LoaderLock;
    DWORD OSMajorVersion;
    DWORD OSMinorVersion;
    WORD OSBuildNumber;
    WORD OSCSDVersion;
    DWORD OSPlatformId;
    DWORD ImageSubsystem;
    DWORD ImageSubsystemMajorVersion;
    QWORD ImageSubsystemMinorVersion;
    QWORD ActiveProcessAffinityMask;
    QWORD GdiHandleBuffer[30];
    QWORD PostProcessInitRoutine;
    QWORD TlsExpansionBitmap;
    DWORD TlsExpansionBitmapBits[32];
    QWORD SessionId;
    ULARGE_INTEGER AppCompatFlags;
    ULARGE_INTEGER AppCompatFlagsUser;
    QWORD pShimData;
    QWORD AppCompatInfo;
    UNICODE_STRING64 CSDVersion;
    QWORD ActivationContextData;
    QWORD ProcessAssemblyStorageMap;
    QWORD SystemDefaultActivationContextData;
    QWORD SystemAssemblyStorageMap;
    QWORD MinimumStackCommit;
    QWORD FlsCallback;
    LIST_ENTRY64 FlsListHead;
    QWORD FlsBitmap;
    DWORD FlsBitmapBits[4];
    QWORD FlsHighIndex;
    QWORD WerRegistrationData;
    QWORD WerShipAssertPtr;
    QWORD pContextData;
    QWORD pImageHeaderHash;
    QWORD TracingFlags;
} PEB64;

#pragma pack(pop)


typedef struct _PEB_LDR_DATA32
{
    ULONG length;
    BOOLEAN initialized;
    DWORD ssHandle;
    LIST_ENTRY32 inLoadOrderModuleList;
    LIST_ENTRY32 inMemoryOrderModuleList;
    LIST_ENTRY32 inInitializationOrderModuleList;
    DWORD entryInProgress;
    BOOLEAN shutdownInProgress;
    DWORD shutdownThreadId;
} PEB_LDR_DATA32;

typedef struct _PEB_LDR_DATA64
{
    ULONG length;
    BOOLEAN initialized;
    QWORD ssHandle;
    LIST_ENTRY64 inLoadOrderModuleList;
    LIST_ENTRY64 inMemoryOrderModuleList;
    LIST_ENTRY64 inInitializationOrderModuleList;
    QWORD entryInProgress;
    BOOLEAN shutdownInProgress;
    QWORD shutdownThreadId;
} PEB_LDR_DATA64;


typedef struct _RTL_BALANCED_NODE32
{
    union
    {
        DWORD pChildren[2];
        struct
        {
            DWORD left;
            DWORD right;
        };
    };
    union
    {
        DWORD red : 1;
        DWORD balance : 2;
        DWORD parentValue;
    };
} RTL_BALANCED_NODE32;

typedef struct _RTL_BALANCED_NODE64
{
    union
    {
        QWORD pChildren[2];
        struct
        {
            QWORD left;
            QWORD right;
        };
    };
    union
    {
        QWORD red : 1;
        QWORD balance : 2;
        QWORD parentValue;
    };
} RTL_BALANCED_NODE64;


//LDR_DATA_TABLE_ENTRY structs are not very accurate in the end (after tlsIndex), but idfc

typedef struct _LDR_DATA_TABLE_ENTRY32
{
    LIST_ENTRY32 inLoadOrderLinks;
    LIST_ENTRY32 inMemoryOrderLinks;
    LIST_ENTRY32 inInitializationOrderLinks;
    DWORD dllBase;
    DWORD entryPoint;
    ULONG sizeOfImage;
    UNICODE_STRING32 fullDllName;
    UNICODE_STRING32 baseDllName;
    union
    {
        BYTE flagGroup[4];
        ULONG flags;
        struct
        {
            ULONG packagedBinary : 1;
            ULONG markedForRemoval : 1;
            ULONG imageDll : 1;
            ULONG loadNotificationsSent : 1;
            ULONG telemetryEntryProcessed : 1;
            ULONG processStaticImport : 1;
            ULONG inLegacyLists : 1;
            ULONG inIndexes : 1;
            ULONG shimDll : 1;
            ULONG inExceptionTable : 1;
            ULONG reservedFlags1 : 2;
            ULONG loadInProgress : 1;
            ULONG loadConfigProcessed : 1;
            ULONG entryProcessed : 1;
            ULONG protectDelayLoad : 1;
            ULONG reservedFlags3 : 2;
            ULONG dontCallForThreads : 1;
            ULONG processAttachCalled : 1;
            ULONG processAttachFailed : 1;
            ULONG corDeferredValidate : 1;
            ULONG corImage : 1;
            ULONG dontRelocate : 1;
            ULONG corILOnly : 1;
            ULONG reservedFlags5 : 3;
            ULONG redirected : 1;
            ULONG reservedFlags6 : 2;
            ULONG compatDatabaseProcessed : 1;
        };
    };
    USHORT obsoleteLoadCount;
    USHORT tlsIndex;
    LIST_ENTRY32 hashLinks;
    ULONG timeDateStamp;
    LIST_ENTRY32 nodeModuleLink;
    RTL_BALANCED_NODE32 baseAddressIndexNode;
    RTL_BALANCED_NODE32 mappingInfoIndexNode;
    LARGE_INTEGER loadTime;
    ULONG baseNameHashValue;
    LDR_DLL_LOAD_REASON loadReason;
    ULONG implicitPathOptions;
    ULONG referenceCount;
} LDR_DATA_TABLE_ENTRY32;

typedef struct _LDR_DATA_TABLE_ENTRY64
{
    LIST_ENTRY64 inLoadOrderLinks;
    LIST_ENTRY64 inMemoryOrderLinks;
    LIST_ENTRY64 inInitializationOrderLinks;
    QWORD dllBase;
    QWORD entryPoint;
    ULONG sizeOfImage;
    UNICODE_STRING64 fullDllName;
    UNICODE_STRING64 baseDllName;
    union
    {
        BYTE flagGroup[4];
        ULONG flags;
        struct
        {
            ULONG packagedBinary : 1;
            ULONG markedForRemoval : 1;
            ULONG imageDll : 1;
            ULONG loadNotificationsSent : 1;
            ULONG telemetryEntryProcessed : 1;
            ULONG processStaticImport : 1;
            ULONG inLegacyLists : 1;
            ULONG inIndexes : 1;
            ULONG shimDll : 1;
            ULONG inExceptionTable : 1;
            ULONG reservedFlags1 : 2;
            ULONG loadInProgress : 1;
            ULONG loadConfigProcessed : 1;
            ULONG entryProcessed : 1;
            ULONG protectDelayLoad : 1;
            ULONG reservedFlags3 : 2;
            ULONG dontCallForThreads : 1;
            ULONG processAttachCalled : 1;
            ULONG processAttachFailed : 1;
            ULONG corDeferredValidate : 1;
            ULONG corImage : 1;
            ULONG dontRelocate : 1;
            ULONG corILOnly : 1;
            ULONG reservedFlags5 : 3;
            ULONG redirected : 1;
            ULONG reservedFlags6 : 2;
            ULONG compatDatabaseProcessed : 1;
        };
    };
    USHORT obsoleteLoadCount;
    USHORT tlsIndex;
    LIST_ENTRY64 hashLinks;
    ULONG timeDateStamp;
    LIST_ENTRY64 nodeModuleLink;
    RTL_BALANCED_NODE64 baseAddressIndexNode;
    RTL_BALANCED_NODE64 mappingInfoIndexNode;
    LARGE_INTEGER loadTime;
    ULONG baseNameHashValue;
    LDR_DLL_LOAD_REASON loadReason;
    ULONG implicitPathOptions;
    ULONG referenceCount;
} LDR_DATA_TABLE_ENTRY64;


typedef struct _PROCESS_BASIC_INFORMATION32
{
    NTSTATUS exitStatus;
    DWORD pebBaseAddress;
    DWORD affinityMask;
    KPRIORITY basePriority;
    DWORD uniqueProcessID;
    DWORD inheritedFromUniqueProcessID;
} PROCESS_BASIC_INFORMATION32;

typedef struct _PROCESS_BASIC_INFORMATION64
{
    NTSTATUS exitStatus;
    QWORD pebBaseAddress;
    QWORD affinityMask;
    KPRIORITY basePriority;
    QWORD uniqueProcessID;
    QWORD inheritedFromUniqueProcessID;
} PROCESS_BASIC_INFORMATION64;

typedef struct _PROCESS_BASIC_INFORMATION_WOW64
{
    NTSTATUS ExitStatus;
    QWORD  PEB_BaseAddress;
    QWORD  AffinityMask;
    KPRIORITY BasePriority;
    QWORD  UniqueProcessID;
    QWORD  InheritedFromUniqueProcessID;
} PROCESS_BASIC_INFORMATION_WOW64;

#endif PEB_H