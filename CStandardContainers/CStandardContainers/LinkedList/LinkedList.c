#include "LinkedList.h"
#include "../Allocator/Allocator.h"
#include "../Utils/MemoryUtils.h"

static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorRegister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorUnregister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorFirstElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorNextElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorLastElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorPreviousElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorMoveToIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index);
static CSC_SIZE_T CSCMETHOD CSC_LinkedListIteratorGetCurrentElementIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_PVOID CSCMETHOD CSC_LinkedListIteratorGetCurrentElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_PVOID CSCMETHOD CSC_LinkedListIteratorGetElementAt(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index);
static CSC_SIZE_T CSCMETHOD CSC_LinkedListIteratorGetElementCount(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorSwapElementsByIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T secondIndex);
static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorSwapElementsByPtr(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond);

static CONST CSC_PCVOID g_linkedListVirtualTable[csc_vfn_it_num_of_entries] =
{
	(CSC_PCVOID)CSC_LinkedListIteratorRegister,
	(CSC_PCVOID)CSC_LinkedListIteratorUnregister,
	(CSC_PCVOID)CSC_LinkedListIteratorFirstElement,
	(CSC_PCVOID)CSC_LinkedListIteratorNextElement,
	(CSC_PCVOID)CSC_LinkedListIteratorLastElement,
	(CSC_PCVOID)CSC_LinkedListIteratorPreviousElement,
	(CSC_PCVOID)CSC_LinkedListIteratorMoveToIndex,
	(CSC_PCVOID)CSC_LinkedListIteratorGetCurrentElementIndex,
	(CSC_PCVOID)CSC_LinkedListIteratorGetCurrentElement,
	(CSC_PCVOID)CSC_LinkedListIteratorGetElementAt,
	(CSC_PCVOID)CSC_LinkedListIteratorGetElementCount,
	(CSC_PCVOID)CSC_LinkedListIteratorSwapElementsByIndex,
	(CSC_PCVOID)CSC_LinkedListIteratorSwapElementsByPtr
};

#define CSC_LINKED_LIST_MAXIMUM_LENGTH (CSC_SIZE_T)0x40000000
#define CSC_LINKED_LIST_MAXIMUM_ELEMENT_SIZE (CSC_SIZE_T)0x10000


static CSC_STATUS CSCMETHOD CSC_LinkedListInsertElementImpl(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_opt_ LLNode* CONST pValue);
static CSC_STATUS CSCMETHOD CSC_LinkedListRotateImpl(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T rotationCount, _In_ CONST CSC_BOOLEAN forward);


