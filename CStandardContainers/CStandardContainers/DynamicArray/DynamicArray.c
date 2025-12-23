#include "DynamicArray.h"
#include "../Allocator/Allocator.h"
#include "../Utils/MemoryUtils.h"

static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorRegister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorUnregister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorFirstElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorNextElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorLastElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorPreviousElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorMoveToIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index);
static CSC_SIZE_T CSCMETHOD CSC_DynamicArrayIteratorGetCurrentElementIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_PVOID CSCMETHOD CSC_DynamicArrayIteratorGetCurrentElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_PVOID CSCMETHOD CSC_DynamicArrayIteratorGetElementAt(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index);
static CSC_SIZE_T CSCMETHOD CSC_DynamicArrayIteratorGetElementCount(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorSwapElementsByIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T secondIndex);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorSwapElementsByPtr(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond);

static CONST CSC_PCVOID g_dynamicArrayVirtualTable[csc_vfn_it_num_of_entries] =
{
	(CSC_PCVOID)CSC_DynamicArrayIteratorRegister,
	(CSC_PCVOID)CSC_DynamicArrayIteratorUnregister,
	(CSC_PCVOID)CSC_DynamicArrayIteratorFirstElement,
	(CSC_PCVOID)CSC_DynamicArrayIteratorNextElement,
	(CSC_PCVOID)CSC_DynamicArrayIteratorLastElement,
	(CSC_PCVOID)CSC_DynamicArrayIteratorPreviousElement,
	(CSC_PCVOID)CSC_DynamicArrayIteratorMoveToIndex,
	(CSC_PCVOID)CSC_DynamicArrayIteratorGetCurrentElementIndex,
	(CSC_PCVOID)CSC_DynamicArrayIteratorGetCurrentElement,
	(CSC_PCVOID)CSC_DynamicArrayIteratorGetElementAt,
	(CSC_PCVOID)CSC_DynamicArrayIteratorGetElementCount,
	(CSC_PCVOID)CSC_DynamicArrayIteratorSwapElementsByIndex,
	(CSC_PCVOID)CSC_DynamicArrayIteratorSwapElementsByPtr
};

#define CSC_DYNAMIC_ARRAY_MINIMUM_SPACE (CSC_SIZE_T)0x10
#define CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE (CSC_SIZE_T)0x40000000
#define CSC_DYNAMIC_ARRAY_LINEAR_ALLOCATION_THRESHOLD (CSC_SIZE_T)0x10000
#define CSC_DYNAMIC_ARRAY_CALCULATION_ERROR (CSC_SIZE_T)-1


static CSC_STATUS CSCMETHOD CSC_DynamicArrayInitializeWithSizeImpl(_Out_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PVOID pAllocator);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayInitializeWithValueImpl(_Out_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PCVOID pValue, _In_ CONST CSC_PVOID pAllocator);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayInitializeWithCopyImpl(_Out_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_DynamicArray* CONST pSrc);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayInitializeWithArrayImpl(_Out_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_PCVOID pFirst, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_PVOID pAllocator);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayResizeImpl(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_BOOLEAN reserve, _In_ CONST CSC_BOOLEAN shrink);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayAssignBlockImpl(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PCVOID pValue);
static CSC_STATUS CSCMETHOD CSC_DynamicArrayPopFrontImpl(_Inout_ CSC_DynamicArray* CONST pThis, _When_(return == CSC_STATUS_SUCCESS, _Out_opt_) CONST CSC_PVOID pValue, _In_ CONST CSC_BOOLEAN shrink);


static CSC_SIZE_T CSCAPI CSC_DynamicArrayCalculateAllocReserve(_In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_SIZE_T numOfElements)
{
	CONST CSC_SIZE_T requestedSize = elementSize * numOfElements;
	CSC_SIZE_T calculatedSize = (CSC_DYNAMIC_ARRAY_MINIMUM_SPACE >> (CSC_SIZE_T)1);

	if (!numOfElements || !elementSize)
		return CSC_DYNAMIC_ARRAY_CALCULATION_ERROR;

	if (numOfElements > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE / elementSize || elementSize > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE)
		return CSC_DYNAMIC_ARRAY_CALCULATION_ERROR;

	if (requestedSize >= CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE)
		return CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE;

	if (requestedSize < (CSC_DYNAMIC_ARRAY_MINIMUM_SPACE >> (CSC_SIZE_T)1))
		return CSC_DYNAMIC_ARRAY_MINIMUM_SPACE;

	if (requestedSize >= CSC_DYNAMIC_ARRAY_LINEAR_ALLOCATION_THRESHOLD)
	{
		return (requestedSize & (~((CSC_SIZE_T)0xFFFF))) + ((CSC_SIZE_T)2 * CSC_DYNAMIC_ARRAY_LINEAR_ALLOCATION_THRESHOLD);
	}
	else
	{
		while (calculatedSize < requestedSize)
			calculatedSize = calculatedSize << (CSC_SIZE_T)1;

		return calculatedSize << (CSC_SIZE_T)1;
	}
}

static CSC_SIZE_T CSCAPI CSC_DynamicArrayCalculateRequestResize(_In_ CONST CSC_DynamicArray* CONST pArray, _In_ CONST CSC_SIZE_T numOfReserveElements, _In_ CONST CSC_BOOLEAN reserve)
{
	CSC_SIZE_T requestedSize, calculatedReserve;

	if (CSC_DynamicArrayIsValid(pArray) != CSC_STATUS_SUCCESS || numOfReserveElements > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE / pArray->elementSize)
		return CSC_DYNAMIC_ARRAY_CALCULATION_ERROR;

	if ((reserve) ? (numOfReserveElements <= pArray->elementCount) : (numOfReserveElements == pArray->elementCount))
		return pArray->reservedSpace;

	requestedSize = pArray->elementSize * numOfReserveElements;

	if (requestedSize >= CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE)
		return CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE;

	if (requestedSize < (CSC_DYNAMIC_ARRAY_MINIMUM_SPACE >> (CSC_SIZE_T)1))
		return CSC_DYNAMIC_ARRAY_MINIMUM_SPACE;

	if (requestedSize >= CSC_DYNAMIC_ARRAY_LINEAR_ALLOCATION_THRESHOLD)
	{
		return (requestedSize & (~((CSC_SIZE_T)0xFFFF))) + CSC_DYNAMIC_ARRAY_LINEAR_ALLOCATION_THRESHOLD;
	}
	else
	{
		calculatedReserve = CSC_DYNAMIC_ARRAY_MINIMUM_SPACE;

		while (calculatedReserve < requestedSize)
			calculatedReserve = calculatedReserve << (CSC_SIZE_T)1;

		return calculatedReserve;
	}
}

static CSC_SIZE_T CSCAPI CSC_DynamicArrayCalculatePushSize(_In_ CONST CSC_DynamicArray* CONST pArray)
{
	CSC_SIZE_T requestedSize;

	if (CSC_DynamicArrayIsValid(pArray) != CSC_STATUS_SUCCESS)
		return CSC_DYNAMIC_ARRAY_CALCULATION_ERROR;

	requestedSize = pArray->elementSize * (pArray->elementCount + (CSC_SIZE_T)1);

	if (requestedSize <= pArray->reservedSpace)
		return pArray->reservedSpace;

	return CSC_DynamicArrayCalculateRequestResize(pArray, pArray->elementCount + (CSC_SIZE_T)1, (CSC_BOOLEAN)TRUE);
}

