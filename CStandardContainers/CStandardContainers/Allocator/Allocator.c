#include "Allocator.h"

CSC_STATUS CSCMETHOD CSC_AllocatorInit(_Inout_ CONST CSC_PVOID pThis)
{
	CSC_PCVOID* pVTable;
	CSC_P_VFN_ALLOCATOR_INIT pVfnInitialize;

	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	pVTable = *(CSC_PCVOID**)pThis;

	if (!pVTable)
		return CSC_STATUS_INVALID_PARAMETER;

	pVfnInitialize = (CSC_P_VFN_ALLOCATOR_INIT)pVTable[csc_vfn_al_initialize];

	if (!pVfnInitialize)
		return CSC_STATUS_INVALID_PARAMETER;

	return pVfnInitialize(pThis);
}

CSC_STATUS CSCMETHOD CSC_AllocatorCleanup(_Inout_ CONST CSC_PVOID pThis)
{
	CSC_PCVOID* pVTable;
	CSC_P_VFN_ALLOCATOR_CLEANUP pVfnCleanup;

	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	pVTable = *(CSC_PCVOID**)pThis;

	if (!pVTable)
		return CSC_STATUS_INVALID_PARAMETER;

	pVfnCleanup = (CSC_P_VFN_ALLOCATOR_CLEANUP)pVTable[csc_vfn_al_cleanup];

	if (!pVfnCleanup)
		return CSC_STATUS_INVALID_PARAMETER;

	return pVfnCleanup(pThis);
}


CSC_PVOID CSCMETHOD CSC_AllocatorAlloc(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_SIZE_T size)
{
	CSC_PCVOID* pVTable;
	CSC_P_VFN_ALLOCATOR_ALLOC pVfnAllocate;

	if (!pThis || !size)
		return NULL;

	pVTable = *(CSC_PCVOID**)pThis;

	if (!pVTable)
		return NULL;

	pVfnAllocate = (CSC_P_VFN_ALLOCATOR_ALLOC)pVTable[csc_vfn_al_alloc];

	if (!pVfnAllocate)
		return NULL;

	return pVfnAllocate(pThis, size);
}

CSC_PVOID CSCMETHOD CSC_AllocatorAllocZero(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_SIZE_T size)
{
	CSC_PCVOID* pVTable;
	CSC_P_VFN_ALLOCATOR_ALLOC_ZERO pVfnAllocateZero;

	if (!pThis || !size)
		return NULL;

	pVTable = *(CSC_PCVOID**)pThis;

	if (!pVTable)
		return NULL;

	pVfnAllocateZero = (CSC_P_VFN_ALLOCATOR_ALLOC_ZERO)pVTable[csc_vfn_al_alloc_zero];

	if (!pVfnAllocateZero)
		return NULL;

	return pVfnAllocateZero(pThis, size);
}


CSC_STATUS CSCMETHOD CSC_AllocatorFree(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_PVOID pMemoryBlock)
{
	CSC_PCVOID* pVTable;
	CSC_P_VFN_ALLOCATOR_FREE pVfnDeallocate;

	if (!pThis || !pMemoryBlock)
		return CSC_STATUS_INVALID_PARAMETER;

	pVTable = *(CSC_PCVOID**)pThis;

	if (!pVTable)
		return CSC_STATUS_INVALID_PARAMETER;

	pVfnDeallocate = (CSC_P_VFN_ALLOCATOR_FREE)pVTable[csc_vfn_al_free];

	if (!pVfnDeallocate)
		return CSC_STATUS_INVALID_PARAMETER;

	return pVfnDeallocate(pThis, pMemoryBlock);
}


CSC_STATUS CSCMETHOD CSC_AllocatorIsUsable(_In_ CONST CSC_PVOID pThis)
{
	CSC_PCVOID* pVTable;
	CSC_P_VFN_ALLOCATOR_USABLE pVfnUsable;

	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	pVTable = *(CSC_PCVOID**)pThis;

	if (!pVTable)
		return CSC_STATUS_INVALID_PARAMETER;

	pVfnUsable = (CSC_P_VFN_ALLOCATOR_USABLE)pVTable[csc_vfn_al_usable];

	if (!pVfnUsable)
		return CSC_STATUS_INVALID_PARAMETER;

	return pVfnUsable(pThis);
}