CSC_STATUS CSCMETHOD CSC_LinkedListInitialize(_Out_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_BOOLEAN circular, _In_ CONST CSC_PVOID pAllocator)
{
	CSC_STATUS status = CSC_LinkedListZeroMemory(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (elementSize <= sizeof(LLNode) || elementSize > CSC_LINKED_LIST_MAXIMUM_ELEMENT_SIZE || !pAllocator)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_AllocatorIsUsable(pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pThis->pVTableIterator = (CSC_PCVOID)g_linkedListVirtualTable;
	pThis->pAllocator = pAllocator;
	pThis->elementSize = elementSize;
	pThis->circular = circular;

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_LinkedListInitializeWithSize(_Out_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T elementSize, _In_ CONST CSC_BOOLEAN circular, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PVOID pAllocator)
{
	LLNode* pNode;
	CSC_SIZE_T iterator;
	CSC_LinkedList llBuffer;
	CSC_STATUS status = CSC_LinkedListZeroMemory(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (numOfElements > CSC_LINKED_LIST_MAXIMUM_LENGTH || elementSize > CSC_LINKED_LIST_MAXIMUM_ELEMENT_SIZE)
		return CSC_STATUS_INVALID_PARAMETER;

	if (!numOfElements)
		return CSC_LinkedListInitialize(pThis, elementSize, circular, pAllocator);

	status = CSC_LinkedListInitialize(&llBuffer, elementSize, circular, pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_LinkedListInsertRange(&llBuffer, (CSC_SIZE_T)0, NULL, numOfElements);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_MemoryUtilsCopyMemory((CSC_PVOID)pThis, (CSC_PCVOID)&llBuffer, sizeof(llBuffer));

	if (status != CSC_STATUS_SUCCESS)
		CSC_LinkedListDestroy(&llBuffer);

	pNode = pThis->pListHead;

	for (iterator = (CSC_SIZE_T)0; iterator < numOfElements; ++iterator)
	{
		if (!pNode)
		{
			CSC_LinkedListDestroy(pThis);
			return CSC_STATUS_GENERAL_FAILURE;
		}

		pNode->pLL = pThis;
		pNode = pNode->pNext;
	}

	return status;
}


CSC_STATUS CSCMETHOD CSC_LinkedListInsertElement(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_opt_ CONST CSC_PCVOID pValue)
{
	LLNode* pNewElement;
	CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis->elementCount >= CSC_LINKED_LIST_MAXIMUM_LENGTH)
		return CSC_STATUS_GENERAL_FAILURE;

	if (insertIndex > pThis->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_AllocatorIsUsable(pThis->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pNewElement = (LLNode*)CSC_AllocatorAllocZero(pThis->pAllocator, pThis->elementSize);

	if (!pNewElement)
		return CSC_STATUS_MEMORY_NOT_ALLOCATED;

	if (pValue)
	{
		status = CSC_MemoryUtilsCopyMemory((CSC_PVOID)pNewElement, pValue, pThis->elementSize);

		if (status != CSC_STATUS_SUCCESS)
		{
			CSC_AllocatorFree(pThis->pAllocator, (CSC_PVOID)pNewElement);
			return status;
		}
	}

	pNewElement->elementSize = pThis->elementSize;
	status = CSC_LinkedListInsertElementImpl(pThis, insertIndex, pNewElement);

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_AllocatorFree(pThis->pAllocator, (CSC_PVOID)pNewElement);
		return status;
	}

	return CSC_STATUS_SUCCESS;
}

static CSC_STATUS CSCMETHOD CSC_LinkedListInsertElementImpl(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_opt_ LLNode* CONST pValue)
{
	LLNode *pInsertElement, *pPrevElement;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis->elementCount >= CSC_LINKED_LIST_MAXIMUM_LENGTH)
		return CSC_STATUS_GENERAL_FAILURE;

	if (insertIndex > pThis->elementCount || !pValue || pValue->elementSize != pThis->elementSize)
		return CSC_STATUS_INVALID_PARAMETER;

	if (!insertIndex)
	{
		if (insertIndex == pThis->elementCount)
		{
			pValue->pNext = ((pThis->circular) ? pValue : NULL);
			pValue->pPrev = pValue->pNext;

			pValue->pLL = pThis;

			++pThis->elementCount;
			pThis->pListHead = pValue;

			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = pThis->elementCount;

				if (pThis->pIterator->iterationValid)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}
		else
		{
			pInsertElement = (LLNode*)CSC_LinkedListFront(pThis);

			if (!pInsertElement)
				return CSC_STATUS_GENERAL_FAILURE;

			pValue->pPrev = pInsertElement->pPrev;
			pValue->pNext = pInsertElement;

			if (pValue->pPrev)
				pValue->pPrev->pNext = pValue;

			pInsertElement->pPrev = pValue;

			pValue->pLL = pThis;

			++pThis->elementCount;
			pThis->pListHead = pValue;

			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = pThis->elementCount;

				if (pThis->pIterator->iterationValid)
					++pThis->pIterator->currentIndex;
			}
		}
	}
	else
	{
		if (insertIndex == pThis->elementCount)
		{
			pPrevElement = (LLNode*)CSC_LinkedListBack(pThis);

			if (!pPrevElement)
				return CSC_STATUS_GENERAL_FAILURE;

			pValue->pPrev = pPrevElement;
			pValue->pNext = pPrevElement->pNext;

			if (pValue->pNext)
				pValue->pNext->pPrev = pValue;

			pPrevElement->pNext = pValue;

			pValue->pLL = pThis;

			++pThis->elementCount;

			if (pThis->pIterator)
				pThis->pIterator->elementCount = pThis->elementCount;
		}
		else
		{
			pInsertElement = (LLNode*)CSC_LinkedListAccessElement(pThis, insertIndex);

			if (!pInsertElement)
				return CSC_STATUS_GENERAL_FAILURE;

			pValue->pPrev = pInsertElement->pPrev;
			pValue->pNext = pInsertElement;

			pValue->pPrev->pNext = pValue;
			pInsertElement->pPrev = pValue;

			pValue->pLL = pThis;

			++pThis->elementCount;

			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = pThis->elementCount;

				if (pThis->pIterator->iterationValid && insertIndex <= pThis->pIterator->currentIndex)
					++pThis->pIterator->currentIndex;
			}
		}
	}

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_LinkedListInsertRange(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_opt_ CONST CSC_PCVOID pElements, _In_ CONST CSC_SIZE_T numOfElements)
{
	LLNode *pFirst, *pLast, *pOld, *pInsertElement, *pPrevElement;
	CSC_SIZE_T iterator;
	CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis->elementCount + numOfElements > CSC_LINKED_LIST_MAXIMUM_LENGTH)
		return CSC_STATUS_GENERAL_FAILURE;

	if (insertIndex > pThis->elementCount || !numOfElements)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_AllocatorIsUsable(pThis->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pInsertElement = NULL;
	pPrevElement = NULL;

	if (!insertIndex)
	{
		if (insertIndex != pThis->elementCount)
		{
			pInsertElement = (LLNode*)CSC_LinkedListFront(pThis);

			if (!pInsertElement)
				return CSC_STATUS_GENERAL_FAILURE;
		}
	}
	else
	{
		if (insertIndex == pThis->elementCount)
		{
			pPrevElement = (LLNode*)CSC_LinkedListBack(pThis);

			if (!pPrevElement)
				return CSC_STATUS_GENERAL_FAILURE;
		}
		else
		{
			pInsertElement = (LLNode*)CSC_LinkedListAccessElement(pThis, insertIndex);

			if (!pInsertElement)
				return CSC_STATUS_GENERAL_FAILURE;
		}
	}

	pOld = NULL;
	pFirst = NULL;
	pLast = NULL;

	for (iterator = (CSC_SIZE_T)0; iterator < numOfElements; ++iterator)
	{
		pFirst = (LLNode*)CSC_AllocatorAllocZero(pThis->pAllocator, pThis->elementSize);

		if (pElements)
		{
			if (CSC_MemoryUtilsCopyMemory((CSC_PVOID)pFirst, (CSC_PCVOID)((CONST CSC_BYTE* CONST)pElements + (numOfElements - iterator - (CSC_SIZE_T)1) * pThis->elementSize), pThis->elementSize) != CSC_STATUS_SUCCESS)
			{
				CSC_AllocatorFree(pThis->pAllocator, (CSC_PVOID)pFirst);
				pFirst = NULL;
			}
		}

		if (!pFirst)
		{
			while (pOld)
			{
				pLast = pOld;
				pOld = pOld->pNext;
				
				CSC_AllocatorFree(pThis->pAllocator, (CSC_PVOID)pLast);
			}

			return CSC_STATUS_GENERAL_FAILURE;
		}

		if (!pLast)
			pLast = pFirst;

		pFirst->pNext = pOld;
		pFirst->pPrev = NULL;
		pFirst->elementSize = pThis->elementSize;
		pFirst->pLL = pThis;

		if (pOld)
			pOld->pPrev = pFirst;

		pOld = pFirst;
	}

	if (!insertIndex)
	{
		if (insertIndex == pThis->elementCount)
		{
			pLast->pNext = ((pThis->circular) ? pFirst : NULL);
			pFirst->pPrev = ((pThis->circular) ? pLast : NULL);

			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = pThis->elementCount + numOfElements;

				if (pThis->pIterator->iterationValid)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}
		else
		{
			pFirst->pPrev = pInsertElement->pPrev;
			pLast->pNext = pInsertElement;

			if (pFirst->pPrev)
				pFirst->pPrev->pNext = pFirst;

			pInsertElement->pPrev = pLast;

			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = pThis->elementCount + numOfElements;

				if (pThis->pIterator->iterationValid)
					pThis->pIterator->currentIndex += numOfElements;
			}
		}

		pThis->pListHead = pFirst;
	}
	else
	{
		if (insertIndex == pThis->elementCount)
		{
			pFirst->pPrev = pPrevElement;
			pLast->pNext = pPrevElement->pNext;

			if (pLast->pNext)
				pLast->pNext->pPrev = pLast;

			pPrevElement->pNext = pFirst;

			if (pThis->pIterator)
				pThis->pIterator->elementCount = pThis->elementCount + numOfElements;
		}
		else
		{
			pFirst->pPrev = pInsertElement->pPrev;
			pLast->pNext = pInsertElement;

			pFirst->pPrev->pNext = pFirst;
			pInsertElement->pPrev = pLast;

			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = pThis->elementCount + numOfElements;

				if (insertIndex <= pThis->pIterator->currentIndex)
					pThis->pIterator->currentIndex += numOfElements;
			}
		}
	}

	pThis->elementCount += numOfElements;

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_LinkedListInsertListCopy(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_ CONST CSC_LinkedList* CONST pOther)
{
	LLNode *pFirst, *pLast, *pOld, *pInsertElement, *pPrevElement, *pIterator;
	CSC_SIZE_T iterator;
	CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_LinkedListIsValid(pOther);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis->elementCount + pOther->elementCount > CSC_LINKED_LIST_MAXIMUM_LENGTH)
		return CSC_STATUS_GENERAL_FAILURE;

	if (pOther->elementSize != pThis->elementSize || insertIndex > pThis->elementCount || !pOther->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_AllocatorIsUsable(pThis->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pInsertElement = NULL;
	pPrevElement = NULL;

	if (!insertIndex)
	{
		if (insertIndex != pThis->elementCount)
		{
			pInsertElement = (LLNode*)CSC_LinkedListFront(pThis);

			if (!pInsertElement)
				return CSC_STATUS_GENERAL_FAILURE;
		}
	}
	else
	{
		if (insertIndex == pThis->elementCount)
		{
			pPrevElement = (LLNode*)CSC_LinkedListBack(pThis);

			if (!pPrevElement)
				return CSC_STATUS_GENERAL_FAILURE;
		}
		else
		{
			pInsertElement = (LLNode*)CSC_LinkedListAccessElement(pThis, insertIndex);

			if (!pInsertElement)
				return CSC_STATUS_GENERAL_FAILURE;
		}
	}

	pOld = NULL;
	pFirst = NULL;
	pLast = NULL;
	pIterator = (LLNode*)CSC_LinkedListFront(pOther);

	if (!pIterator)
		return CSC_STATUS_INVALID_PARAMETER;

	for (iterator = (CSC_SIZE_T)0; iterator < pOther->elementCount; ++iterator)
	{
		pLast = (LLNode*)CSC_AllocatorAllocZero(pThis->pAllocator, pThis->elementSize);

		if (CSC_MemoryUtilsCopyMemory((CSC_PVOID)pLast, (CSC_PCVOID)pIterator, pThis->elementSize) != CSC_STATUS_SUCCESS)
		{
			CSC_AllocatorFree(pThis->pAllocator, (CSC_PVOID)pLast);
			pLast = NULL;
		}

		if (!pLast)
		{
			while (pOld)
			{
				pFirst = pOld;
				pOld = pOld->pPrev;

				CSC_AllocatorFree(pThis->pAllocator, (CSC_PVOID)pFirst);
			}

			return CSC_STATUS_GENERAL_FAILURE;
		}

		if (!pFirst)
			pFirst = pLast;

		pLast->pNext = NULL;
		pLast->pPrev = pOld;
		pLast->elementSize = pThis->elementSize;
		pLast->pLL = pThis;

		if (pOld)
			pOld->pNext = pLast;

		pOld = pLast;
		pIterator = pIterator->pNext;
	}

	if (!insertIndex)
	{
		if (insertIndex == pThis->elementCount)
		{
			pLast->pNext = ((pThis->circular) ? pFirst : NULL);
			pFirst->pPrev = ((pThis->circular) ? pLast : NULL);

			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = pThis->elementCount + pOther->elementCount;

				if (pThis->pIterator->iterationValid)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}
		else
		{
			pFirst->pPrev = pInsertElement->pPrev;
			pLast->pNext = pInsertElement;

			if (pFirst->pPrev)
				pFirst->pPrev->pNext = pFirst;

			pInsertElement->pPrev = pLast;

			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = pThis->elementCount + pOther->elementCount;

				if (pThis->pIterator->iterationValid)
					pThis->pIterator->currentIndex += pOther->elementCount;
			}
		}

		pThis->pListHead = pFirst;
	}
	else
	{
		if (insertIndex == pThis->elementCount)
		{
			pFirst->pPrev = pPrevElement;
			pLast->pNext = pPrevElement->pNext;

			if (pLast->pNext)
				pLast->pNext->pPrev = pLast;

			pPrevElement->pNext = pFirst;

			if (pThis->pIterator)
				pThis->pIterator->elementCount = pThis->elementCount + pOther->elementCount;
		}
		else
		{
			pFirst->pPrev = pInsertElement->pPrev;
			pLast->pNext = pInsertElement;

			pFirst->pPrev->pNext = pFirst;
			pInsertElement->pPrev = pLast;

			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = pThis->elementCount + pOther->elementCount;

				if (insertIndex <= pThis->pIterator->currentIndex)
					pThis->pIterator->currentIndex += pOther->elementCount;
			}
		}
	}

	pThis->elementCount += pOther->elementCount;

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_LinkedListInsertListMove(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _Inout_ CSC_LinkedList* CONST pOther)
{
	CSC_STATUS status = CSC_LinkedListIsValid(pOther);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_AllocatorIsUsable(pOther->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_LinkedListInsertListCopy(pThis, insertIndex, pOther);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis == pOther)
		return CSC_STATUS_SUCCESS;

	CSC_LinkedListErase(pOther);

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_LinkedListRemoveElement(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T removeIndex)
{
	LLNode *pRemoveElement, *pPrevElement, *pNextElement;
	CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (removeIndex >= pThis->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_AllocatorIsUsable(pThis->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis->elementCount == (CSC_SIZE_T)1)
	{
		CSC_AllocatorFree(pThis->pAllocator, (CSC_PVOID)pThis->pListHead);

		pThis->elementCount = (CSC_SIZE_T)0;
		pThis->pListHead = NULL;

		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = pThis->elementCount;

			if (pThis->pIterator->iterationValid)
				CSC_IteratorInvalidateIteration(pThis->pIterator);
		}

		return CSC_STATUS_SUCCESS;
	}
	else
	{
		pRemoveElement = CSC_LinkedListAccessElement(pThis, removeIndex);

		if (!pRemoveElement)
			return CSC_STATUS_GENERAL_FAILURE;

		pPrevElement = pRemoveElement->pPrev;
		pNextElement = pRemoveElement->pNext;

		if (pPrevElement)
			pPrevElement->pNext = pNextElement;

		if (pNextElement)
			pNextElement->pPrev = pPrevElement;

		if (removeIndex == (CSC_SIZE_T)0)
			pThis->pListHead = pRemoveElement->pNext;

		CSC_AllocatorFree(pThis->pAllocator, (CSC_PVOID)pRemoveElement);

		--pThis->elementCount;

		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = pThis->elementCount;

			if (pThis->pIterator->iterationValid)
			{
				if (removeIndex == pThis->pIterator->currentIndex)
				{
					CSC_IteratorInvalidateIteration(pThis->pIterator);
				}
				else
				{
					if (removeIndex < pThis->pIterator->currentIndex)
						--pThis->pIterator->currentIndex;
				}
			}
		}

		return CSC_STATUS_SUCCESS;
	}
}

CSC_STATUS CSCMETHOD CSC_LinkedListRemoveRange(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T removeIndex, _In_ CONST CSC_SIZE_T numOfElements)
{
	CSC_SIZE_T iterator;
	LLNode *pFirst, *pLast, *pIterator;
	CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (removeIndex + numOfElements > pThis->elementCount || !numOfElements)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_AllocatorIsUsable(pThis->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pFirst = CSC_LinkedListAccessElement(pThis, removeIndex);
	pLast = CSC_LinkedListAccessElement(pThis, removeIndex + numOfElements - (CSC_SIZE_T)1);

	if (!pFirst || !pLast)
		return CSC_STATUS_GENERAL_FAILURE;

	if (numOfElements == pThis->elementCount)
	{
		pThis->pListHead = NULL;
	}
	else
	{
		if (!removeIndex)
			pThis->pListHead = pLast->pNext;
	}

	pThis->elementCount -= numOfElements;

	if (pFirst->pPrev)
		pFirst->pPrev->pNext = pLast->pNext;

	if (pLast->pNext)
		pLast->pNext->pPrev = pFirst->pPrev;

	pIterator = pFirst;

	for (iterator = (CSC_SIZE_T)0; iterator < numOfElements; ++iterator)
	{
		pFirst = pIterator;
		pIterator = pFirst->pNext;

		CSC_AllocatorFree(pThis->pAllocator, pFirst);
	}

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
			}
		}
	}

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_LinkedListPushElement(_Inout_ CSC_LinkedList* CONST pThis, _In_opt_ CONST CSC_PCVOID pValue)
{
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	return CSC_LinkedListInsertElement(pThis, pThis->elementCount, pValue);
}

CSC_STATUS CSCMETHOD CSC_LinkedListPopElement(_Inout_ CSC_LinkedList* CONST pThis, _Out_opt_ CONST CSC_PVOID pValue)
{
	CSC_PVOID pElement;
	CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pThis->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pValue)
	{
		status = CSC_MemoryUtilsSetZeroMemory(pValue, pThis->elementSize);

		if (status != CSC_STATUS_SUCCESS)
			return status;

		pElement = CSC_LinkedListBack(pThis);

		if (!pElement)
			return CSC_STATUS_GENERAL_FAILURE;

		status = CSC_MemoryUtilsCopyMemory(pValue, (CSC_PCVOID)pElement, pThis->elementSize);

		if (status != CSC_STATUS_SUCCESS)
			return status;
	}

	status = CSC_LinkedListRemoveElement(pThis, pThis->elementCount - (CSC_SIZE_T)1);

	if (pValue && status != CSC_STATUS_SUCCESS)
		CSC_MemoryUtilsSetZeroMemory(pValue, pThis->elementSize);

	return status;
}