static CSC_SIZE_T CSCAPI CSC_DynamicArrayCalculatePopSize(_In_ CONST CSC_DynamicArray* CONST pArray)
{
	CSC_SIZE_T requestedSize;

	if (CSC_DynamicArrayIsValid(pArray) != CSC_STATUS_SUCCESS || !pArray->elementCount || !pArray->reservedSpace || !pArray->pData)
		return CSC_DYNAMIC_ARRAY_CALCULATION_ERROR;

	requestedSize = pArray->elementSize * (pArray->elementCount - (CSC_SIZE_T)1);

	if (requestedSize >= pArray->reservedSpace)
		return CSC_DYNAMIC_ARRAY_CALCULATION_ERROR;

	if (!requestedSize)
		return requestedSize;

	if (requestedSize >= CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE)
		return CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE;

	if (requestedSize < (CSC_DYNAMIC_ARRAY_MINIMUM_SPACE >> (CSC_SIZE_T)2))
		return CSC_DYNAMIC_ARRAY_MINIMUM_SPACE;

	if (pArray->reservedSpace > (CSC_DYNAMIC_ARRAY_LINEAR_ALLOCATION_THRESHOLD << (CSC_SIZE_T)1))
	{
		if (pArray->reservedSpace - requestedSize > CSC_DYNAMIC_ARRAY_LINEAR_ALLOCATION_THRESHOLD)
			return pArray->reservedSpace - CSC_DYNAMIC_ARRAY_LINEAR_ALLOCATION_THRESHOLD;
		else
			return pArray->reservedSpace;
	}
	else
	{
		if (requestedSize < (pArray->reservedSpace >> (CSC_SIZE_T)2))
			return pArray->reservedSpace >> (CSC_SIZE_T)1;
		else
			return pArray->reservedSpace;
	}
}


