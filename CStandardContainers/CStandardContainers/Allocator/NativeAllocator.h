#ifndef CSC_NATIVE_ALLOCATOR
#define CSC_NATIVE_ALLOCATOR

#include "../Configuration/Configuration.h"

CSC_PVOID CSCAPI CSC_NativeAllocatorGetPointer();

CSC_STATUS CSCAPI CSC_NativeAllocatorCreate();
CSC_STATUS CSCAPI CSC_NativeAllocatorDestroy();

CSC_PVOID CSCAPI CSC_NativeAllocatorAlloc(_In_ CONST CSC_SIZE_T size);
CSC_PVOID CSCAPI CSC_NativeAllocatorAllocZero(_In_ CONST CSC_SIZE_T size);

CSC_STATUS CSCAPI CSC_NativeAllocatorFree(_In_ CONST CSC_PVOID pMemoryBlock);

CSC_STATUS CSCAPI CSC_NativeAllocatorIsUsable();

#endif CSC_NATIVE_ALLOCATOR