static CSC_STATUS CSCMETHOD CSC_LinkedListRotateImpl(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T rotationCount, _In_ CONST CSC_BOOLEAN forward)
{
	CSC_SIZE_T iterator, effectiveRotationCount;
	LLNode* pIterator;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis->elementCount <= (CSC_SIZE_T)1 || !pThis->circular || !rotationCount)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator = pThis->pListHead;

	if (!pIterator)
		return CSC_STATUS_GENERAL_FAILURE;

	effectiveRotationCount = rotationCount % pThis->elementCount;

	if (!effectiveRotationCount)
		return CSC_STATUS_SUCCESS;

	for (iterator = (CSC_SIZE_T)0; iterator < effectiveRotationCount; ++iterator)
	{
		pIterator = ((forward) ? pIterator->pNext : pIterator->pPrev);

		if (!pIterator)
			return CSC_STATUS_GENERAL_FAILURE;
	}

	pThis->pListHead = pIterator;

	if (pThis->pIterator)
	{
		if (forward)
			pThis->pIterator->currentIndex = (pThis->pIterator->currentIndex + effectiveRotationCount) % pThis->elementCount;
		else
			pThis->pIterator->currentIndex = pThis->elementCount - (((pThis->elementCount - pThis->pIterator->currentIndex - (CSC_SIZE_T)1) + effectiveRotationCount) % pThis->elementCount) - (CSC_SIZE_T)1;
	}

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_LinkedListRotateForward(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T rotationCount)
{
	return CSC_LinkedListRotateImpl(pThis, rotationCount, (CSC_BOOLEAN)TRUE);
}