CSC_STATUS CSCMETHOD CSC_DynamicArrayInitialize(_Out_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_PVOID pAllocator)
{
	CSC_STATUS status = CSC_DynamicArrayZeroMemory(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!elementSize || !pAllocator || elementSize > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_AllocatorIsUsable(pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pThis->pVTableIterator = (CSC_PCVOID)g_dynamicArrayVirtualTable;
	pThis->pAllocator = pAllocator;
	pThis->elementSize = elementSize;

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayInitializeWithSize(_Out_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PVOID pAllocator)
{
	CSC_DynamicArray arrayBuffer;
	CSC_STATUS status = CSC_DynamicArrayZeroMemory(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (numOfElements > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE / elementSize || elementSize > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayInitializeWithSizeImpl(&arrayBuffer, elementSize, numOfElements, pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_MemoryUtilsCopyMemory((CSC_PVOID)pThis, (CSC_PCVOID)&arrayBuffer, sizeof(arrayBuffer));

	if (status != CSC_STATUS_SUCCESS && arrayBuffer.pData)
		CSC_DynamicArrayDestroy(&arrayBuffer);

	return status;
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayInitializeWithSizeImpl(_Out_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PVOID pAllocator)
{
	CSC_SIZE_T allocationReserve;
	CSC_STATUS status;

	if (numOfElements > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE / elementSize || elementSize > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayInitialize(pThis, elementSize, pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!numOfElements)
		return CSC_STATUS_SUCCESS;

	allocationReserve = CSC_DynamicArrayCalculateAllocReserve(elementSize, numOfElements);

	if (allocationReserve == CSC_DYNAMIC_ARRAY_CALCULATION_ERROR)
		return CSC_STATUS_GENERAL_FAILURE;

	pThis->pData = CSC_AllocatorAllocZero(pThis->pAllocator, allocationReserve);

	if (!pThis->pData)
	{
		CSC_DynamicArrayDestroy(pThis);
		return CSC_STATUS_MEMORY_NOT_ALLOCATED;
	}

	pThis->elementSize = elementSize;
	pThis->elementCount = numOfElements;
	pThis->reservedSpace = allocationReserve;

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayInitializeWithValue(_Out_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PCVOID pValue, _In_ CONST CSC_PVOID pAllocator)
{
	CSC_DynamicArray arrayBuffer;
	CSC_STATUS status = CSC_DynamicArrayZeroMemory(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!numOfElements || numOfElements > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE / elementSize || elementSize > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayInitializeWithValueImpl(&arrayBuffer, elementSize, numOfElements, pValue, pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_MemoryUtilsCopyMemory((CSC_PVOID)pThis, (CSC_PCVOID)&arrayBuffer, sizeof(arrayBuffer));

	if (status != CSC_STATUS_SUCCESS)
		CSC_DynamicArrayDestroy(&arrayBuffer);

	return status;
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayInitializeWithValueImpl(_Out_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PCVOID pValue, _In_ CONST CSC_PVOID pAllocator)
{
	CSC_STATUS status;

	if (!numOfElements || numOfElements > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE / elementSize || elementSize > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE)
		return CSC_STATUS_INVALID_PARAMETER;
	
	status = CSC_DynamicArrayInitializeWithSizeImpl(pThis, elementSize, numOfElements, pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayAssignBlockImpl(pThis, (CSC_SIZE_T)0, numOfElements, pValue);

	if (status != CSC_STATUS_SUCCESS)
		CSC_DynamicArrayDestroy(pThis);

	return status;
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayInitializeWithCopy(_Out_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_DynamicArray* CONST pSrc)
{
	CSC_DynamicArray arrayBuffer;
	CSC_STATUS status = CSC_DynamicArrayZeroMemory(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayInitializeWithCopyImpl(&arrayBuffer, pSrc);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_MemoryUtilsCopyMemory((CSC_PVOID)pThis, (CSC_PCVOID)&arrayBuffer, sizeof(arrayBuffer));

	if (status != CSC_STATUS_SUCCESS)
		CSC_DynamicArrayDestroy(&arrayBuffer);

	return status;
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayInitializeWithCopyImpl(_Out_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_DynamicArray* CONST pSrc)
{
	CSC_STATUS status = CSC_DynamicArrayZeroMemory(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayIsValid(pSrc);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayInitializeWithSizeImpl(pThis, pSrc->elementSize, pSrc->elementCount, pSrc->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pSrc->elementCount)
	{
		status = CSC_DynamicArrayCopy(pThis, pSrc);

		if (status != CSC_STATUS_SUCCESS)
			CSC_DynamicArrayDestroy(pThis);
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayInitializeWithArray(_Out_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_PCVOID pFirst, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_PVOID pAllocator)
{
	CSC_DynamicArray arrayBuffer;
	CSC_STATUS status = CSC_DynamicArrayZeroMemory(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayInitializeWithArrayImpl(&arrayBuffer, pFirst, numOfElements, elementSize, pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_MemoryUtilsCopyMemory((CSC_PVOID)pThis, (CSC_PCVOID)&arrayBuffer, sizeof(arrayBuffer));

	if (status != CSC_STATUS_SUCCESS)
		CSC_DynamicArrayDestroy(&arrayBuffer);

	return status;
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayInitializeWithArrayImpl(_Out_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_PCVOID pFirst, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_PVOID pAllocator)
{
	CSC_STATUS status;

	if (!pFirst || !numOfElements || numOfElements > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE / elementSize || elementSize > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayInitializeWithSizeImpl(pThis, elementSize, numOfElements, pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayCopyArray(pThis, pFirst, numOfElements, elementSize);

	if (status != CSC_STATUS_SUCCESS)
		CSC_DynamicArrayDestroy(pThis);

	return status;
}


CSC_STATUS CSCMETHOD CSC_DynamicArrayResize(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements)
{
	return CSC_DynamicArrayResizeImpl(pThis, numOfElements, (CSC_BOOLEAN)FALSE, (CSC_BOOLEAN)TRUE);
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayLazyResize(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements)
{
	return CSC_DynamicArrayResizeImpl(pThis, numOfElements, (CSC_BOOLEAN)FALSE, (CSC_BOOLEAN)FALSE);
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayReserve(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements)
{
	return CSC_DynamicArrayResizeImpl(pThis, numOfElements, (CSC_BOOLEAN)TRUE, (CSC_BOOLEAN)FALSE);
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayShrinkToFit(_Inout_ CSC_DynamicArray* CONST pThis)
{
	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_DynamicArrayResizeImpl(pThis, pThis->elementCount, (CSC_BOOLEAN)TRUE, (CSC_BOOLEAN)TRUE);
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayResizeImpl(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_BOOLEAN reserve, _In_ CONST CSC_BOOLEAN shrink)
{
	CSC_SIZE_T allocationSize;
	CSC_PVOID pNewData;
	CSC_STATUS status = CSC_DynamicArrayIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	allocationSize = CSC_DynamicArrayCalculateRequestResize(pThis, numOfElements, reserve);

	if (allocationSize == CSC_DYNAMIC_ARRAY_CALCULATION_ERROR)
		return CSC_STATUS_INVALID_PARAMETER;

	if (reserve)
	{
		if ((shrink) ? (allocationSize >= pThis->reservedSpace) : (allocationSize <= pThis->reservedSpace))
			return CSC_STATUS_SUCCESS;
	}
	else
	{
		if ((shrink) ? (allocationSize == pThis->reservedSpace) : (allocationSize <= pThis->reservedSpace))
		{
			if (numOfElements > pThis->elementCount)
			{
				status = CSC_MemoryUtilsSetZeroMemory((CSC_PVOID)((CSC_BYTE* CONST)pThis->pData + pThis->elementSize * pThis->elementCount), pThis->elementSize * (numOfElements - pThis->elementCount));

				if (status != CSC_STATUS_SUCCESS)
					return status;
			}

			pThis->elementCount = numOfElements;

			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = pThis->elementCount;

				if (pThis->pIterator->iterationValid && pThis->pIterator->currentIndex >= pThis->elementCount)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}

			return CSC_STATUS_SUCCESS;
		}
	}

	status = CSC_AllocatorIsUsable(pThis->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pNewData = CSC_AllocatorAllocZero(pThis->pAllocator, allocationSize);

	if (!pNewData)
		return CSC_STATUS_MEMORY_NOT_ALLOCATED;

	if (pThis->pData)
	{
		if (reserve)
			status = CSC_MemoryUtilsCopyMemory(pNewData, (CSC_PCVOID)pThis->pData, pThis->elementSize * pThis->elementCount);
		else
			status = CSC_MemoryUtilsCopyMemory(pNewData, (CSC_PCVOID)pThis->pData, pThis->elementSize * ((pThis->elementCount < numOfElements) ? pThis->elementCount : numOfElements));

		if (status != CSC_STATUS_SUCCESS)
		{
			CSC_AllocatorFree(pThis->pAllocator, pNewData);
			return status;
		}

		CSC_AllocatorFree(pThis->pAllocator, pThis->pData);
	}

	pThis->pData = pNewData;
	pThis->reservedSpace = allocationSize;

	if (!reserve)
		pThis->elementCount = numOfElements;

	if (pThis->pIterator)
	{
		pThis->pIterator->elementCount = pThis->elementCount;

		if (pThis->pIterator->iterationValid)
		{
			pThis->pIterator->pCurrElement = CSC_DynamicArrayAccessElement(pThis, pThis->pIterator->currentIndex);

			if (!pThis->pIterator->pCurrElement)
				CSC_IteratorInvalidateIteration(pThis->pIterator);
		}
	}

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_DynamicArrayDestroy(_Inout_ CSC_DynamicArray* CONST pThis)
{
	CSC_STATUS status = CSC_DynamicArrayErase(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis->pIterator)
	{
		status = CSC_IteratorUnregister(pThis->pIterator);

		if (status != CSC_STATUS_SUCCESS)
			return status;
	}

	return CSC_DynamicArrayZeroMemory(pThis);
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayErase(_Inout_ CSC_DynamicArray* CONST pThis)
{
	CSC_STATUS status;

	if (!pThis || pThis->pVTableIterator != (CSC_PCVOID)g_dynamicArrayVirtualTable)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pThis->pData)
	{
		status = CSC_AllocatorIsUsable(pThis->pAllocator);

		if (status != CSC_STATUS_SUCCESS)
			return status;

		CSC_AllocatorFree(pThis->pAllocator, pThis->pData);
		pThis->pData = NULL;
	}
	
	pThis->elementCount = (CSC_SIZE_T)0;
	pThis->reservedSpace = (CSC_SIZE_T)0;

	if (pThis->pIterator)
	{
		pThis->pIterator->elementCount = pThis->elementCount;

		if (pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);
	}

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayClear(_Inout_ CSC_DynamicArray* CONST pThis)
{
	if (!pThis || !pThis->elementSize || pThis->pVTableIterator != (CSC_PCVOID)g_dynamicArrayVirtualTable)
		return CSC_STATUS_INVALID_PARAMETER;

	pThis->elementCount = (CSC_SIZE_T)0;

	if (pThis->pIterator)
	{
		pThis->pIterator->elementCount = pThis->elementCount;

		if (pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);
	}

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayZeroMemory(_Out_ CSC_DynamicArray* CONST pThis)
{
	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	pThis->pVTableIterator = NULL;
	pThis->pAllocator = NULL;
	pThis->pData = NULL;
	pThis->pIterator = NULL;
	pThis->elementSize = (CSC_SIZE_T)0;
	pThis->elementCount = (CSC_SIZE_T)0;
	pThis->reservedSpace = (CSC_SIZE_T)0;

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_DynamicArrayPushValue(_Inout_ CSC_DynamicArray* CONST pThis, _In_opt_ CONST CSC_PCVOID pValue)
{
	CSC_SIZE_T allocationSize;
	CSC_PVOID pNewData, pNewElement;
	CSC_STATUS status = CSC_DynamicArrayIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	allocationSize = CSC_DynamicArrayCalculatePushSize(pThis);

	if (allocationSize == CSC_DYNAMIC_ARRAY_CALCULATION_ERROR)
		return CSC_STATUS_INVALID_PARAMETER;

	if (allocationSize <= pThis->reservedSpace)
	{
		++pThis->elementCount;
		pNewElement = CSC_DynamicArrayBack(pThis);

		if (!pNewElement)
		{
			--pThis->elementCount;
			return CSC_STATUS_GENERAL_FAILURE;
		}

		status = ((pValue) ? CSC_MemoryUtilsCopyMemory(pNewElement, pValue, pThis->elementSize) : CSC_MemoryUtilsSetZeroMemory(pNewElement, pThis->elementSize));

		if (status != CSC_STATUS_SUCCESS)
			--pThis->elementCount;

		if (pThis->pIterator)
			pThis->pIterator->elementCount = pThis->elementCount;

		return CSC_STATUS_SUCCESS;
	}
	else
	{
		status = CSC_AllocatorIsUsable(pThis->pAllocator);

		if (status != CSC_STATUS_SUCCESS)
			return status;

		pNewData = CSC_AllocatorAllocZero(pThis->pAllocator, allocationSize);

		if (!pNewData)
			return CSC_STATUS_MEMORY_NOT_ALLOCATED;

		if (pThis->elementCount)
		{
			status = CSC_MemoryUtilsCopyMemory(pNewData, (CSC_PCVOID)pThis->pData, pThis->elementSize * pThis->elementCount);

			if (status != CSC_STATUS_SUCCESS)
			{
				CSC_AllocatorFree(pThis->pAllocator, pNewData);
				return status;
			}
		}

		pNewElement = (CSC_PVOID)((CSC_BYTE* CONST)pNewData + pThis->elementSize * pThis->elementCount);

		if (!pNewElement)
		{
			CSC_AllocatorFree(pThis->pAllocator, pNewData);
			return CSC_STATUS_GENERAL_FAILURE;
		}

		status = ((pValue) ? CSC_MemoryUtilsCopyMemory(pNewElement, pValue, pThis->elementSize) : CSC_MemoryUtilsSetZeroMemory(pNewElement, pThis->elementSize));

		if (status != CSC_STATUS_SUCCESS)
		{
			CSC_AllocatorFree(pThis->pAllocator, pNewData);
			return status;
		}

		if (pThis->elementCount++)
			CSC_AllocatorFree(pThis->pAllocator, pThis->pData);

		pThis->pData = pNewData;
		pThis->reservedSpace = allocationSize;

		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = pThis->elementCount;

			if (pThis->pIterator->iterationValid)
			{
				pThis->pIterator->pCurrElement = CSC_DynamicArrayAccessElement(pThis, pThis->pIterator->currentIndex);

				if (!pThis->pIterator->pCurrElement)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}

		return CSC_STATUS_SUCCESS;
	}
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayPopValue(_Inout_ CSC_DynamicArray* CONST pThis, _When_(return == CSC_STATUS_SUCCESS, _Out_opt_) CONST CSC_PVOID pValue)
{
	CSC_SIZE_T allocationSize;
	CSC_PVOID pNewData;
	CSC_STATUS status = CSC_DynamicArrayIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pValue)
	{
		status = CSC_MemoryUtilsSetZeroMemory(pValue, pThis->elementSize);

		if (status != CSC_STATUS_SUCCESS)
			return status;
	}

	allocationSize = CSC_DynamicArrayCalculatePopSize(pThis);

	if (allocationSize == CSC_DYNAMIC_ARRAY_CALCULATION_ERROR)
		return CSC_STATUS_INVALID_PARAMETER;

	if (!allocationSize)
	{
		status = CSC_AllocatorIsUsable(pThis->pAllocator);

		if (status != CSC_STATUS_SUCCESS)
			return status;

		if (pValue)
		{
			status = CSC_MemoryUtilsCopyMemory(pValue, (CSC_PCVOID)CSC_DynamicArrayBack(pThis), pThis->elementSize);

			if (status != CSC_STATUS_SUCCESS)
				return status;
		}

		CSC_DynamicArrayErase(pThis);

		return CSC_STATUS_SUCCESS;
	}

	if (allocationSize >= pThis->reservedSpace)
	{
		if (pValue)
		{
			status = CSC_MemoryUtilsCopyMemory(pValue, (CSC_PCVOID)CSC_DynamicArrayBack(pThis), pThis->elementSize);

			if (status != CSC_STATUS_SUCCESS)
				return status;
		}

		--pThis->elementCount;

		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = pThis->elementCount;

			if (pThis->pIterator->iterationValid && pThis->pIterator->currentIndex >= pThis->elementCount)
				CSC_IteratorInvalidateIteration(pThis->pIterator);
		}

		return CSC_STATUS_SUCCESS;
	}
	else
	{
		status = CSC_AllocatorIsUsable(pThis->pAllocator);

		if (status != CSC_STATUS_SUCCESS)
			return status;

		pNewData = CSC_AllocatorAllocZero(pThis->pAllocator, allocationSize);

		if (!pNewData)
			return CSC_STATUS_MEMORY_NOT_ALLOCATED;

		status = CSC_MemoryUtilsCopyMemory(pNewData, (CSC_PCVOID)pThis->pData, pThis->elementSize * (pThis->elementCount - (CSC_SIZE_T)1));

		if (status != CSC_STATUS_SUCCESS)
		{
			CSC_AllocatorFree(pThis->pAllocator, pNewData);
			return status;
		}

		if (pValue)
		{
			status = CSC_MemoryUtilsCopyMemory(pValue, (CSC_PCVOID)CSC_DynamicArrayBack(pThis), pThis->elementSize);

			if (status != CSC_STATUS_SUCCESS)
			{
				CSC_AllocatorFree(pThis->pAllocator, pNewData);
				return status;
			}
		}

		CSC_AllocatorFree(pThis->pAllocator, pThis->pData);
		pThis->pData = pNewData;
		pThis->reservedSpace = allocationSize;
		--pThis->elementCount;

		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = pThis->elementCount;

			if (pThis->pIterator->iterationValid)
			{
				pThis->pIterator->pCurrElement = CSC_DynamicArrayAccessElement(pThis, pThis->pIterator->currentIndex);

				if (!pThis->pIterator->pCurrElement)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}

		return CSC_STATUS_SUCCESS;
	}
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayLazyPopValue(_Inout_ CSC_DynamicArray* CONST pThis, _When_(return == CSC_STATUS_SUCCESS, _Out_opt_) CONST CSC_PVOID pValue)
{
	CSC_SIZE_T allocationSize;
	CSC_STATUS status = CSC_DynamicArrayIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pThis->reservedSpace)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pValue)
	{
		status = CSC_MemoryUtilsSetZeroMemory(pValue, pThis->elementSize);

		if (status != CSC_STATUS_SUCCESS)
			return status;
	}

	allocationSize = CSC_DynamicArrayCalculatePopSize(pThis);

	if (allocationSize == CSC_DYNAMIC_ARRAY_CALCULATION_ERROR)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pValue)
	{
		status = CSC_MemoryUtilsCopyMemory(pValue, (CSC_PCVOID)CSC_DynamicArrayBack(pThis), pThis->elementSize);

		if (status != CSC_STATUS_SUCCESS)
			return status;
	}

	--pThis->elementCount;

	if (pThis->pIterator)
	{
		pThis->pIterator->elementCount = pThis->elementCount;

		if (pThis->pIterator->iterationValid && pThis->pIterator->currentIndex >= pThis->elementCount)
			CSC_IteratorInvalidateIteration(pThis->pIterator);
	}

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayPopFront(_Inout_ CSC_DynamicArray* CONST pThis, _When_(return == CSC_STATUS_SUCCESS, _Out_opt_) CONST CSC_PVOID pValue)
{
	return CSC_DynamicArrayPopFrontImpl(pThis, pValue, (CSC_BOOLEAN)TRUE);
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayLazyPopFront(_Inout_ CSC_DynamicArray* CONST pThis, _When_(return == CSC_STATUS_SUCCESS, _Out_opt_) CONST CSC_PVOID pValue)
{
	return CSC_DynamicArrayPopFrontImpl(pThis, pValue, (CSC_BOOLEAN)FALSE);
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayPopFrontImpl(_Inout_ CSC_DynamicArray* CONST pThis, _When_(return == CSC_STATUS_SUCCESS, _Out_opt_) CONST CSC_PVOID pValue, _In_ CONST CSC_BOOLEAN shrink)
{
	CSC_STATUS status = CSC_DynamicArrayIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pValue)
	{
		status = CSC_MemoryUtilsCopyMemory(pValue, (CSC_PCVOID)CSC_DynamicArrayFront(pThis), pThis->elementSize);

		if (status != CSC_STATUS_SUCCESS)
			return status;
	}

	status = (shrink) ? CSC_DynamicArrayRemoveElement(pThis, (CSC_SIZE_T)0) : CSC_DynamicArrayLazyRemoveElement(pThis, (CSC_SIZE_T)0);

	if (pValue && status != CSC_STATUS_SUCCESS)
		CSC_MemoryUtilsSetZeroMemory(pValue, pThis->elementSize);

	return status;
}


CSC_PVOID CSCMETHOD CSC_DynamicArrayAccessElement(_In_ CONST CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T index)
{
	if (CSC_DynamicArrayIsValid(pThis) != CSC_STATUS_SUCCESS || !pThis->reservedSpace || index >= pThis->elementCount || !pThis->pData)
		return NULL;

	return (CSC_PVOID)((CSC_BYTE* CONST)pThis->pData + pThis->elementSize * index);
}


CSC_STATUS CSCMETHOD CSC_DynamicArrayAssign(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PCVOID pValue)
{
	CSC_DynamicArray arrayBuffer;
	CSC_PVOID pOldData;
	CSC_Iterator* pIterator;
	CSC_STATUS status = CSC_DynamicArrayIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!numOfElements || !pValue || numOfElements > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE / pThis->elementSize)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayInitializeWithValueImpl(&arrayBuffer, pThis->elementSize, numOfElements, pValue, pThis->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pOldData = pThis->pData;
	pIterator = pThis->pIterator;
	status = CSC_MemoryUtilsCopyMemory((CSC_PVOID)pThis, (CSC_PCVOID)&arrayBuffer, sizeof(arrayBuffer));

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_DynamicArrayDestroy(&arrayBuffer);
		return status;
	}

	if (pOldData)
		CSC_AllocatorFree(pThis->pAllocator, pOldData);

	if (pIterator)
	{
		pThis->pIterator = pIterator;
		pThis->pIterator->elementCount = pThis->elementCount;

		if (pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);
	}

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayAssignBlock(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PCVOID pValue)
{
	CSC_DynamicArray arrayBuffer;
	CSC_STATUS status = CSC_DynamicArrayInitializeWithCopyImpl(&arrayBuffer, pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayAssignBlockImpl(&arrayBuffer, firstIndex, numOfElements, pValue);

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_DynamicArrayDestroy(&arrayBuffer);
		return status;
	}

	status = CSC_DynamicArrayCopy(pThis, &arrayBuffer);

	if (status != CSC_STATUS_SUCCESS)
		CSC_DynamicArrayDestroy(&arrayBuffer);

	return status;
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayAssignRange(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T lastIndex, _In_ CONST CSC_PCVOID pValue)
{
	if (firstIndex > lastIndex)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_DynamicArrayAssignBlock(pThis, firstIndex, lastIndex - firstIndex + (CSC_SIZE_T)1, pValue);
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayAssignBlockImpl(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PCVOID pValue)
{
	CSC_SIZE_T iterator;
	CSC_STATUS status = CSC_DynamicArrayIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pValue || firstIndex >= pThis->elementCount || firstIndex + numOfElements > pThis->elementCount || !numOfElements)
		return CSC_STATUS_INVALID_PARAMETER;

	for (iterator = firstIndex; iterator < firstIndex + numOfElements; ++iterator)
	{
		status = CSC_MemoryUtilsCopyMemory(CSC_DynamicArrayAccessElement(pThis, iterator), pValue, pThis->elementSize);

		if (status != CSC_STATUS_SUCCESS)
			return status;
	}

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_DynamicArrayCopy(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_DynamicArray* CONST pSrc)
{
	CONST CSC_STATUS status = CSC_DynamicArrayIsValid(pSrc);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis == pSrc)
		return CSC_STATUS_SUCCESS;

	return CSC_DynamicArrayCopyArray(pThis, (CSC_PCVOID)pSrc->pData, pSrc->elementCount, pSrc->elementSize);
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayMove(_Inout_ CSC_DynamicArray* CONST pThis, _Inout_ CSC_DynamicArray* CONST pSrc)
{
	CONST CSC_STATUS status = CSC_DynamicArrayCopy(pThis, pSrc);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis == pSrc)
		return CSC_STATUS_SUCCESS;

	CSC_DynamicArrayErase(pSrc);

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayCopyArray(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_PCVOID pFirst, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_SIZE_T elementSize)
{
	CSC_SIZE_T allocationSize;
	CSC_PVOID pNewData;
	CSC_STATUS status = CSC_DynamicArrayIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pFirst || !elementSize)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_AllocatorIsUsable(pThis->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (numOfElements && !pFirst)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pThis->reservedSpace >= elementSize * numOfElements)
	{
		if (!numOfElements)
		{
			status = CSC_DynamicArrayClear(pThis);	// Possibly swap out for CSC_DynamicArrayErase?

			if (status != CSC_STATUS_SUCCESS)
				return status;

			pThis->elementSize = elementSize;

			if (pThis->pIterator)
				pThis->pIterator->elementSize = pThis->elementSize;

			return CSC_STATUS_SUCCESS;
		}

		status = CSC_MemoryUtilsCopyMemory(pThis->pData, pFirst, elementSize * numOfElements);

		if (status != CSC_STATUS_SUCCESS)
			return status;
	}
	else
	{
		allocationSize = CSC_DynamicArrayCalculateAllocReserve(elementSize, numOfElements);

		if (allocationSize == CSC_DYNAMIC_ARRAY_CALCULATION_ERROR)
			return CSC_STATUS_INVALID_PARAMETER;

		pNewData = CSC_AllocatorAllocZero(pThis->pAllocator, allocationSize);

		if (!pNewData)
			return CSC_STATUS_MEMORY_NOT_ALLOCATED;

		status = CSC_MemoryUtilsCopyMemory(pNewData, pFirst, elementSize * numOfElements);

		if (status != CSC_STATUS_SUCCESS)
		{
			CSC_AllocatorFree(pThis->pAllocator, pNewData);
			return status;
		}

		if (pThis->pData)
			CSC_AllocatorFree(pThis->pAllocator, pThis->pData);

		pThis->pData = pNewData;
		pThis->reservedSpace = allocationSize;
	}

	pThis->elementSize = elementSize;
	pThis->elementCount = numOfElements;

	if (pThis->pIterator)
	{
		pThis->pIterator->elementSize = pThis->elementSize;
		pThis->pIterator->elementCount = pThis->elementCount;

		if (pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);
	}

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_DynamicArrayInsertElement(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_opt_ CONST CSC_PCVOID pValue)
{
	return CSC_DynamicArrayInsertRange(pThis, insertIndex, pValue, (CSC_SIZE_T)1);
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayInsertRange(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_opt_ CONST CSC_PCVOID pFirst, _In_ CONST CSC_SIZE_T numOfElements)
{
	CSC_DynamicArray arrayBuffer;
	CSC_PVOID pOldData;
	CSC_Iterator* pIterator;
	CSC_STATUS status = CSC_DynamicArrayIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!numOfElements || insertIndex > pThis->elementCount || pThis->elementCount + numOfElements > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE / pThis->elementSize)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayInitializeWithCopyImpl(&arrayBuffer, pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayLazyResize(&arrayBuffer, arrayBuffer.elementCount + numOfElements);

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_DynamicArrayDestroy(&arrayBuffer);
		return status;
	}

	if (insertIndex < arrayBuffer.elementCount - numOfElements)
	{
		status = CSC_MemoryUtilsCopyMemory(CSC_DynamicArrayAccessElement(&arrayBuffer, insertIndex + numOfElements), (CSC_PCVOID)CSC_DynamicArrayAccessElement(&arrayBuffer, insertIndex), arrayBuffer.elementSize * (arrayBuffer.elementCount - numOfElements - insertIndex));

		if (status != CSC_STATUS_SUCCESS)
		{
			CSC_DynamicArrayDestroy(&arrayBuffer);
			return status;
		}
	}

	status = ((pFirst) ? CSC_MemoryUtilsCopyMemory(CSC_DynamicArrayAccessElement(&arrayBuffer, insertIndex), pFirst, arrayBuffer.elementSize * numOfElements) : CSC_MemoryUtilsSetZeroMemory(CSC_DynamicArrayAccessElement(&arrayBuffer, insertIndex), arrayBuffer.elementSize * numOfElements));

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_DynamicArrayDestroy(&arrayBuffer);
		return status;
	}
		
	pOldData = pThis->pData;
	pIterator = pThis->pIterator;
	status = CSC_MemoryUtilsCopyMemory((CSC_PVOID)pThis, (CSC_PCVOID)&arrayBuffer, sizeof(arrayBuffer));

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_DynamicArrayDestroy(&arrayBuffer);
		return status;
	}

	if (pOldData)
		CSC_AllocatorFree(pThis->pAllocator, pOldData);

	if (pIterator)
	{
		pThis->pIterator = pIterator;
		pThis->pIterator->elementCount = pThis->elementCount;

		if (pThis->pIterator->iterationValid)
		{
			if (insertIndex <= pThis->pIterator->currentIndex)
				pThis->pIterator->currentIndex += numOfElements;

			pThis->pIterator->pCurrElement = CSC_DynamicArrayAccessElement(pThis, pThis->pIterator->currentIndex);

			if (!pThis->pIterator->pCurrElement)
				CSC_IteratorInvalidateIteration(pThis->pIterator);
		}
	}

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayInsertArray(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_ CONST CSC_DynamicArray* CONST pSrc)
{
	CSC_STATUS status = CSC_DynamicArrayIsValid(pSrc);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis->elementSize != pSrc->elementSize)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_DynamicArrayInsertRange(pThis, insertIndex, (CSC_PCVOID)pSrc->pData, pSrc->elementCount);
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayAppendCopy(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_DynamicArray* CONST pSrc)
{
	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_DynamicArrayInsertArray(pThis, pThis->elementCount, pSrc);
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayAppendMove(_Inout_ CSC_DynamicArray* CONST pThis, _Inout_ CSC_DynamicArray* CONST pSrc)
{
	CONST CSC_STATUS status = CSC_DynamicArrayAppendCopy(pThis, (CONST CSC_DynamicArray* CONST)pSrc);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis == pSrc)
		return CSC_STATUS_SUCCESS;

	CSC_DynamicArrayErase(pSrc);

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_DynamicArrayRemoveElement(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T removeIndex)
{
	return CSC_DynamicArrayRemoveRange(pThis, removeIndex, (CSC_SIZE_T)1);
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayLazyRemoveElement(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T removeIndex)
{
	return CSC_DynamicArrayLazyRemoveRange(pThis, removeIndex, (CSC_SIZE_T)1);
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayRemoveRange(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T removeIndex, _In_ CONST CSC_SIZE_T numOfElements)
{
	CSC_SIZE_T allocationSize;
	CSC_PVOID pNewData;

	CSC_STATUS status = CSC_DynamicArrayIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (removeIndex + numOfElements > pThis->elementCount || !numOfElements)
		return CSC_STATUS_INVALID_PARAMETER;

	if (numOfElements == pThis->elementCount)
		return CSC_DynamicArrayErase(pThis);

	status = CSC_AllocatorIsUsable(pThis->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	allocationSize = (numOfElements == (CSC_SIZE_T)1) ? CSC_DynamicArrayCalculatePopSize(pThis) : CSC_DynamicArrayCalculateRequestResize(pThis, pThis->elementCount - numOfElements, (CSC_BOOLEAN)FALSE);

	if (allocationSize == CSC_DYNAMIC_ARRAY_CALCULATION_ERROR)
		return CSC_STATUS_INVALID_PARAMETER;

	pNewData = CSC_AllocatorAllocZero(pThis->pAllocator, allocationSize);

	if (!pNewData)
		return CSC_STATUS_MEMORY_NOT_ALLOCATED;

	if (removeIndex)
	{
		status = CSC_MemoryUtilsCopyMemory(pNewData, (CSC_PCVOID)CSC_DynamicArrayFront(pThis), pThis->elementSize * removeIndex);

		if (status != CSC_STATUS_SUCCESS)
		{
			CSC_AllocatorFree(pThis->pAllocator, pNewData);
			return status;
		}
	}

	if (removeIndex + numOfElements < pThis->elementCount)
	{
		status = CSC_MemoryUtilsCopyMemory((CSC_PVOID)((CSC_BYTE* CONST)pNewData + (pThis->elementSize * removeIndex)), (CSC_PCVOID)CSC_DynamicArrayAccessElement(pThis, removeIndex + numOfElements), pThis->elementSize * (pThis->elementCount - removeIndex - numOfElements));

		if (status != CSC_STATUS_SUCCESS)
		{
			CSC_AllocatorFree(pThis->pAllocator, pNewData);
			return status;
		}
	}

	CSC_AllocatorFree(pThis->pAllocator, pThis->pData);
	pThis->pData = pNewData;
	pThis->elementCount -= numOfElements;
	pThis->reservedSpace = allocationSize;

	if (pThis->pIterator)
	{
		pThis->pIterator->elementCount = pThis->elementCount;

		if (pThis->pIterator->iterationValid)
		{
			if (pThis->pIterator->currentIndex >= removeIndex && pThis->pIterator->currentIndex < removeIndex + numOfElements)
			{
				CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
			else
			{
				if (pThis->pIterator->currentIndex >= removeIndex + numOfElements)
					pThis->pIterator->currentIndex -= numOfElements;

				pThis->pIterator->pCurrElement = CSC_DynamicArrayAccessElement(pThis, pThis->pIterator->currentIndex);

				if (!pThis->pIterator->pCurrElement)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}
	}

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayLazyRemoveRange(_Inout_ CSC_DynamicArray* CONST pThis, _In_ CONST CSC_SIZE_T removeIndex, _In_ CONST CSC_SIZE_T numOfElements)
{
	CSC_STATUS status = CSC_DynamicArrayIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (removeIndex + numOfElements > pThis->elementCount || !numOfElements)
		return CSC_STATUS_INVALID_PARAMETER;

	if (removeIndex + numOfElements < pThis->elementCount)
	{
		status = CSC_MemoryUtilsCopyMemory(CSC_DynamicArrayAccessElement(pThis, removeIndex), (CSC_PCVOID)CSC_DynamicArrayAccessElement(pThis, removeIndex + numOfElements), pThis->elementSize * (pThis->elementCount - removeIndex - numOfElements));

		if (status != CSC_STATUS_SUCCESS)
			return status;
	}

	pThis->elementCount -= numOfElements;

	if (pThis->pIterator)
	{
		pThis->pIterator->elementCount = pThis->elementCount;

		if (pThis->pIterator->iterationValid)
		{
			if (pThis->pIterator->currentIndex >= removeIndex && pThis->pIterator->currentIndex < removeIndex + numOfElements)
			{
				CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
			else
			{
				if (pThis->pIterator->currentIndex >= removeIndex + numOfElements)
					pThis->pIterator->currentIndex -= numOfElements;

				pThis->pIterator->pCurrElement = CSC_DynamicArrayAccessElement(pThis, pThis->pIterator->currentIndex);

				if (!pThis->pIterator->pCurrElement)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}
	}

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_DynamicArrayReverse(_Inout_ CSC_DynamicArray* CONST pThis)
{
	CSC_SIZE_T iterator;
	CSC_PVOID pCurrElement, pNewData;
	CSC_STATUS status = CSC_DynamicArrayIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pThis->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pThis->elementCount == (CSC_SIZE_T)1)
		return CSC_STATUS_SUCCESS;

	status = CSC_AllocatorIsUsable(pThis->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pNewData = CSC_AllocatorAllocZero(pThis->pAllocator, pThis->reservedSpace);

	if (!pNewData)
		return CSC_STATUS_MEMORY_NOT_ALLOCATED;

	pCurrElement = NULL;

	for (iterator = (CSC_SIZE_T)0; iterator < pThis->elementCount; ++iterator)
	{
		pCurrElement = CSC_DynamicArrayAccessElement(pThis, pThis->elementCount - iterator - 1);

		if (!pCurrElement)
		{
			CSC_AllocatorFree(pThis->pAllocator, pNewData);
			return CSC_STATUS_GENERAL_FAILURE;
		}

		status = CSC_MemoryUtilsCopyMemory((CSC_PVOID)((CSC_BYTE* CONST)pNewData + (pThis->elementSize * iterator)), (CSC_PCVOID)pCurrElement, pThis->elementSize);

		if (status != CSC_STATUS_SUCCESS)
		{
			CSC_AllocatorFree(pThis->pAllocator, pNewData);
			return status;
		}
	}

	CSC_AllocatorFree(pThis->pAllocator, pThis->pData);
	pThis->pData = pNewData;

	if (pThis->pIterator && pThis->pIterator->iterationValid)
		CSC_IteratorInvalidateIteration(pThis->pIterator);

	return CSC_STATUS_SUCCESS;
}


CSC_PVOID CSCMETHOD CSC_DynamicArrayFront(_In_ CONST CSC_DynamicArray* CONST pThis)
{
	return CSC_DynamicArrayAccessElement(pThis, (CSC_SIZE_T)0);
}

CSC_PVOID CSCMETHOD CSC_DynamicArrayBack(_In_ CONST CSC_DynamicArray* CONST pThis)
{
	if (!pThis || !pThis->elementCount)
		return NULL;

	return CSC_DynamicArrayAccessElement(pThis, pThis->elementCount - (CSC_SIZE_T)1);
}

CSC_PVOID CSCMETHOD CSC_DynamicArrayData(_In_ CONST CSC_DynamicArray* CONST pThis)
{
	return CSC_DynamicArrayFront(pThis);
}


CSC_STATUS CSCMETHOD CSC_DynamicArrayIsEmpty(_In_ CONST CSC_DynamicArray* CONST pThis)
{
	return ((CSC_DynamicArrayIsValid(pThis) == CSC_STATUS_SUCCESS && pThis->elementCount && pThis->pData) ? CSC_STATUS_GENERAL_FAILURE : CSC_STATUS_SUCCESS);
}

CSC_STATUS CSCMETHOD CSC_DynamicArrayIsValid(_In_ CONST CSC_DynamicArray* CONST pThis)
{
	return (!pThis || !pThis->elementSize || pThis->pVTableIterator != (CSC_PCVOID)g_dynamicArrayVirtualTable || (pThis->elementCount * pThis->elementSize) > pThis->reservedSpace || pThis->elementCount > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE / pThis->elementSize || pThis->elementSize > CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE) ? CSC_STATUS_INVALID_PARAMETER : CSC_STATUS_SUCCESS;
}


CSC_SIZE_T CSCMETHOD CSC_DynamicArrayGetSize(_In_ CONST CSC_DynamicArray* CONST pThis)
{
	return ((CSC_DynamicArrayIsValid(pThis) == CSC_STATUS_SUCCESS) ? pThis->elementCount : (CSC_SIZE_T)0);
}

CSC_SIZE_T CSCMETHOD CSC_DynamicArrayGetCapacity(_In_ CONST CSC_DynamicArray* CONST pThis)
{
	return ((CSC_DynamicArrayIsValid(pThis) == CSC_STATUS_SUCCESS && pThis->reservedSpace) ? (pThis->reservedSpace / pThis->elementSize) : (CSC_SIZE_T)0);
}

CSC_SIZE_T CSCMETHOD CSC_DynamicArrayGetMaxElements(_In_ CONST CSC_DynamicArray* CONST pThis)
{
	return ((CSC_DynamicArrayIsValid(pThis) == CSC_STATUS_SUCCESS) ? (CSC_DYNAMIC_ARRAY_MAXIMUM_SPACE / pThis->elementSize) : (CSC_SIZE_T)0);
}

CSC_SIZE_T CSCMETHOD CSC_DynamicArrayGetElementSize(_In_ CONST CSC_DynamicArray* CONST pThis)
{
	return ((CSC_DynamicArrayIsValid(pThis) == CSC_STATUS_SUCCESS) ? pThis->elementSize : (CSC_SIZE_T)0);
}

CSC_PVOID CSCMETHOD CSC_DynamicArrayGetAllocator(_In_ CONST CSC_DynamicArray* CONST pThis)
{
	return ((CSC_DynamicArrayIsValid(pThis) == CSC_STATUS_SUCCESS) ? pThis->pAllocator : NULL);
}


static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorRegister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_DynamicArray* CONST pDynArray = (CSC_DynamicArray* CONST)pContainer;
	CONST CSC_STATUS status = CSC_DynamicArrayIsValid(pDynArray);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator->pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pDynArray->pIterator)
		return CSC_STATUS_GENERAL_FAILURE;

	pIterator->elementSize = pDynArray->elementSize;
	pIterator->elementCount = pDynArray->elementCount;
	pIterator->swapByIndex = (CSC_BOOLEAN)TRUE;
	pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
	pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	pIterator->pCurrElement = NULL;
	pIterator->pContainer = pContainer;

	pDynArray->pIterator = pIterator;

	return CSC_STATUS_SUCCESS;
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorUnregister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_DynamicArray* CONST pDynArray = (CSC_DynamicArray* CONST)pContainer;
	CONST CSC_STATUS status = CSC_DynamicArrayIsValid(pDynArray);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pDynArray->pIterator != pIterator)
		return CSC_STATUS_GENERAL_FAILURE;

	pDynArray->pIterator = NULL;
	
	CSC_IteratorZeroMemory(pIterator);

	return CSC_STATUS_SUCCESS;
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorFirstElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CONST CSC_DynamicArray* CONST pDynArray = (CONST CSC_DynamicArray* CONST)pContainer;
	CONST CSC_STATUS status = CSC_DynamicArrayIsValid(pDynArray);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pDynArray->pIterator || pIterator->elementSize != pDynArray->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pDynArray->elementCount;

	if (!pDynArray->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->pCurrElement = CSC_DynamicArrayFront(pDynArray);

	if (pIterator->pCurrElement)
	{
		pIterator->iterationValid = (CSC_BOOLEAN)TRUE;
		pIterator->currentIndex = (CSC_SIZE_T)0;
	}
	else
	{
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorNextElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CONST CSC_DynamicArray* CONST pDynArray = (CONST CSC_DynamicArray* CONST)pContainer;
	CONST CSC_STATUS status = CSC_DynamicArrayIsValid(pDynArray);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pDynArray->pIterator || pIterator->elementSize != pDynArray->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pDynArray->elementCount;

	if (!pIterator->iterationValid)
		return CSC_STATUS_INVALID_PARAMETER;

	if (!pDynArray->elementCount || pIterator->currentIndex >= pDynArray->elementCount)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return CSC_STATUS_INVALID_PARAMETER;
	}

	++pIterator->currentIndex;
	pIterator->pCurrElement = CSC_DynamicArrayAccessElement(pDynArray, pIterator->currentIndex);

	if (!pIterator->pCurrElement)
	{
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorLastElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CONST CSC_DynamicArray* CONST pDynArray = (CONST CSC_DynamicArray* CONST)pContainer;
	CONST CSC_STATUS status = CSC_DynamicArrayIsValid(pDynArray);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pDynArray->pIterator || pIterator->elementSize != pDynArray->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pDynArray->elementCount;

	if (!pDynArray->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->pCurrElement = CSC_DynamicArrayBack(pDynArray);

	if (pIterator->pCurrElement)
	{
		pIterator->iterationValid = (CSC_BOOLEAN)TRUE;
		pIterator->currentIndex = pDynArray->elementCount - (CSC_SIZE_T)1;
	}
	else
	{
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorPreviousElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CONST CSC_DynamicArray* CONST pDynArray = (CONST CSC_DynamicArray* CONST)pContainer;
	CONST CSC_STATUS status = CSC_DynamicArrayIsValid(pDynArray);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pDynArray->pIterator || pIterator->elementSize != pDynArray->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pDynArray->elementCount;

	if (!pIterator->iterationValid)
		return CSC_STATUS_INVALID_PARAMETER;

	if (!pDynArray->elementCount || pIterator->currentIndex >= pDynArray->elementCount || !pIterator->currentIndex)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return (!pIterator->currentIndex) ? CSC_STATUS_GENERAL_FAILURE : CSC_STATUS_INVALID_PARAMETER;
	}

	--pIterator->currentIndex;
	pIterator->pCurrElement = CSC_DynamicArrayAccessElement(pDynArray, pIterator->currentIndex);

	if (!pIterator->pCurrElement)
	{
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorMoveToIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index)
{
	CONST CSC_DynamicArray* CONST pDynArray = (CONST CSC_DynamicArray* CONST)pContainer;
	CONST CSC_STATUS status = CSC_DynamicArrayIsValid(pDynArray);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pDynArray->pIterator || pIterator->elementSize != pDynArray->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pDynArray->elementCount;

	if (!pDynArray->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->pCurrElement = CSC_DynamicArrayAccessElement(pDynArray, index);

	if (pIterator->pCurrElement)
	{
		pIterator->iterationValid = (CSC_BOOLEAN)TRUE;
		pIterator->currentIndex = index;
	}
	else
	{
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_SIZE_T CSCMETHOD CSC_DynamicArrayIteratorGetCurrentElementIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CONST CSC_DynamicArray* CONST pDynArray = (CONST CSC_DynamicArray* CONST)pContainer;
	CONST CSC_STATUS status = CSC_DynamicArrayIsValid(pDynArray);

	if (status != CSC_STATUS_SUCCESS)
		return CSC_ITERATOR_INVALID_INDEX;

	if (!pIterator || pIterator != pDynArray->pIterator || pIterator->elementSize != pDynArray->elementSize || pIterator->pContainer != pContainer)
		return CSC_ITERATOR_INVALID_INDEX;

	pIterator->elementCount = pDynArray->elementCount;

	if (!pIterator->iterationValid)
		return CSC_ITERATOR_INVALID_INDEX;

	if (!pDynArray->elementCount || pIterator->currentIndex >= pDynArray->elementCount)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return CSC_ITERATOR_INVALID_INDEX;
	}
	
	return pIterator->currentIndex;
}

static CSC_PVOID CSCMETHOD CSC_DynamicArrayIteratorGetCurrentElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CONST CSC_DynamicArray* CONST pDynArray = (CONST CSC_DynamicArray* CONST)pContainer;
	CONST CSC_STATUS status = CSC_DynamicArrayIsValid(pDynArray);

	if (status != CSC_STATUS_SUCCESS)
		return NULL;

	if (!pIterator || pIterator != pDynArray->pIterator || pIterator->elementSize != pDynArray->elementSize || pIterator->pContainer != pContainer)
		return NULL;

	pIterator->elementCount = pDynArray->elementCount;

	if (!pIterator->iterationValid)
		return NULL;

	if (!pDynArray->elementCount || pIterator->currentIndex >= pDynArray->elementCount)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return NULL;
	}

	return CSC_DynamicArrayAccessElement(pDynArray, pIterator->currentIndex);
}

static CSC_PVOID CSCMETHOD CSC_DynamicArrayIteratorGetElementAt(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index)
{
	CONST CSC_DynamicArray* CONST pDynArray = (CONST CSC_DynamicArray* CONST)pContainer;
	CONST CSC_STATUS status = CSC_DynamicArrayIsValid(pDynArray);

	if (status != CSC_STATUS_SUCCESS)
		return NULL;

	if (!pIterator || pIterator != pDynArray->pIterator || pIterator->elementSize != pDynArray->elementSize || pIterator->pContainer != pContainer)
		return NULL;

	pIterator->elementCount = pDynArray->elementCount;

	if (!pDynArray->elementCount)
		return NULL;

	return CSC_DynamicArrayAccessElement(pDynArray, index);
}

static CSC_SIZE_T CSCMETHOD CSC_DynamicArrayIteratorGetElementCount(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CONST CSC_DynamicArray* CONST pDynArray = (CONST CSC_DynamicArray* CONST)pContainer;
	CONST CSC_STATUS status = CSC_DynamicArrayIsValid(pDynArray);

	if (status != CSC_STATUS_SUCCESS)
		return CSC_ITERATOR_INVALID_LENGTH;

	if (!pIterator || pIterator != pDynArray->pIterator || pIterator->elementSize != pDynArray->elementSize || pIterator->pContainer != pContainer)
		return CSC_ITERATOR_INVALID_LENGTH;

	pIterator->elementCount = pDynArray->elementCount;

	return pDynArray->elementCount;
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorSwapElementsByIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T secondIndex)
{
	CONST CSC_DynamicArray* CONST pDynArray = (CONST CSC_DynamicArray* CONST)pContainer;
	CSC_STATUS status = CSC_DynamicArrayIsValid(pDynArray);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pDynArray->pIterator || pIterator->elementSize != pDynArray->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pDynArray->elementCount;

	if (!pDynArray->elementCount || firstIndex >= pDynArray->elementCount || secondIndex >= pDynArray->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	if (firstIndex == secondIndex)
		return CSC_STATUS_SUCCESS;

	status = CSC_AllocatorIsUsable(pDynArray->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	return CSC_MemoryUtilsSwapValues(CSC_DynamicArrayAccessElement(pDynArray, firstIndex), CSC_DynamicArrayAccessElement(pDynArray, secondIndex), pDynArray->elementSize, pDynArray->pAllocator);
}

static CSC_STATUS CSCMETHOD CSC_DynamicArrayIteratorSwapElementsByPtr(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond)
{
	CONST CSC_DynamicArray* CONST pDynArray = (CONST CSC_DynamicArray* CONST)pContainer;
	CONST CSC_PVOID pFront = CSC_DynamicArrayFront(pDynArray);
	CONST CSC_PVOID pBack = CSC_DynamicArrayBack(pDynArray);
	CSC_STATUS status = CSC_DynamicArrayIsValid(pDynArray);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pDynArray->pIterator || pIterator->elementSize != pDynArray->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pDynArray->elementCount;

	if (!pDynArray->elementCount || !pFront || !pBack || (CSC_POINTER_TYPE)pFirst < (CSC_POINTER_TYPE)pFront || (CSC_POINTER_TYPE)pFirst >(CSC_POINTER_TYPE)pBack || (CSC_POINTER_TYPE)pSecond < (CSC_POINTER_TYPE)pFront || (CSC_POINTER_TYPE)pSecond >(CSC_POINTER_TYPE)pBack)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pFirst == pSecond)
		return CSC_STATUS_SUCCESS;

	status = CSC_AllocatorIsUsable(pDynArray->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	return CSC_MemoryUtilsSwapValues(pFirst, pSecond, pDynArray->elementSize, pDynArray->pAllocator);
}
