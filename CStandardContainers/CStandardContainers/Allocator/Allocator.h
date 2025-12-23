#ifndef CSC_ALLOCATOR
#define CSC_ALLOCATOR

#include "../Configuration/Configuration.h"

typedef enum _CSC_AllocatorVirtualFunction
{
	csc_vfn_al_initialize = 0,
	csc_vfn_al_cleanup,
	csc_vfn_al_alloc,
	csc_vfn_al_alloc_zero,
	csc_vfn_al_free,
	csc_vfn_al_usable,
	csc_vfn_al_num_of_entries
} CSC_AllocatorVirtualFunction;

typedef CSC_STATUS(CSCMETHOD* CSC_P_VFN_ALLOCATOR_INIT)(_Inout_ CONST CSC_PVOID pAllocator);
typedef CSC_STATUS(CSCMETHOD* CSC_P_VFN_ALLOCATOR_CLEANUP)(_Inout_ CONST CSC_PVOID pAllocator);

typedef CSC_PVOID(CSCMETHOD* CSC_P_VFN_ALLOCATOR_ALLOC)(_In_ CONST CSC_PVOID pAllocator, _In_ CONST CSC_SIZE_T size);
typedef CSC_PVOID(CSCMETHOD* CSC_P_VFN_ALLOCATOR_ALLOC_ZERO)(_In_ CONST CSC_PVOID pAllocator, _In_ CONST CSC_SIZE_T size);

typedef CSC_STATUS(CSCMETHOD* CSC_P_VFN_ALLOCATOR_FREE)(_In_ CONST CSC_PVOID pAllocator, _In_ CONST CSC_PVOID pMemoryBlock);

typedef CSC_STATUS(CSCMETHOD* CSC_P_VFN_ALLOCATOR_USABLE)(_In_ CONST CSC_PVOID pAllocator);

CSC_STATUS CSCMETHOD CSC_AllocatorInit(_Inout_ CONST CSC_PVOID pThis);
CSC_STATUS CSCMETHOD CSC_AllocatorCleanup(_Inout_ CONST CSC_PVOID pThis);

CSC_PVOID CSCMETHOD CSC_AllocatorAlloc(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_SIZE_T size);
CSC_PVOID CSCMETHOD CSC_AllocatorAllocZero(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_SIZE_T size);

CSC_STATUS CSCMETHOD CSC_AllocatorFree(_In_ CONST CSC_PVOID pThis, _In_ CONST CSC_PVOID pMemoryBlock);

CSC_STATUS CSCMETHOD CSC_AllocatorIsUsable(_In_ CONST CSC_PVOID pThis);

#endif CSC_ALLOCATOR
// Major Code-Smell in the library: Include a static (private I mean) verify function in each class so that per function parameter validation can be separated from general validation -> Done for CSC_Iterator, DynArray, LL and Strings
// Major Code-Smell: In C const keyword cannot be compile time -> compile time constants need to be replaces through macros.