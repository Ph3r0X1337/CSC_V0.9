#include "PrivateHeapNativeAllocator.h"
#include "Allocator.h"
#include "../Utils/WinUtils.h"

static CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorInitImpl(_Inout_ CONST CSC_PVOID pThis);
static CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorCleanupImpl(_Inout_ CONST CSC_PVOID pThis);

static CSC_PVOID CSCMETHOD CSC_PrivateHeapNativeAllocatorAllocImpl(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_SIZE_T size);
static CSC_PVOID CSCMETHOD CSC_PrivateHeapNativeAllocatorAllocZeroImpl(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_SIZE_T size);

static CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorFreeImpl(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_PVOID pMemoryBlock);

static CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorUsableImpl(_In_ CONST CSC_PVOID pThis);

static CONST CSC_PCVOID g_privateHeapNativeAllocatorVirtualTable[csc_vfn_al_num_of_entries] =
{
	(CSC_PCVOID)CSC_PrivateHeapNativeAllocatorInitImpl,
	(CSC_PCVOID)CSC_PrivateHeapNativeAllocatorCleanupImpl,
	(CSC_PCVOID)CSC_PrivateHeapNativeAllocatorAllocImpl,
	(CSC_PCVOID)CSC_PrivateHeapNativeAllocatorAllocZeroImpl,
	(CSC_PCVOID)CSC_PrivateHeapNativeAllocatorFreeImpl,
	(CSC_PCVOID)CSC_PrivateHeapNativeAllocatorUsableImpl
};


CSC_PrivateHeapNativeAllocator CSCAPI CSC_PrivateHeapNativeAllocatorGet()
{
	CSC_PrivateHeapNativeAllocator result;

	result.heapHandle = NULL;
	result.pVTableAllocator = (CSC_PCVOID)g_privateHeapNativeAllocatorVirtualTable;

	return result;
}


CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorCreate(_Inout_ CSC_PrivateHeapNativeAllocator* CONST pThis)
{
	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_PrivateHeapNativeAllocatorInitImpl((CONST CSC_PVOID)pThis);
}

CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorDestroy(_Inout_ CSC_PrivateHeapNativeAllocator* CONST pThis)
{
	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_PrivateHeapNativeAllocatorCleanupImpl((CONST CSC_PVOID)pThis);
}


CSC_PVOID CSCMETHOD CSC_PrivateHeapNativeAllocatorAlloc(_In_ CONST CSC_PrivateHeapNativeAllocator* CONST pThis, _In_ CONST CSC_SIZE_T size)
{
	if (!pThis)
		return NULL;

	return CSC_PrivateHeapNativeAllocatorAllocImpl((CONST CSC_PVOID)pThis, size);
}

CSC_PVOID CSCMETHOD CSC_PrivateHeapNativeAllocatorAllocZero(_In_ CONST CSC_PrivateHeapNativeAllocator* CONST pThis, _In_ CONST CSC_SIZE_T size)
{
	if (!pThis)
		return NULL;

	return CSC_PrivateHeapNativeAllocatorAllocZeroImpl((CONST CSC_PVOID)pThis, size);
}


CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorFree(_In_ CONST CSC_PrivateHeapNativeAllocator* CONST pThis, _In_ CONST CSC_PVOID pMemoryBlock)
{
	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_PrivateHeapNativeAllocatorFreeImpl((CONST CSC_PVOID)pThis, pMemoryBlock);
}


CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorIsUsable(_In_ CONST CSC_PrivateHeapNativeAllocator* CONST pThis)
{
	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_PrivateHeapNativeAllocatorUsableImpl((CONST CSC_PVOID)pThis);
}


static CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorInitImpl(_Inout_ CONST CSC_PVOID pThis)
{
	CSC_PVOID heapHandle;
	CSC_PrivateHeapNativeAllocator* CONST pPHNAllocator = (CSC_PrivateHeapNativeAllocator* CONST)pThis;

	if (!pThis || pPHNAllocator->pVTableAllocator != &g_privateHeapNativeAllocatorVirtualTable)
		return CSC_STATUS_INVALID_PARAMETER;

	heapHandle = (CSC_PVOID)RtlCreateHeap(HEAP_GROWABLE, NULL, (SIZE_T)0, (SIZE_T)0, NULL, NULL);

	if (CSC_WinUtilsValidHandle((CONST HANDLE)heapHandle) != CSC_STATUS_SUCCESS)
		return CSC_STATUS_INVALID_HANDLE;

	pPHNAllocator->heapHandle = heapHandle;

	return CSC_STATUS_SUCCESS;
}

static CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorCleanupImpl(_Inout_ CONST CSC_PVOID pThis)
{
	CSC_PrivateHeapNativeAllocator* CONST pPHNAllocator = (CSC_PrivateHeapNativeAllocator* CONST)pThis;
	CSC_PVOID* pHeapHandle;

	if (!pThis || pPHNAllocator->pVTableAllocator != &g_privateHeapNativeAllocatorVirtualTable)
		return CSC_STATUS_INVALID_PARAMETER;

	pHeapHandle = &pPHNAllocator->heapHandle;

	if (CSC_WinUtilsValidHandle((CONST HANDLE)*pHeapHandle) != CSC_STATUS_SUCCESS)
		return CSC_STATUS_INVALID_HANDLE;

	if (RtlDestroyHeap((PVOID)*pHeapHandle))
		return CSC_STATUS_GENERAL_FAILURE;

	*pHeapHandle = NULL;

	return CSC_STATUS_SUCCESS;
}


static CSC_PVOID CSCMETHOD CSC_PrivateHeapNativeAllocatorAllocImpl(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_SIZE_T size)
{
	CSC_PrivateHeapNativeAllocator* CONST pPHNAllocator = (CSC_PrivateHeapNativeAllocator* CONST)pThis;

	if (!pThis || pPHNAllocator->pVTableAllocator != &g_privateHeapNativeAllocatorVirtualTable || !size)
		return NULL;

	if (CSC_WinUtilsValidHandle((CONST HANDLE)pPHNAllocator->heapHandle) != CSC_STATUS_SUCCESS)
		return NULL;

	return (CSC_PVOID)RtlAllocateHeap((PVOID)pPHNAllocator->heapHandle, (ULONG)0, (SIZE_T)size);
}

static CSC_PVOID CSCMETHOD CSC_PrivateHeapNativeAllocatorAllocZeroImpl(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_SIZE_T size)
{
	CSC_PrivateHeapNativeAllocator* CONST pPHNAllocator = (CSC_PrivateHeapNativeAllocator* CONST)pThis;

	if (!pThis || pPHNAllocator->pVTableAllocator != &g_privateHeapNativeAllocatorVirtualTable || !size)
		return NULL;

	if (CSC_WinUtilsValidHandle((CONST HANDLE)pPHNAllocator->heapHandle) != CSC_STATUS_SUCCESS)
		return NULL;

	return (CSC_PVOID)RtlAllocateHeap((PVOID)pPHNAllocator->heapHandle, HEAP_ZERO_MEMORY, (SIZE_T)size);
}


static CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorFreeImpl(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_PVOID pMemoryBlock)
{
	CSC_PrivateHeapNativeAllocator* CONST pPHNAllocator = (CSC_PrivateHeapNativeAllocator* CONST)pThis;

	if (!pThis || pPHNAllocator->pVTableAllocator != &g_privateHeapNativeAllocatorVirtualTable || !pMemoryBlock)
		return CSC_STATUS_INVALID_PARAMETER;

	if (CSC_WinUtilsValidHandle((CONST HANDLE)pPHNAllocator->heapHandle) != CSC_STATUS_SUCCESS)
		return CSC_STATUS_INVALID_HANDLE;

	return (RtlFreeHeap((PVOID)pPHNAllocator->heapHandle, (ULONG)0, (PVOID)pMemoryBlock)) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE;
}


static CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorUsableImpl(_In_ CONST CSC_PVOID pThis)
{
	CSC_PrivateHeapNativeAllocator* CONST pPHNAllocator = (CSC_PrivateHeapNativeAllocator* CONST)pThis;

	if (!pThis || pPHNAllocator->pVTableAllocator != &g_privateHeapNativeAllocatorVirtualTable)
		return CSC_STATUS_INVALID_PARAMETER;

	if (CSC_WinUtilsValidHandle((CONST HANDLE)pPHNAllocator->heapHandle) != CSC_STATUS_SUCCESS)
		return CSC_STATUS_INVALID_HANDLE;

	return CSC_STATUS_SUCCESS;
}

