#ifndef CSC_PRIVATE_HEAP_NATIVE_ALLOCATOR
#define CSC_PRIVATE_HEAP_NATIVE_ALLOCATOR

#include "../Configuration/Configuration.h"

typedef struct _CSC_PrivateHeapNativeAllocator
{
	CSC_PCVOID pVTableAllocator;
	CSC_PVOID heapHandle;
} CSC_PrivateHeapNativeAllocator;

CSC_PrivateHeapNativeAllocator CSCAPI CSC_PrivateHeapNativeAllocatorGet();

CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorCreate(_Inout_ CSC_PrivateHeapNativeAllocator* CONST pThis);
CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorDestroy(_Inout_ CSC_PrivateHeapNativeAllocator* CONST pThis);

CSC_PVOID CSCMETHOD CSC_PrivateHeapNativeAllocatorAlloc(_In_ CONST CSC_PrivateHeapNativeAllocator* CONST pThis, _In_ CONST CSC_SIZE_T size);
CSC_PVOID CSCMETHOD CSC_PrivateHeapNativeAllocatorAllocZero(_In_ CONST CSC_PrivateHeapNativeAllocator* CONST pThis, _In_ CONST CSC_SIZE_T size);

CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorFree(_In_ CONST CSC_PrivateHeapNativeAllocator* CONST pThis, _In_ CONST CSC_PVOID pMemoryBlock);

CSC_STATUS CSCMETHOD CSC_PrivateHeapNativeAllocatorIsUsable(_In_ CONST CSC_PrivateHeapNativeAllocator* CONST pThis);

#endif CSC_PRIVATE_HEAP_NATIVE_ALLOCATOR