CSC_STATUS CSCMETHOD CSC_LinkedListRotateForwardByOne(_Inout_ CSC_LinkedList* CONST pThis)
{
	return CSC_LinkedListRotateForward(pThis, (CSC_SIZE_T)1);
}

CSC_STATUS CSCMETHOD CSC_LinkedListRotateBackward(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T rotationCount)
{
	return CSC_LinkedListRotateImpl(pThis, rotationCount, (CSC_BOOLEAN)FALSE);
}

CSC_STATUS CSCMETHOD CSC_LinkedListRotateBackwardByOne(_Inout_ CSC_LinkedList* CONST pThis)
{
	return CSC_LinkedListRotateBackward(pThis, (CSC_SIZE_T)1);
}

CSC_STATUS CSCMETHOD CSC_LinkedListRotateToIndex(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T newHeadIndex)
{
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pThis->circular || newHeadIndex >= pThis->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	if (!newHeadIndex)
		return CSC_STATUS_SUCCESS;

	if (newHeadIndex > (pThis->elementCount >> (CSC_SIZE_T)1))
		return CSC_LinkedListRotateForward(pThis, newHeadIndex);

	return CSC_LinkedListRotateBackward(pThis, pThis->elementCount - newHeadIndex);
}


CSC_STATUS CSCMETHOD CSC_LinkedListOpen(_Inout_ CSC_LinkedList* CONST pThis)
{
	LLNode *pFirst, *pLast;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pThis->circular)
		return CSC_STATUS_SUCCESS;

	if (pThis->elementCount)
	{
		pFirst = CSC_LinkedListFront(pThis);
		pLast = CSC_LinkedListBack(pThis);

		if (!pFirst || !pLast)
			return CSC_STATUS_GENERAL_FAILURE;

		pFirst->pPrev = NULL;
		pLast->pNext = NULL;
	}

	pThis->circular = (CSC_BOOLEAN)FALSE;

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_LinkedListClose(_Inout_ CSC_LinkedList* CONST pThis)
{
	LLNode *pFirst, *pLast;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis->circular)
		return CSC_STATUS_SUCCESS;

	if (pThis->elementCount)
	{
		pFirst = CSC_LinkedListFront(pThis);
		pLast = CSC_LinkedListBack(pThis);

		if (!pFirst || !pLast)
			return CSC_STATUS_GENERAL_FAILURE;

		pFirst->pPrev = pLast;
		pLast->pNext = pFirst;
	}

	pThis->circular = (CSC_BOOLEAN)TRUE;

	return CSC_STATUS_SUCCESS;
}


