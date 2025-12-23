#include "NativeAllocator.h"
#include "Allocator.h"
#include "../Utils/WinUtils.h"

#pragma comment (lib, "ntdll.lib")

static CSC_STATUS CSCMETHOD CSC_NativeAllocatorInitImpl(_In_ CONST CSC_PVOID pThis);
static CSC_STATUS CSCMETHOD CSC_NativeAllocatorCleanupImpl(_In_ CONST CSC_PVOID pThis);

static CSC_PVOID CSCMETHOD CSC_NativeAllocatorAllocImpl(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_SIZE_T size);
static CSC_PVOID CSCMETHOD CSC_NativeAllocatorAllocZeroImpl(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_SIZE_T size);

static CSC_STATUS CSCMETHOD CSC_NativeAllocatorFreeImpl(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_PVOID pMemoryBlock);

static CSC_STATUS CSCMETHOD CSC_NativeAllocatorUsableImpl(_In_ CONST CSC_PVOID pThis);

static CONST CSC_PCVOID g_nativeAllocatorVirtualTable[csc_vfn_al_num_of_entries] =
{
	(CSC_PCVOID)CSC_NativeAllocatorInitImpl,
	(CSC_PCVOID)CSC_NativeAllocatorCleanupImpl,
	(CSC_PCVOID)CSC_NativeAllocatorAllocImpl,
	(CSC_PCVOID)CSC_NativeAllocatorAllocZeroImpl,
	(CSC_PCVOID)CSC_NativeAllocatorFreeImpl,
	(CSC_PCVOID)CSC_NativeAllocatorUsableImpl
};

#define CSC_NATIVE_ALLOCATOR_MAX_REFCOUNT (CSC_DWORD)-1

static CONST CSC_PCVOID* CONST g_pVTableNativeAllocator = g_nativeAllocatorVirtualTable;
static CSC_PVOID g_heapHandle = NULL;
static CSC_DWORD g_refCount = (CSC_DWORD)0;


CSC_PVOID CSCAPI CSC_NativeAllocatorGetPointer()
{
	return (CSC_PVOID)&g_pVTableNativeAllocator;
}


CSC_STATUS CSCAPI CSC_NativeAllocatorCreate()
{
	return CSC_NativeAllocatorInitImpl((CONST CSC_PVOID)&g_pVTableNativeAllocator);
}

CSC_STATUS CSCAPI CSC_NativeAllocatorDestroy()
{
	return CSC_NativeAllocatorCleanupImpl((CONST CSC_PVOID)&g_pVTableNativeAllocator);
}


CSC_PVOID CSCAPI CSC_NativeAllocatorAlloc(_In_ CONST CSC_SIZE_T size)
{
	if (!size)
		return NULL;

	return CSC_NativeAllocatorAllocImpl((CONST CSC_PVOID)&g_pVTableNativeAllocator, size);
}

CSC_PVOID CSCAPI CSC_NativeAllocatorAllocZero(_In_ CONST CSC_SIZE_T size)
{
	if (!size)
		return NULL;

	return CSC_NativeAllocatorAllocZeroImpl((CONST CSC_PVOID)&g_pVTableNativeAllocator, size);
}


CSC_STATUS CSCAPI CSC_NativeAllocatorFree(_In_ CONST CSC_PVOID pMemoryBlock)
{
	if (!pMemoryBlock)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_NativeAllocatorFreeImpl((CONST CSC_PVOID)&g_pVTableNativeAllocator, pMemoryBlock);
}


CSC_STATUS CSCAPI CSC_NativeAllocatorIsUsable()
{
	return CSC_NativeAllocatorUsableImpl((CONST CSC_PVOID)&g_pVTableNativeAllocator);
}


static CSC_STATUS CSCMETHOD CSC_NativeAllocatorInitImpl(_In_ CONST CSC_PVOID pThis)
{
	if (g_refCount >= CSC_NATIVE_ALLOCATOR_MAX_REFCOUNT)
		return CSC_STATUS_GENERAL_FAILURE;

	if (pThis != (CSC_PVOID)&g_pVTableNativeAllocator)
		return CSC_STATUS_INVALID_PARAMETER;

	if (CSC_WinUtilsValidHandle((CONST HANDLE)g_heapHandle) != CSC_STATUS_SUCCESS)
	{
		g_heapHandle = (CSC_PVOID)RtlProcessHeap();

		if (CSC_WinUtilsValidHandle((CONST HANDLE)g_heapHandle) != CSC_STATUS_SUCCESS)
			return CSC_STATUS_INVALID_HANDLE;
	}

	++g_refCount;

	return CSC_STATUS_SUCCESS;
}

static CSC_STATUS CSCMETHOD CSC_NativeAllocatorCleanupImpl(_In_ CONST CSC_PVOID pThis)
{
	if (!g_refCount)
		return CSC_STATUS_GENERAL_FAILURE;

	if (pThis != (CSC_PVOID)&g_pVTableNativeAllocator)
		return CSC_STATUS_INVALID_PARAMETER;

	--g_refCount;

	if (!g_refCount)
		g_heapHandle = NULL;

	return CSC_STATUS_SUCCESS;
}


static CSC_PVOID CSCMETHOD CSC_NativeAllocatorAllocImpl(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_SIZE_T size)
{
	if (pThis != (CSC_PVOID)&g_pVTableNativeAllocator || !size || CSC_WinUtilsValidHandle((CONST HANDLE)g_heapHandle) != CSC_STATUS_SUCCESS)
		return NULL;

	return (CSC_PVOID)RtlAllocateHeap((PVOID)g_heapHandle, (ULONG)0, (SIZE_T)size);
}

static CSC_PVOID CSCMETHOD CSC_NativeAllocatorAllocZeroImpl(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_SIZE_T size)
{
	if (pThis != (CSC_PVOID)&g_pVTableNativeAllocator || !size || CSC_WinUtilsValidHandle((CONST HANDLE)g_heapHandle) != CSC_STATUS_SUCCESS)
		return NULL;

	return (CSC_PVOID)RtlAllocateHeap((PVOID)g_heapHandle, HEAP_ZERO_MEMORY, (SIZE_T)size);
}


static CSC_STATUS CSCMETHOD CSC_NativeAllocatorFreeImpl(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_PVOID pMemoryBlock)
{
	if (pThis != (CSC_PVOID)&g_pVTableNativeAllocator || !pMemoryBlock)
		return CSC_STATUS_INVALID_PARAMETER;

	if (CSC_WinUtilsValidHandle((CONST HANDLE)g_heapHandle) != CSC_STATUS_SUCCESS)
		return CSC_STATUS_INVALID_HANDLE;

	return (RtlFreeHeap((PVOID)g_heapHandle, (ULONG)0, (PVOID)pMemoryBlock)) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE;
}


static CSC_STATUS CSCMETHOD CSC_NativeAllocatorUsableImpl(_In_ CONST CSC_PVOID pThis)
{
	if (pThis != (CSC_PVOID)&g_pVTableNativeAllocator)
		return CSC_STATUS_INVALID_PARAMETER;

	if (CSC_WinUtilsValidHandle((CONST HANDLE)g_heapHandle) != CSC_STATUS_SUCCESS)
		return CSC_STATUS_INVALID_HANDLE;

	if (!g_refCount || g_refCount > CSC_NATIVE_ALLOCATOR_MAX_REFCOUNT)
		return CSC_STATUS_GENERAL_FAILURE;

	return CSC_STATUS_SUCCESS;
}