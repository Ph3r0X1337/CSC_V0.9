#ifndef CSC_MEMORY_UTILS
#define CSC_MEMORY_UTILS

#include "../Configuration/Configuration.h"

CSC_STATUS CSCAPI CSC_MemoryUtilsCopyMemory(_Out_ CONST CSC_PVOID pDst, _In_ CONST CSC_PCVOID pSrc, _In_ CONST CSC_SIZE_T size);
CSC_STATUS CSCAPI CSC_MemoryUtilsSetMemory(_Out_ CONST CSC_PVOID pDst, _In_ CONST CSC_BYTE value, _In_ CONST CSC_SIZE_T size);
CSC_STATUS CSCAPI CSC_MemoryUtilsSetZeroMemory(_Out_ CONST CSC_PVOID pDst, _In_ CONST CSC_SIZE_T size);
CSC_STATUS CSCAPI CSC_MemoryUtilsSetArrayValue(_Out_ CONST CSC_PVOID pDst, _In_ CONST CSC_PCVOID pValue, _In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_SIZE_T elementCount);

CSC_STATUS CSCAPI CSC_MemoryUtilsCompareMemory(_In_ CONST CSC_PCVOID pFirst, _In_ CONST CSC_PCVOID pSecond, _In_ CONST CSC_SIZE_T size);

CSC_STATUS CSCAPI CSC_MemoryUtilsSwapValues(_Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond, _In_ CONST CSC_SIZE_T size, _In_ CONST CSC_PVOID pAllocator);
CSC_STATUS CSCAPI CSC_MemoryUtilsSwapValuesHeap(_Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond, _In_ CONST CSC_SIZE_T size, _In_ CONST CSC_PVOID pAllocator);
CSC_STATUS CSCAPI CSC_MemoryUtilsSwapValues256(_Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond, _In_ CONST CSC_SIZE_T size);
CSC_STATUS CSCAPI CSC_MemoryUtilsMoveValue(_Out_ CONST CSC_PVOID pDst, _Inout_ CONST CSC_PVOID pSrc, _In_ CONST CSC_SIZE_T size);

#endif CSC_MEMORY_UTILS