CSC_PVOID CSCMETHOD CSC_LinkedListAccessElement(_In_ CONST CSC_LinkedList* CONST pThis, _In_ CONST CSC_SIZE_T index)
{
	CSC_SIZE_T iterator;
	LLNode* pNode;

	if (CSC_LinkedListIsValid(pThis) != CSC_STATUS_SUCCESS || index >= pThis->elementCount || !pThis->pListHead)
		return NULL;

	pNode = pThis->pListHead;

	if (pThis->circular && index > (pThis->elementCount >> (CSC_SIZE_T)1))
	{
		for (iterator = (CSC_SIZE_T)0; iterator < (pThis->elementCount - index); ++iterator)
		{
			if (!pNode)
				return NULL;

			pNode = pNode->pPrev;
		}
	}
	else
	{
		for (iterator = (CSC_SIZE_T)0; iterator < index; ++iterator)
		{
			if (!pNode)
				return NULL;

			pNode = pNode->pNext;
		}
	}

	return (CSC_PVOID)pNode;
}


CSC_STATUS CSCMETHOD CSC_LinkedListReverse(_Inout_ CSC_LinkedList* CONST pThis)
{
	CSC_SIZE_T iterator;
	LLNode *pNode, *pNodeBuffer;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pThis->pListHead)
		return CSC_STATUS_INVALID_PARAMETER;

	pNode = pThis->pListHead;
	pThis->pListHead = CSC_LinkedListBack(pThis);

	if (!pThis->pListHead)
	{
		pThis->pListHead = pNode;
		return CSC_STATUS_GENERAL_FAILURE;
	}

	if (pThis->pIterator && pThis->pIterator->iterationValid)
		CSC_IteratorInvalidateIteration(pThis->pIterator);

	for (iterator = (CSC_SIZE_T)0; iterator < pThis->elementCount; ++iterator)
	{
		if (!pNode)
		{
			CSC_IteratorUnregister(pThis->pIterator);
			return CSC_STATUS_GENERAL_FAILURE;
		}

		pNodeBuffer = pNode->pNext;
		pNode->pNext = pNode->pPrev;
		pNode->pPrev = pNodeBuffer;

		pNode = pNode->pPrev;
	}

	return CSC_STATUS_SUCCESS;
}


CSC_PVOID CSCMETHOD CSC_LinkedListFront(_In_ CONST CSC_LinkedList* CONST pThis)
{
	if (CSC_LinkedListIsValid(pThis) != CSC_STATUS_SUCCESS || !pThis->elementCount)
		return NULL;

	return pThis->pListHead;
}

CSC_PVOID CSCMETHOD CSC_LinkedListBack(_In_ CONST CSC_LinkedList* CONST pThis)
{
	if (CSC_LinkedListIsValid(pThis) != CSC_STATUS_SUCCESS || !pThis->elementCount)
		return NULL;

	return CSC_LinkedListAccessElement(pThis, pThis->elementCount - (CSC_SIZE_T)1);
}

CSC_PVOID CSCMETHOD CSC_LinkedListData(_In_ CONST CSC_LinkedList* CONST pThis)
{
	return CSC_LinkedListFront(pThis);
}


CSC_STATUS CSCMETHOD CSC_LinkedListAppendCopy(_Inout_ CSC_LinkedList* CONST pThis, _In_ CONST CSC_LinkedList* CONST pOther)
{
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	return CSC_LinkedListInsertListCopy(pThis, pThis->elementCount, pOther);
}

CSC_STATUS CSCMETHOD CSC_LinkedListAppendMove(_Inout_ CSC_LinkedList* CONST pThis, _Inout_ CSC_LinkedList* CONST pOther)
{
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	return CSC_LinkedListInsertListMove(pThis, pThis->elementCount, pOther);
}


CSC_STATUS CSCMETHOD CSC_LinkedListDestroy(_Inout_ CSC_LinkedList* CONST pThis)
{
	CSC_STATUS status = CSC_LinkedListErase(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis->pIterator)
	{
		status = CSC_IteratorUnregister(pThis->pIterator);

		if (status != CSC_STATUS_SUCCESS)
			return status;
	}

	return CSC_LinkedListZeroMemory(pThis);
}

CSC_STATUS CSCMETHOD CSC_LinkedListErase(_Inout_ CSC_LinkedList* CONST pThis)
{
	return CSC_LinkedListRemoveRange(pThis, (CSC_SIZE_T)0, pThis->elementCount);
}

CSC_STATUS CSCMETHOD CSC_LinkedListZeroMemory(_Out_ CSC_LinkedList* CONST pThis)
{
	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	pThis->pVTableIterator = NULL;
	pThis->pAllocator = NULL;
	pThis->pListHead = NULL;
	pThis->pIterator = NULL;
	pThis->elementCount = (CSC_SIZE_T)0;
	pThis->elementSize = (CSC_SIZE_T)0;
	pThis->circular = (CSC_BOOLEAN)FALSE;

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_LinkedListIsEmpty(_In_ CONST CSC_LinkedList* CONST pThis)
{
	return ((CSC_LinkedListIsValid(pThis) == CSC_STATUS_SUCCESS && pThis->elementCount && pThis->pListHead) ? CSC_STATUS_GENERAL_FAILURE : CSC_STATUS_SUCCESS);
}

CSC_STATUS CSCMETHOD CSC_LinkedListIsValid(_In_ CONST CSC_LinkedList* CONST pThis)
{
	return (!pThis || pThis->elementSize <= sizeof(LLNode) || pThis->elementSize > CSC_LINKED_LIST_MAXIMUM_ELEMENT_SIZE || pThis->pVTableIterator != (CSC_PCVOID)g_linkedListVirtualTable || pThis->elementCount > CSC_LINKED_LIST_MAXIMUM_LENGTH) ? CSC_STATUS_INVALID_PARAMETER : CSC_STATUS_SUCCESS;
}


CSC_SIZE_T CSCMETHOD CSC_LinkedListGetSize(_In_ CONST CSC_LinkedList* CONST pThis)
{
	return ((CSC_LinkedListIsValid(pThis) == CSC_STATUS_SUCCESS && pThis->pListHead) ? pThis->elementCount : (CSC_SIZE_T)0);
}

CSC_SIZE_T CSCMETHOD CSC_LinkedListGetMaxElements(_In_ CONST CSC_LinkedList* CONST pThis)
{
	return ((CSC_LinkedListIsValid(pThis) == CSC_STATUS_SUCCESS) ? CSC_LINKED_LIST_MAXIMUM_LENGTH : (CSC_SIZE_T)0);
}

CSC_SIZE_T CSCMETHOD CSC_LinkedListGetElementSize(_In_ CONST CSC_LinkedList* CONST pThis)
{
	return ((CSC_LinkedListIsValid(pThis) == CSC_STATUS_SUCCESS) ? pThis->elementSize : (CSC_SIZE_T)0);
}

CSC_STATUS CSCMETHOD CSC_LinkedListGetCircular(_In_ CONST CSC_LinkedList* CONST pThis)
{
	return ((CSC_LinkedListIsValid(pThis) == CSC_STATUS_SUCCESS && pThis->circular) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_PVOID CSCMETHOD CSC_LinkedListGetAllocator(_In_ CONST CSC_LinkedList* CONST pThis)
{
	return ((CSC_LinkedListIsValid(pThis) == CSC_STATUS_SUCCESS) ? pThis->pAllocator : NULL);
}


static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorRegister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_LinkedList* CONST pLL = (CSC_LinkedList* CONST)pContainer;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pLL);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator->pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pLL->pIterator)
		return CSC_STATUS_GENERAL_FAILURE;

	pIterator->elementSize = pLL->elementSize;
	pIterator->elementCount = pLL->elementCount;
	pIterator->swapByIndex = (CSC_BOOLEAN)FALSE;
	pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
	pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	pIterator->pCurrElement = NULL;
	pIterator->pContainer = pContainer;

	pLL->pIterator = pIterator;

	return CSC_STATUS_SUCCESS;
}

static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorUnregister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_LinkedList* CONST pLL = (CSC_LinkedList* CONST)pContainer;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pLL);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pLL->pIterator != pIterator)
		return CSC_STATUS_GENERAL_FAILURE;

	pLL->pIterator = NULL;

	CSC_IteratorZeroMemory(pIterator);

	return CSC_STATUS_SUCCESS;
}

static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorFirstElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CONST CSC_LinkedList* CONST pLL = (CONST CSC_LinkedList* CONST)pContainer;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pLL);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pLL->pIterator || pIterator->elementSize != pLL->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pLL->elementCount;

	if (!pLL->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->pCurrElement = CSC_LinkedListFront(pLL);

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

static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorNextElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CONST CSC_LinkedList* CONST pLL = (CONST CSC_LinkedList* CONST)pContainer;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pLL);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pLL->pIterator || pIterator->elementSize != pLL->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pLL->elementCount;

	if (!pIterator->iterationValid)
		return CSC_STATUS_INVALID_PARAMETER;

	if (!pLL->elementCount || pIterator->currentIndex >= pLL->elementCount || ((CONST LLNode * CONST)pIterator->pCurrElement)->pLL != pLL)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return CSC_STATUS_INVALID_PARAMETER;
	}	

	++pIterator->currentIndex;

	if (pIterator->currentIndex >= pLL->elementCount)
	{
		if (pLL->circular)
		{
			pIterator->currentIndex %= pLL->elementCount;
		}
		else
		{
			CSC_IteratorInvalidateIteration(pIterator);
			return CSC_STATUS_GENERAL_FAILURE;
		}
	}

	pIterator->pCurrElement = (CSC_PVOID)((CONST LLNode* CONST)pIterator->pCurrElement)->pNext;

	if (!pIterator->pCurrElement)
	{
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorLastElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T iterator;
	CONST CSC_LinkedList* CONST pLL = (CONST CSC_LinkedList* CONST)pContainer;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pLL);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pLL->pIterator || pIterator->elementSize != pLL->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pLL->elementCount;

	if (!pLL->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pIterator->iterationValid && pIterator->currentIndex < pLL->elementCount && !pLL->circular && ((CONST LLNode* CONST)pIterator->pCurrElement)->pLL == pLL)
	{
		for (iterator = (CSC_SIZE_T)0; iterator < pLL->elementCount - pIterator->currentIndex - (CSC_SIZE_T)1; ++iterator)
		{
			pIterator->pCurrElement = (CSC_PVOID)((CONST LLNode* CONST)pIterator->pCurrElement)->pNext;

			if (!pIterator->pCurrElement)
			{
				CSC_IteratorInvalidateIteration(pIterator);
				break;
			}
		}

		if (pIterator->iterationValid)
		{
			pIterator->currentIndex = pLL->elementCount - (CSC_SIZE_T)1;
			return CSC_STATUS_SUCCESS;
		}
	}

	pIterator->pCurrElement = CSC_LinkedListBack(pLL);

	if (pIterator->pCurrElement)
	{
		pIterator->iterationValid = (CSC_BOOLEAN)TRUE;
		pIterator->currentIndex = pLL->elementCount - (CSC_SIZE_T)1;
	}
	else
	{
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorPreviousElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CONST CSC_LinkedList* CONST pLL = (CONST CSC_LinkedList* CONST)pContainer;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pLL);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pLL->pIterator || pIterator->elementSize != pLL->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pLL->elementCount;

	if (!pIterator->iterationValid)
		return CSC_STATUS_INVALID_PARAMETER;

	if (!pLL->elementCount || pIterator->currentIndex >= pLL->elementCount || ((CONST LLNode * CONST)pIterator->pCurrElement)->pLL != pLL)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return CSC_STATUS_INVALID_PARAMETER;
	}

	if (!pIterator->currentIndex)
	{
		if (pLL->circular)
		{
			pIterator->currentIndex = pLL->elementCount - (CSC_SIZE_T)1;
		}
		else
		{
			CSC_IteratorInvalidateIteration(pIterator);
			return CSC_STATUS_GENERAL_FAILURE;
		}
	}
	else
	{
		--pIterator->currentIndex;
	}
	
	pIterator->pCurrElement = (CSC_PVOID)((CONST LLNode* CONST)pIterator->pCurrElement)->pPrev;

	if (!pIterator->pCurrElement)
	{
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorMoveToIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index)
{
	CONST CSC_LinkedList* CONST pLL = (CONST CSC_LinkedList* CONST)pContainer;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pLL);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pLL->pIterator || pIterator->elementSize != pLL->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pLL->elementCount;

	if (!pLL->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->pCurrElement = CSC_LinkedListIteratorGetElementAt(pContainer, pIterator, index);

	if (!pIterator->pCurrElement)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return CSC_STATUS_GENERAL_FAILURE;
	}

	pIterator->currentIndex = index;
	pIterator->iterationValid = (CSC_BOOLEAN)TRUE;

	return CSC_STATUS_SUCCESS;
}

static CSC_SIZE_T CSCMETHOD CSC_LinkedListIteratorGetCurrentElementIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CONST CSC_LinkedList* CONST pLL = (CONST CSC_LinkedList* CONST)pContainer;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pLL);

	if (status != CSC_STATUS_SUCCESS)
		return CSC_ITERATOR_INVALID_INDEX;

	if (!pIterator || pIterator != pLL->pIterator || pIterator->elementSize != pLL->elementSize || pIterator->pContainer != pContainer)
		return CSC_ITERATOR_INVALID_INDEX;

	pIterator->elementCount = pLL->elementCount;

	if (!pIterator->iterationValid)
		return CSC_ITERATOR_INVALID_INDEX;

	if (!pLL->elementCount || pIterator->currentIndex >= pLL->elementCount || ((CONST LLNode* CONST)pIterator->pCurrElement)->pLL != pLL)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return CSC_ITERATOR_INVALID_INDEX;
	}

	return pIterator->currentIndex;
}

static CSC_PVOID CSCMETHOD CSC_LinkedListIteratorGetCurrentElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CONST CSC_LinkedList* CONST pLL = (CONST CSC_LinkedList* CONST)pContainer;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pLL);

	if (status != CSC_STATUS_SUCCESS)
		return NULL;

	if (!pIterator || pIterator != pLL->pIterator || pIterator->elementSize != pLL->elementSize || pIterator->pContainer != pContainer)
		return NULL;

	pIterator->elementCount = pLL->elementCount;

	if (!pIterator->iterationValid)
		return NULL;

	if (!pLL->elementCount || pIterator->currentIndex >= pLL->elementCount || ((CONST LLNode* CONST)pIterator->pCurrElement)->pLL != pLL)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return NULL;
	}

	return pIterator->pCurrElement;
}

static CSC_PVOID CSCMETHOD CSC_LinkedListIteratorGetElementAt(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index)
{
	CSC_PVOID pCurrElement;
	CSC_BOOLEAN forwardFromCurrElement;
	CSC_SIZE_T iterator, distanceToHead, distanceToCurrElement;
	CONST CSC_LinkedList* CONST pLL = (CONST CSC_LinkedList* CONST)pContainer;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pLL);

	if (status != CSC_STATUS_SUCCESS)
		return NULL;

	if (!pIterator || pIterator != pLL->pIterator || pIterator->elementSize != pLL->elementSize || pIterator->pContainer != pContainer)
		return NULL;

	pIterator->elementCount = pLL->elementCount;

	if (!pLL->elementCount || index >= pLL->elementCount)
		return NULL;

	if (pIterator->iterationValid && pIterator->currentIndex < pLL->elementCount && ((CONST LLNode* CONST)pIterator->pCurrElement)->pLL == pLL)
	{
		forwardFromCurrElement = ((index > pIterator->currentIndex) ? (CSC_BOOLEAN)TRUE : (CSC_BOOLEAN)FALSE);
		distanceToCurrElement = ((forwardFromCurrElement) ? index - pIterator->currentIndex : pIterator->currentIndex - index);

		if (pLL->circular)
		{
			distanceToHead = ((index > (pLL->elementCount >> (CSC_SIZE_T)1)) ? pLL->elementCount - index : index);

			if (distanceToCurrElement > (pLL->elementCount >> (CSC_SIZE_T)1))
			{
				distanceToCurrElement = pLL->elementCount - distanceToCurrElement;
				forwardFromCurrElement = ((forwardFromCurrElement) ? (CSC_BOOLEAN)FALSE : (CSC_BOOLEAN)TRUE);
			}
		}
		else
		{
			distanceToHead = index;
		}

		if (distanceToCurrElement < distanceToHead)
		{
			pCurrElement = pIterator->pCurrElement;

			for (iterator = (CSC_SIZE_T)0; iterator < distanceToCurrElement; ++iterator)
			{
				pCurrElement = (CSC_PVOID)((forwardFromCurrElement) ? ((CONST LLNode* CONST)pCurrElement)->pNext : ((CONST LLNode* CONST)pCurrElement)->pPrev);

				if (!pCurrElement)
					break;
			}

			if (pCurrElement)
				return pCurrElement;
		}
	}

	return CSC_LinkedListAccessElement(pLL, index);
}

static CSC_SIZE_T CSCMETHOD CSC_LinkedListIteratorGetElementCount(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CONST CSC_LinkedList* CONST pLL = (CONST CSC_LinkedList* CONST)pContainer;
	CONST CSC_STATUS status = CSC_LinkedListIsValid(pLL);

	if (status != CSC_STATUS_SUCCESS)
		return CSC_ITERATOR_INVALID_LENGTH;

	if (!pIterator || pIterator != pLL->pIterator || pIterator->elementSize != pLL->elementSize || pIterator->pContainer != pContainer)
		return CSC_ITERATOR_INVALID_LENGTH;

	pIterator->elementCount = pLL->elementCount;

	return pLL->elementCount;
}

static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorSwapElementsByIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T secondIndex)
{
	CSC_PVOID pFirstElement, pSecondElement;
	CONST CSC_LinkedList* CONST pLL = (CONST CSC_LinkedList* CONST)pContainer;
	CSC_STATUS status = CSC_LinkedListIsValid(pLL);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pLL->pIterator || pIterator->elementSize != pLL->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pLL->elementCount;

	if (!pLL->elementCount || firstIndex >= pLL->elementCount || secondIndex >= pLL->elementCount)
		return CSC_STATUS_INVALID_PARAMETER;

	if (firstIndex == secondIndex)
		return CSC_STATUS_SUCCESS;

	status = CSC_AllocatorIsUsable(pLL->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pFirstElement = CSC_LinkedListIteratorGetElementAt(pContainer, pIterator, firstIndex);
	pSecondElement = CSC_LinkedListIteratorGetElementAt(pContainer, pIterator, secondIndex);

	if (!pFirstElement || !pSecondElement)
		return CSC_STATUS_GENERAL_FAILURE;

	return CSC_MemoryUtilsSwapValues((CSC_PVOID)((CONST CSC_BYTE* CONST)pFirstElement + sizeof(LLNode)), (CSC_PVOID)((CONST CSC_BYTE* CONST)pSecondElement + sizeof(LLNode)), pLL->elementSize - sizeof(LLNode), pLL->pAllocator);
}

static CSC_STATUS CSCMETHOD CSC_LinkedListIteratorSwapElementsByPtr(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond)
{
	CONST CSC_LinkedList* CONST pLL = (CONST CSC_LinkedList* CONST)pContainer;
	CSC_STATUS status = CSC_LinkedListIsValid(pLL);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator || pIterator != pLL->pIterator || pIterator->elementSize != pLL->elementSize || pIterator->pContainer != pContainer)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = pLL->elementCount;

	if (!pLL->elementCount || !pFirst || !pSecond || ((CONST LLNode* CONST)pFirst)->pLL != pLL || ((CONST LLNode* CONST)pSecond)->pLL != pLL)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pFirst == pSecond)
		return CSC_STATUS_SUCCESS;

	status = CSC_AllocatorIsUsable(pLL->pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	return CSC_MemoryUtilsSwapValues((CSC_PVOID)((CONST CSC_BYTE* CONST)pFirst + sizeof(LLNode)), (CSC_PVOID)((CONST CSC_BYTE* CONST)pSecond + sizeof(LLNode)), pLL->elementSize - sizeof(LLNode), pLL->pAllocator);
}
