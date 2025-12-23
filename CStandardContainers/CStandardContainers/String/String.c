#include "String.h"
#include "../Allocator/Allocator.h"
#include "../Utils/MemoryUtils.h"

static CSC_STATUS CSCMETHOD CSC_AStringIteratorRegister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_AStringIteratorUnregister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_AStringIteratorFirstElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_AStringIteratorNextElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_AStringIteratorLastElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_AStringIteratorPreviousElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_AStringIteratorMoveToIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index);
static CSC_SIZE_T CSCMETHOD CSC_AStringIteratorGetCurrentElementIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_PVOID CSCMETHOD CSC_AStringIteratorGetCurrentElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_PVOID CSCMETHOD CSC_AStringIteratorGetElementAt(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index);
static CSC_SIZE_T CSCMETHOD CSC_AStringIteratorGetElementCount(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_AStringIteratorSwapElementsByIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T secondIndex);
static CSC_STATUS CSCMETHOD CSC_AStringIteratorSwapElementsByPtr(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond);

static CONST CSC_PCVOID g_aStringVirtualTable[csc_vfn_it_num_of_entries] =
{
	(CSC_PCVOID)CSC_AStringIteratorRegister,
	(CSC_PCVOID)CSC_AStringIteratorUnregister,
	(CSC_PCVOID)CSC_AStringIteratorFirstElement,
	(CSC_PCVOID)CSC_AStringIteratorNextElement,
	(CSC_PCVOID)CSC_AStringIteratorLastElement,
	(CSC_PCVOID)CSC_AStringIteratorPreviousElement,
	(CSC_PCVOID)CSC_AStringIteratorMoveToIndex,
	(CSC_PCVOID)CSC_AStringIteratorGetCurrentElementIndex,
	(CSC_PCVOID)CSC_AStringIteratorGetCurrentElement,
	(CSC_PCVOID)CSC_AStringIteratorGetElementAt,
	(CSC_PCVOID)CSC_AStringIteratorGetElementCount,
	(CSC_PCVOID)CSC_AStringIteratorSwapElementsByIndex,
	(CSC_PCVOID)CSC_AStringIteratorSwapElementsByPtr
};

static CSC_STATUS CSCMETHOD CSC_WStringIteratorRegister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_WStringIteratorUnregister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_WStringIteratorFirstElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_WStringIteratorNextElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_WStringIteratorLastElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_WStringIteratorPreviousElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_WStringIteratorMoveToIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index);
static CSC_SIZE_T CSCMETHOD CSC_WStringIteratorGetCurrentElementIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_PVOID CSCMETHOD CSC_WStringIteratorGetCurrentElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_PVOID CSCMETHOD CSC_WStringIteratorGetElementAt(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index);
static CSC_SIZE_T CSCMETHOD CSC_WStringIteratorGetElementCount(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD CSC_WStringIteratorSwapElementsByIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T secondIndex);
static CSC_STATUS CSCMETHOD CSC_WStringIteratorSwapElementsByPtr(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond);

static CONST CSC_PCVOID g_wStringVirtualTable[csc_vfn_it_num_of_entries] =
{
	(CSC_PCVOID)CSC_WStringIteratorRegister,
	(CSC_PCVOID)CSC_WStringIteratorUnregister,
	(CSC_PCVOID)CSC_WStringIteratorFirstElement,
	(CSC_PCVOID)CSC_WStringIteratorNextElement,
	(CSC_PCVOID)CSC_WStringIteratorLastElement,
	(CSC_PCVOID)CSC_WStringIteratorPreviousElement,
	(CSC_PCVOID)CSC_WStringIteratorMoveToIndex,
	(CSC_PCVOID)CSC_WStringIteratorGetCurrentElementIndex,
	(CSC_PCVOID)CSC_WStringIteratorGetCurrentElement,
	(CSC_PCVOID)CSC_WStringIteratorGetElementAt,
	(CSC_PCVOID)CSC_WStringIteratorGetElementCount,
	(CSC_PCVOID)CSC_WStringIteratorSwapElementsByIndex,
	(CSC_PCVOID)CSC_WStringIteratorSwapElementsByPtr
};

static CSC_STATUS CSCMETHOD WStringIteratorInitialize(_In_ CONST CSC_PCVOID pContainer, _Out_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD WStringIteratorFirstElement(_In_ CONST CSC_PCVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD WStringIteratorNextElement(_In_ CONST CSC_PCVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD WStringIteratorLastElement(_In_ CONST CSC_PCVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD WStringIteratorPreviousElement(_In_ CONST CSC_PCVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
static CSC_STATUS CSCMETHOD WStringIteratorSwapElements(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CONST CSC_PVOID pFirstElement, _Inout_ CONST CSC_PVOID pSecondElement);


static CSC_STATUS CSCAPI CSC_AStringIsValidSequence(_In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements);

static CSC_STATUS CSCMETHOD CSC_AStringInitializeSequenceImpl(_Out_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PVOID pAllocator, _In_ CONST CSC_BOOLEAN validate);
static CSC_STATUS CSCMETHOD CSC_AStringAssignSequenceImpl(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_BOOLEAN validate);
static CSC_STATUS CSCMETHOD CSC_AStringReplaceSequenceImpl(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T replaceIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_BOOLEAN validate);
static CSC_STATUS CSCMETHOD CSC_AStringInsertSequenceImpl(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_BOOLEAN validate);
static CSC_SIZE_T CSCMETHOD CSC_AStringFindSequenceImpl(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_BOOLEAN reverseDir, _In_ CONST CSC_BOOLEAN reverseIndexDir, _In_ CONST CSC_BOOLEAN validate);
static CSC_SIZE_T CSCMETHOD CSC_AStringRFindSequenceImpl(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_BOOLEAN validate);
static CSC_STATUS CSCMETHOD CSC_AStringEndsWithSequenceImpl(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_BOOLEAN validate);
static CSC_STATUS CSCMETHOD CSC_AStringCompareWithSequenceImpl(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_BOOLEAN validate);


static CSC_STATUS CSCAPI CSC_WStringIsValidSequence(_In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _When_(return == CSC_STATUS_SUCCESS, _Out_opt_) CSC_SIZE_T* CONST pCodePointLength);

static CSC_STATUS CSCMETHOD CSC_WStringInitializeSequenceImpl(_Out_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PVOID pAllocator, _In_ CONST CSC_BOOLEAN validate);
static CSC_WCHAR* CSCMETHOD CSC_WStringLazyAccessCodePointImpl(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T codePointIndex, _When_(return != NULL, _Out_opt_) CSC_SIZE_T* CONST pArrayCharIndex);
static CSC_STATUS CSCMETHOD CSC_WStringAssignSequenceImpl(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_BOOLEAN validate);
static CSC_STATUS CSCMETHOD CSC_WStringInsertSequenceImpl(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T insertCodePointIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_BOOLEAN validate);
static CSC_SIZE_T CSCMETHOD CSC_WStringFindSequenceImpl(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_BOOLEAN reverseDir, _In_ CONST CSC_BOOLEAN reverseIndexDir);


static CSC_STATUS CSCAPI CSC_AStringIsValidSequence(_In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements)
{
	CSC_SIZE_T iterator;

	if (!pArray || !numOfElements || numOfElements > CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING)
		return CSC_STATUS_INVALID_PARAMETER;

	for (iterator = (CSC_SIZE_T)0; iterator < numOfElements; ++iterator)
	{
		if (!pArray[iterator])
			return CSC_STATUS_INVALID_PARAMETER;
	}

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_AStringInitialize(_Out_ CSC_AString* CONST pThis, _In_ CONST CSC_PVOID pAllocator)
{
	CSC_STATUS status;
	CONST CSC_CHAR nullTerminator = (CSC_CHAR)0;

	if (!pThis || !pAllocator)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_AStringZeroMemory(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayInitializeWithValue(&pThis->dynamicArray, sizeof(CSC_CHAR), (CSC_SIZE_T)1, (CSC_PCVOID)&nullTerminator, pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pThis->pVTableIterator = (CSC_PCVOID)g_aStringVirtualTable;

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_AStringInitializeSequence(_Out_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PVOID pAllocator)
{
	return CSC_AStringInitializeSequenceImpl(pThis, pArray, numOfElements, pAllocator, (CSC_BOOLEAN)TRUE);
}

static CSC_STATUS CSCMETHOD CSC_AStringInitializeSequenceImpl(_Out_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PVOID pAllocator, _In_ CONST CSC_BOOLEAN validate)
{
	CSC_STATUS status = CSC_AStringInitialize(pThis, pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (validate)
	{
		if (CSC_AStringIsValidSequence(pArray, numOfElements) != CSC_STATUS_SUCCESS)
		{
			CSC_AStringDestroy(pThis);
			return CSC_STATUS_INVALID_PARAMETER;
		}
	}

	status = CSC_DynamicArrayInsertRange(&pThis->dynamicArray, (CSC_SIZE_T)0, (CSC_PCVOID)pArray, numOfElements);

	if (status != CSC_STATUS_SUCCESS)
		CSC_AStringDestroy(pThis);
	else
		pThis->cachedLength = numOfElements;

	return status;
}

CSC_STATUS CSCMETHOD CSC_AStringInitializeCString(_Out_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr, _In_ CONST CSC_PVOID pAllocator)
{
	CONST CSC_SIZE_T length = CSC_StringUtilsStrLenAnsiString(pCStr, CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING, (CSC_BOOLEAN)TRUE);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringInitializeSequenceImpl(pThis, pCStr, length, pAllocator, (CSC_BOOLEAN)FALSE);
}

CSC_STATUS CSCMETHOD CSC_AStringInitializeCopy(_Out_ CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther, _In_ CONST CSC_PVOID pAllocator)
{
	CSC_STATUS status = CSC_AStringInitialize(pThis, pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_AStringCopy(pThis, pOther);

	if (status != CSC_STATUS_SUCCESS)
		CSC_AStringDestroy(pThis);

	return status;
}


CSC_STATUS CSCMETHOD CSC_AStringReserve(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements)
{
	CSC_SIZE_T arrayStringLength;
	CSC_STATUS status = CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (numOfElements > CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayReserve(&pThis->dynamicArray, numOfElements + (CSC_SIZE_T)1);
	
	if (status == CSC_STATUS_SUCCESS && pThis->pIterator)
	{
		if (pThis->cachedLength == CSC_STRING_INVALID_LENGTH)
		{
			arrayStringLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1;
			pThis->cachedLength = CSC_StringUtilsStrLenAnsiString((CSC_CHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), arrayStringLength, (CSC_BOOLEAN)TRUE);
		}

		pThis->pIterator->elementCount = pThis->cachedLength;

		if (pThis->pIterator->iterationValid)
		{
			if (pThis->cachedLength == CSC_STRING_INVALID_LENGTH)
			{
				CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
			else
			{
				pThis->pIterator->pCurrElement = CSC_AStringLazyAccessElement(pThis, pThis->pIterator->currentIndex);

				if (!pThis->pIterator->pCurrElement)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_AStringShrinkToFit(_Inout_ CSC_AString* CONST pThis)
{
	CSC_SIZE_T arrayStringLength;
	CSC_STATUS status = CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayShrinkToFit(&pThis->dynamicArray);

	if (status == CSC_STATUS_SUCCESS && pThis->pIterator)
	{
		if (pThis->cachedLength == CSC_STRING_INVALID_LENGTH)
		{
			arrayStringLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1;
			pThis->cachedLength = CSC_StringUtilsStrLenAnsiString((CSC_CHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), arrayStringLength, (CSC_BOOLEAN)TRUE);
		}

		pThis->pIterator->elementCount = pThis->cachedLength;

		if (pThis->pIterator->iterationValid)
		{
			if (pThis->cachedLength == CSC_STRING_INVALID_LENGTH)
			{
				CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
			else
			{
				pThis->pIterator->pCurrElement = CSC_AStringLazyAccessElement(pThis, pThis->pIterator->currentIndex);

				if (!pThis->pIterator->pCurrElement)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_AStringRepair(_Inout_ CSC_AString* CONST pThis)
{
	CSC_STATUS status;
	CSC_SIZE_T arrayLength, length, arrayStringLength;

	if (!pThis || pThis->pVTableIterator != (CSC_PCVOID)g_aStringVirtualTable || CSC_DynamicArrayGetElementSize(&pThis->dynamicArray) != sizeof(CSC_CHAR))
		return CSC_STATUS_INVALID_PARAMETER;

	arrayLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray);

	if (!arrayLength || arrayLength - (CSC_SIZE_T)1 > CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING)
	{
		if (arrayLength)
		{
			status = CSC_DynamicArrayErase(&pThis->dynamicArray);

			if (status != CSC_STATUS_SUCCESS)
			{
				pThis->cachedLength = CSC_STRING_INVALID_LENGTH;

				if (pThis->pIterator)
				{
					if (pThis->pIterator->iterationValid)
						CSC_IteratorInvalidateIteration(pThis->pIterator);

					pThis->pIterator->elementCount = pThis->cachedLength;
				}

				return status;
			}
		}

		arrayStringLength = CSC_STRING_INVALID_LENGTH;
		length = CSC_STRING_INVALID_LENGTH;
	}
	else
	{
		arrayStringLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1;
		length = CSC_StringUtilsStrLenAnsiString((CSC_CHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), arrayStringLength, (CSC_BOOLEAN)TRUE);

		if (length == arrayStringLength)
		{
			pThis->cachedLength = length;

			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = pThis->cachedLength;

				if (pThis->pIterator->iterationValid)
				{
					pThis->pIterator->pCurrElement = CSC_AStringLazyAccessElement(pThis, pThis->pIterator->currentIndex);

					if (!pThis->pIterator->pCurrElement)
						CSC_IteratorInvalidateIteration(pThis->pIterator);
				}
			}

			return CSC_STATUS_SUCCESS;
		}
	}

	pThis->cachedLength = CSC_STRING_INVALID_LENGTH;

	if (pThis->pIterator)
	{
		if (pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);
		
		pThis->pIterator->elementCount = pThis->cachedLength;
	}

	if (length == CSC_STRING_INVALID_LENGTH)
	{
		status = CSC_DynamicArrayPushValue(&pThis->dynamicArray, NULL);

		if (status != CSC_STATUS_SUCCESS)
			return status;

		arrayStringLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1;
		length = CSC_StringUtilsStrLenAnsiString((CSC_CHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), arrayStringLength, (CSC_BOOLEAN)TRUE);

		if (length == arrayStringLength)
		{
			pThis->cachedLength = length;

			if (pThis->pIterator)
				pThis->pIterator->elementCount = pThis->cachedLength;

			return status;
		}
	}

	if (length < arrayStringLength)
	{
		status = CSC_DynamicArrayResize(&pThis->dynamicArray, length + (CSC_SIZE_T)1);

		if (status != CSC_STATUS_SUCCESS)
			return status;

		arrayStringLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1;
		length = CSC_StringUtilsStrLenAnsiString((CSC_CHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), arrayStringLength, (CSC_BOOLEAN)TRUE);

		if (length == arrayStringLength)
		{
			pThis->cachedLength = length;

			if (pThis->pIterator)
				pThis->pIterator->elementCount = pThis->cachedLength;

			return status;
		}
	}

	return CSC_STATUS_GENERAL_FAILURE;
}


CSC_STATUS CSCMETHOD CSC_AStringDestroy(_Inout_ CSC_AString* CONST pThis)
{
	CSC_STATUS status;

	if (!pThis || pThis->pVTableIterator != (CSC_PCVOID)g_aStringVirtualTable)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayErase(&pThis->dynamicArray);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis->pIterator)
	{
		status = CSC_IteratorUnregister(pThis->pIterator);

		if (status != CSC_STATUS_SUCCESS)
			return status;
	}

	return CSC_AStringZeroMemory(pThis);
}

CSC_STATUS CSCMETHOD CSC_AStringErase(_Inout_ CSC_AString* CONST pThis)
{
	CONST CSC_CHAR nullTerminator = (CSC_CHAR)0;
	CSC_STATUS status = CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayAssign(&pThis->dynamicArray, (CSC_SIZE_T)1, (CSC_PCVOID)&nullTerminator);

	if (status == CSC_STATUS_SUCCESS)
	{
		pThis->cachedLength = (CSC_SIZE_T)0;

		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = pThis->cachedLength;

			if (pThis->pIterator->iterationValid)
				CSC_IteratorInvalidateIteration(pThis->pIterator);
		}
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_AStringZeroMemory(_Out_ CSC_AString* CONST pThis)
{
	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	pThis->pIterator = NULL;
	pThis->pVTableIterator = NULL;
	pThis->cachedLength = (CSC_SIZE_T)0;
	CSC_DynamicArrayZeroMemory(&pThis->dynamicArray);

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_AStringPrepareForIndexing(_Inout_ CSC_AString* CONST pThis)
{
	CONST CSC_STATUS status = CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis->cachedLength != CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_SUCCESS;

	pThis->cachedLength = CSC_StringUtilsStrLenAnsiString((CSC_CHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING, (CSC_BOOLEAN)TRUE);

	if (pThis->pIterator)
	{
		pThis->pIterator->elementCount = pThis->cachedLength;

		if (pThis->pIterator->iterationValid)
		{
			if (pThis->cachedLength != CSC_STRING_INVALID_LENGTH)
			{
				pThis->pIterator->pCurrElement = CSC_AStringLazyAccessElement(pThis, pThis->pIterator->currentIndex);

				if (!pThis->pIterator->pCurrElement)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
			else
			{
				CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}
	}

	return ((pThis->cachedLength != CSC_STRING_INVALID_LENGTH) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_CHAR* CSCMETHOD CSC_AStringAccessElement(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T index)
{
	CSC_AStringPrepareForIndexing(pThis);

	return CSC_AStringLazyAccessElement(pThis, index);
}

CSC_CHAR* CSCMETHOD CSC_AStringLazyAccessElement(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T index)
{
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pThis);

	if (length == CSC_STRING_INVALID_LENGTH || index >= length)
		return NULL;

	return (CSC_CHAR*)CSC_DynamicArrayAccessElement(&pThis->dynamicArray, index);
}


CSC_STATUS CSCMETHOD CSC_AStringAssignSequence(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements)
{
	return CSC_AStringAssignSequenceImpl(pThis, pArray, numOfElements, (CSC_BOOLEAN)TRUE);
}

static CSC_STATUS CSCMETHOD CSC_AStringAssignSequenceImpl(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_BOOLEAN validate)
{
	CSC_AString stringBuffer;
	CSC_STATUS status = CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_AStringInitializeSequenceImpl(&stringBuffer, pArray, numOfElements, CSC_DynamicArrayGetAllocator(&pThis->dynamicArray), validate);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_AStringCopy(pThis, &stringBuffer);
	CSC_AStringDestroy(&stringBuffer);

	return status;
}

CSC_STATUS CSCMETHOD CSC_AStringAssignCString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr)
{
	CONST CSC_SIZE_T length = CSC_StringUtilsStrLenAnsiString(pCStr, CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING, (CSC_BOOLEAN)TRUE);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringAssignSequenceImpl(pThis, pCStr, length, (CSC_BOOLEAN)FALSE);
}

CSC_STATUS CSCMETHOD CSC_AStringCopy(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pSrc)
{
	CSC_SIZE_T lengthBuffer;
	CSC_STATUS status = CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis == pSrc)
		return CSC_STATUS_SUCCESS;

	status = CSC_AStringIsValid(pSrc, (CSC_BOOLEAN)TRUE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayCopy(&pThis->dynamicArray, &pSrc->dynamicArray);

	if (status == CSC_STATUS_SUCCESS)
	{
		lengthBuffer = CSC_DynamicArrayGetSize(&pThis->dynamicArray);

		if (!lengthBuffer)
		{
			pThis->cachedLength = CSC_STRING_INVALID_LENGTH;
		}
		else
		{
			--lengthBuffer;
			pThis->cachedLength = ((lengthBuffer <= CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING) ? lengthBuffer : CSC_STRING_INVALID_LENGTH);
		}
	}

	if (pThis->pIterator)
	{
		pThis->pIterator->elementCount = pThis->cachedLength;

		if (pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_AStringMove(_Inout_ CSC_AString* CONST pThis, _Inout_ CSC_AString* CONST pSrc)
{
	CONST CSC_STATUS status = CSC_AStringCopy(pThis, pSrc);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis == pSrc)
		return CSC_STATUS_SUCCESS;

	CSC_AStringErase(pSrc);

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_AStringFill(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR value)
{
	CSC_DynamicArray arrayBuffer;
	CSC_STATUS status = CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!numOfElements || numOfElements > CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING || !value)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayInitializeWithValue(&arrayBuffer, CSC_DynamicArrayGetElementSize(&pThis->dynamicArray), numOfElements, (CSC_PCVOID)&value, CSC_DynamicArrayGetAllocator(&pThis->dynamicArray));

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayPushValue(&arrayBuffer, NULL);

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_DynamicArrayDestroy(&arrayBuffer);
		return status;
	}

	status = CSC_DynamicArrayCopy(&pThis->dynamicArray, &arrayBuffer);
	CSC_DynamicArrayDestroy(&arrayBuffer);

	if (status == CSC_STATUS_SUCCESS)
	{
		pThis->cachedLength = numOfElements;

		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = pThis->cachedLength;

			if (pThis->pIterator->iterationValid)
			{
				pThis->pIterator->pCurrElement = CSC_AStringLazyAccessElement(pThis, pThis->pIterator->currentIndex);

				if (!pThis->pIterator->pCurrElement)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_AStringFillRange(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR value)
{
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pThis);

	if (length == CSC_STRING_INVALID_LENGTH || !numOfElements || !value || firstIndex + numOfElements > length)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_MemoryUtilsSetMemory(CSC_DynamicArrayAccessElement(&pThis->dynamicArray, firstIndex), (CSC_BYTE)value, numOfElements);
}


CSC_STATUS CSCMETHOD CSC_AStringReplaceSequence(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T replaceIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray)
{
	return CSC_AStringReplaceSequenceImpl(pThis, replaceIndex, numOfElements, pArray, (CSC_BOOLEAN)TRUE);
}

static CSC_STATUS CSCMETHOD CSC_AStringReplaceSequenceImpl(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T replaceIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_BOOLEAN validate)
{
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pThis);

	if (length == CSC_STRING_INVALID_LENGTH || replaceIndex + numOfElements > length || (validate && CSC_AStringIsValidSequence(pArray, numOfElements) != CSC_STATUS_SUCCESS))
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_MemoryUtilsCopyMemory(CSC_DynamicArrayAccessElement(&pThis->dynamicArray, replaceIndex), (CSC_PCVOID)pArray, numOfElements);
}

CSC_STATUS CSCMETHOD CSC_AStringReplaceCString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T replaceIndex, _In_ CONST CSC_CHAR* CONST pCStr)
{
	CONST CSC_SIZE_T length = CSC_StringUtilsStrLenAnsiString(pCStr, CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING, (CSC_BOOLEAN)TRUE);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringReplaceSequenceImpl(pThis, replaceIndex, length, pCStr, (CSC_BOOLEAN)FALSE);
}

CSC_STATUS CSCMETHOD CSC_AStringReplaceString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T replaceIndex, _In_ CONST CSC_AString* CONST pOther)
{
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pOther);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringReplaceSequenceImpl(pThis, replaceIndex, length, (CSC_CHAR*)CSC_DynamicArrayFront(&pOther->dynamicArray), (CSC_BOOLEAN)FALSE);
}


CSC_STATUS CSCMETHOD CSC_AStringInsertSequence(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray)
{
	return CSC_AStringInsertSequenceImpl(pThis, insertIndex, numOfElements, pArray, (CSC_BOOLEAN)TRUE);
}

static CSC_STATUS CSCMETHOD CSC_AStringInsertSequenceImpl(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_BOOLEAN validate)
{
	CSC_SIZE_T length, arrayStringLength;
	CSC_STATUS status = CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	arrayStringLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1;
	length = CSC_StringUtilsStrLenAnsiString((CSC_CHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), arrayStringLength, (CSC_BOOLEAN)TRUE);

	if (length != arrayStringLength || insertIndex > length || length + numOfElements > CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING || (validate && CSC_AStringIsValidSequence(pArray, numOfElements) != CSC_STATUS_SUCCESS))
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayInsertRange(&pThis->dynamicArray, insertIndex, (CSC_PCVOID)pArray, numOfElements);

	if (status == CSC_STATUS_SUCCESS)
	{
		pThis->cachedLength = length + numOfElements;

		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = pThis->cachedLength;

			if (pThis->pIterator->iterationValid)
			{
				if (pThis->pIterator->currentIndex >= insertIndex)
					pThis->pIterator->currentIndex += numOfElements;

				pThis->pIterator->pCurrElement = CSC_AStringLazyAccessElement(pThis, pThis->pIterator->currentIndex);

				if (!pThis->pIterator->pCurrElement)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_AStringInsertCString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_ CONST CSC_CHAR* CONST pCStr)
{
	CONST CSC_SIZE_T length = CSC_StringUtilsStrLenAnsiString(pCStr, CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING, (CSC_BOOLEAN)TRUE);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringInsertSequenceImpl(pThis, insertIndex, length, pCStr, (CSC_BOOLEAN)FALSE);
}

CSC_STATUS CSCMETHOD CSC_AStringInsertString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_ CONST CSC_AString* CONST pOther)
{
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pOther);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringInsertSequenceImpl(pThis, insertIndex, length, (CSC_CHAR*)CSC_DynamicArrayFront(&pOther->dynamicArray), (CSC_BOOLEAN)FALSE);
}


CSC_STATUS CSCMETHOD CSC_AStringAppendSequence(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray)
{
	CONST CSC_SIZE_T insertIndex = CSC_AStringGetLength(pThis);

	if (insertIndex == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringInsertSequence(pThis, insertIndex, numOfElements, pArray);
}

CSC_STATUS CSCMETHOD CSC_AStringAppendCString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr)
{
	CONST CSC_SIZE_T insertIndex = CSC_AStringGetLength(pThis);

	if (insertIndex == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringInsertCString(pThis, insertIndex, pCStr);
}

CSC_STATUS CSCMETHOD CSC_AStringAppendString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther)
{
	CONST CSC_SIZE_T insertIndex = CSC_AStringGetLength(pThis);

	if (insertIndex == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringInsertString(pThis, insertIndex, pOther);
}


CSC_STATUS CSCMETHOD CSC_AStringPrependSequence(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray)
{
	return CSC_AStringInsertSequence(pThis, (CSC_SIZE_T)0, numOfElements, pArray);
}

CSC_STATUS CSCMETHOD CSC_AStringPrependCString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr)
{
	return CSC_AStringInsertCString(pThis, (CSC_SIZE_T)0, pCStr);
}

CSC_STATUS CSCMETHOD CSC_AStringPrependString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther)
{
	return CSC_AStringInsertString(pThis, (CSC_SIZE_T)0, pOther);
}


CSC_STATUS CSCMETHOD CSC_AStringReduceToSubstring(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T lastIndex)
{
	CSC_STATUS status;
	CSC_AString stringBuffer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pThis);

	if (length == CSC_STRING_INVALID_LENGTH || firstIndex > lastIndex || lastIndex >= length)
		return CSC_STATUS_INVALID_PARAMETER;

	if (!firstIndex && lastIndex == length - (CSC_SIZE_T)1)
		return CSC_STATUS_SUCCESS;

	status = CSC_AStringInitializeSequence(&stringBuffer, (CSC_CHAR*)CSC_DynamicArrayAccessElement(&pThis->dynamicArray, firstIndex), lastIndex - firstIndex + (CSC_SIZE_T)1, CSC_DynamicArrayGetAllocator(&pThis->dynamicArray));

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_AStringCopy(pThis, &stringBuffer);
	CSC_AStringDestroy(&stringBuffer);

	return status;
}

CSC_STATUS CSCMETHOD CSC_AStringReduceToSubstringRange(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T numOfElements)
{
	if (!numOfElements)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringReduceToSubstring(pThis, firstIndex, firstIndex + numOfElements - (CSC_SIZE_T)1);
}

CSC_STATUS CSCMETHOD CSC_AStringRemove(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T lastIndex)
{
	if (firstIndex > lastIndex)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringRemoveRange(pThis, firstIndex, lastIndex - firstIndex + (CSC_SIZE_T)1);
}

CSC_STATUS CSCMETHOD CSC_AStringRemoveRange(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T removeIndex, _In_ CONST CSC_SIZE_T numOfElements)
{
	CSC_SIZE_T length, arrayStringLength;
	CSC_STATUS status = CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	arrayStringLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1;
	length = CSC_StringUtilsStrLenAnsiString((CSC_CHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), arrayStringLength, (CSC_BOOLEAN)TRUE);

	if (length != arrayStringLength || !numOfElements || removeIndex + numOfElements > length)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayRemoveRange(&pThis->dynamicArray, removeIndex, numOfElements);

	if (status == CSC_STATUS_SUCCESS)
	{
		pThis->cachedLength = length - numOfElements;

		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = pThis->cachedLength;

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

					pThis->pIterator->pCurrElement = CSC_AStringLazyAccessElement(pThis, pThis->pIterator->currentIndex);

					if (!pThis->pIterator->pCurrElement)
						CSC_IteratorInvalidateIteration(pThis->pIterator);
				}
			}
		}
	}

	return status;
}


CSC_STATUS CSCMETHOD CSC_AStringReverse(_Inout_ CSC_AString* CONST pThis)
{
	CSC_STATUS status;
	CSC_DynamicArray arrayBuffer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pThis);

	if (length == CSC_STRING_INVALID_LENGTH || !length)
		return CSC_STATUS_INVALID_PARAMETER;

	if (length == (CSC_SIZE_T)1)
		return CSC_STATUS_SUCCESS;

	status = CSC_DynamicArrayInitializeWithArray(&arrayBuffer, (CSC_PCVOID)CSC_DynamicArrayFront(&pThis->dynamicArray), length, CSC_DynamicArrayGetElementSize(&pThis->dynamicArray), CSC_DynamicArrayGetAllocator(&pThis->dynamicArray));

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayReverse(&arrayBuffer);

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_DynamicArrayDestroy(&arrayBuffer);
		return status;
	}

	status = CSC_DynamicArrayPushValue(&arrayBuffer, NULL);

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_DynamicArrayDestroy(&arrayBuffer);
		return status;
	}

	status = CSC_DynamicArrayCopy(&pThis->dynamicArray, &arrayBuffer);
	CSC_DynamicArrayDestroy(&arrayBuffer);

	if (status == CSC_STATUS_SUCCESS && pThis->pIterator && pThis->pIterator->iterationValid)
		CSC_IteratorInvalidateIteration(pThis->pIterator);

	return status;
}


CSC_SIZE_T CSCMETHOD CSC_AStringFindSequence(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray)
{
	return CSC_AStringFindSequenceImpl(pThis, numOfElements, pArray, (CSC_BOOLEAN)FALSE, (CSC_BOOLEAN)FALSE, (CSC_BOOLEAN)TRUE);
}

static CSC_SIZE_T CSCMETHOD CSC_AStringFindSequenceImpl(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_BOOLEAN reverseDir, _In_ CONST CSC_BOOLEAN reverseIndexDir, _In_ CONST CSC_BOOLEAN validate)
{
	CSC_STATUS status;
	CSC_SIZE_T iterator;
	CONST CSC_CHAR* pCompareBase;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pThis);

	if (length == CSC_STRING_INVALID_LENGTH || !length || numOfElements > length || (validate && CSC_AStringIsValidSequence(pArray, numOfElements) != CSC_STATUS_SUCCESS))
		return CSC_STRING_INVALID_INDEX;

	pCompareBase = (CONST CSC_CHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray);

	for (iterator = (CSC_SIZE_T)0; iterator <= length - numOfElements; ++iterator)
	{
		if (reverseDir)
		{
			status = CSC_MemoryUtilsCompareMemory((CSC_PCVOID)&pCompareBase[length - numOfElements - iterator], (CSC_PCVOID)pArray, numOfElements);

			if (status == CSC_STATUS_SUCCESS)
				return ((reverseIndexDir) ? iterator : length - numOfElements - iterator);
		}
		else
		{
			status = CSC_MemoryUtilsCompareMemory((CSC_PCVOID)&pCompareBase[iterator], (CSC_PCVOID)pArray, numOfElements);

			if (status == CSC_STATUS_SUCCESS)
				return ((reverseIndexDir) ? length - numOfElements - iterator : iterator);
		}
	}

	return CSC_STRING_INVALID_INDEX;
}

CSC_SIZE_T CSCMETHOD CSC_AStringFindCString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr)
{
	CONST CSC_SIZE_T length = CSC_StringUtilsStrLenAnsiString(pCStr, CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING, (CSC_BOOLEAN)TRUE);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STRING_INVALID_INDEX;

	return CSC_AStringFindSequenceImpl(pThis, length, pCStr, (CSC_BOOLEAN)FALSE, (CSC_BOOLEAN)FALSE, (CSC_BOOLEAN)FALSE);
}

CSC_SIZE_T CSCMETHOD CSC_AStringFindString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther)
{
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pOther);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STRING_INVALID_INDEX;

	return CSC_AStringFindSequenceImpl(pThis, length, (CSC_CHAR*)CSC_DynamicArrayFront(&pOther->dynamicArray), (CSC_BOOLEAN)FALSE, (CSC_BOOLEAN)FALSE, (CSC_BOOLEAN)FALSE);
}


CSC_SIZE_T CSCMETHOD CSC_AStringRFindSequence(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray)
{
	return CSC_AStringRFindSequenceImpl(pThis, numOfElements, pArray, (CSC_BOOLEAN)TRUE);
}

static CSC_SIZE_T CSCMETHOD CSC_AStringRFindSequenceImpl(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_BOOLEAN validate)
{
	return CSC_AStringFindSequenceImpl(pThis, numOfElements, pArray, (CSC_BOOLEAN)TRUE, (CSC_BOOLEAN)FALSE, validate);
}

CSC_SIZE_T CSCMETHOD CSC_AStringRFindCString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr)
{
	CONST CSC_SIZE_T length = CSC_StringUtilsStrLenAnsiString(pCStr, CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING, (CSC_BOOLEAN)TRUE);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STRING_INVALID_INDEX;

	return CSC_AStringRFindSequenceImpl(pThis, length, pCStr, (CSC_BOOLEAN)FALSE);
}

CSC_SIZE_T CSCMETHOD CSC_AStringRFindString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther)
{
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pOther);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STRING_INVALID_INDEX;

	return CSC_AStringRFindSequenceImpl(pThis, length, (CSC_CHAR*)CSC_DynamicArrayFront(&pOther->dynamicArray), (CSC_BOOLEAN)FALSE);
}


CSC_STATUS CSCMETHOD CSC_AStringContainsSequence(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray)
{
	return ((CSC_AStringFindSequence(pThis, numOfElements, pArray) != CSC_STRING_INVALID_INDEX) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_STATUS CSCMETHOD CSC_AStringContainsCString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr)
{
	return ((CSC_AStringFindCString(pThis, pCStr) != CSC_STRING_INVALID_INDEX) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_STATUS CSCMETHOD CSC_AStringContainsString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther)
{
	return ((CSC_AStringFindString(pThis, pOther) != CSC_STRING_INVALID_INDEX) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}


CSC_STATUS CSCMETHOD CSC_AStringStartsWithSequence(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray)
{
	return ((CSC_AStringFindSequence(pThis, numOfElements, pArray) == (CSC_SIZE_T)0) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_STATUS CSCMETHOD CSC_AStringStartsWithCString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr)
{
	return ((CSC_AStringFindCString(pThis, pCStr) == (CSC_SIZE_T)0) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_STATUS CSCMETHOD CSC_AStringStartsWithString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther)
{
	return ((CSC_AStringFindString(pThis, pOther) == (CSC_SIZE_T)0) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}


CSC_STATUS CSCMETHOD CSC_AStringEndsWithSequence(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray)
{
	return CSC_AStringEndsWithSequenceImpl(pThis, numOfElements, pArray, (CSC_BOOLEAN)TRUE);
}

static CSC_STATUS CSCMETHOD CSC_AStringEndsWithSequenceImpl(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_BOOLEAN validate)
{
	return ((CSC_AStringFindSequenceImpl(pThis, numOfElements, pArray, (CSC_BOOLEAN)TRUE, (CSC_BOOLEAN)TRUE, validate) == (CSC_SIZE_T)0) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_STATUS CSCMETHOD CSC_AStringEndsWithCString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr)
{
	CONST CSC_SIZE_T length = CSC_StringUtilsStrLenAnsiString(pCStr, CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING, (CSC_BOOLEAN)TRUE);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringEndsWithSequenceImpl(pThis, length, pCStr, (CSC_BOOLEAN)FALSE);
}

CSC_STATUS CSCMETHOD CSC_AStringEndsWithString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther)
{
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pOther);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringEndsWithSequenceImpl(pThis, length, (CSC_CHAR*)CSC_DynamicArrayFront(&pOther->dynamicArray), (CSC_BOOLEAN)FALSE);
}


CSC_STATUS CSCMETHOD CSC_AStringCompareWithSequence(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray)
{
	return CSC_AStringCompareWithSequenceImpl(pThis, strict, numOfElements, pArray, (CSC_BOOLEAN)TRUE);
}

static CSC_STATUS CSCMETHOD CSC_AStringCompareWithSequenceImpl(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_BOOLEAN validate)
{
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pThis);

	if (length == CSC_STRING_INVALID_LENGTH || !length || (validate && CSC_AStringIsValidSequence(pArray, numOfElements) != CSC_STATUS_SUCCESS))
		return CSC_STATUS_INVALID_PARAMETER;

	if (strict && length != numOfElements)
		return CSC_STATUS_GENERAL_FAILURE;

	return CSC_MemoryUtilsCompareMemory((CSC_PCVOID)CSC_DynamicArrayFront(&pThis->dynamicArray), (CSC_PCVOID)pArray, ((length < numOfElements) ? length : numOfElements));
}

CSC_STATUS CSCMETHOD CSC_AStringCompareWithCString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_CHAR* CONST pCStr)
{
	CONST CSC_SIZE_T length = CSC_StringUtilsStrLenAnsiString(pCStr, CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING, (CSC_BOOLEAN)TRUE);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringCompareWithSequenceImpl(pThis, strict, length, pCStr, (CSC_BOOLEAN)FALSE);
}

CSC_STATUS CSCMETHOD CSC_AStringCompareWithString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_AString* CONST pOther)
{
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pOther);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_AStringCompareWithSequenceImpl(pThis, strict, length, (CSC_CHAR*)CSC_DynamicArrayFront(&pOther->dynamicArray), (CSC_BOOLEAN)FALSE);
}


CSC_CHAR* CSCMETHOD CSC_AStringGetData(_In_ CONST CSC_AString* CONST pThis)
{
	return ((CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE) == CSC_STATUS_SUCCESS) ? (CSC_CHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray) : NULL);
}


CSC_STATUS CSCMETHOD CSC_AStringIsEmpty(_In_ CONST CSC_AString* CONST pThis)
{
	return ((CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE) != CSC_STATUS_SUCCESS || *(CSC_CHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray) == (CSC_CHAR)0) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_STATUS CSCMETHOD CSC_AStringIsValid(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_BOOLEAN checkLength)
{
	CSC_SIZE_T arrayLength;

	if (!pThis || pThis->pVTableIterator != (CSC_PCVOID)g_aStringVirtualTable || CSC_DynamicArrayGetElementSize(&pThis->dynamicArray) != sizeof(CSC_CHAR))
		return CSC_STATUS_INVALID_PARAMETER;

	arrayLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray);

	if (!arrayLength || arrayLength - (CSC_SIZE_T)1 > CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING || (checkLength && CSC_StringUtilsStrLenAnsiString((CSC_CHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), arrayLength - (CSC_SIZE_T)1, (CSC_BOOLEAN)TRUE) != arrayLength - (CSC_SIZE_T)1))
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_STATUS_SUCCESS;
}


CSC_SIZE_T CSCMETHOD CSC_AStringGetLength(_In_ CONST CSC_AString* CONST pThis)
{
	if (CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE) != CSC_STATUS_SUCCESS)
		return CSC_STRING_INVALID_LENGTH;

	return ((pThis->cachedLength != CSC_STRING_INVALID_LENGTH) ? pThis->cachedLength : CSC_StringUtilsStrLenAnsiString((CSC_CHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1, (CSC_BOOLEAN)TRUE));
}

CSC_SIZE_T CSCMETHOD CSC_AStringGetCapacity(_In_ CONST CSC_AString* CONST pThis)
{
	return ((CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE) == CSC_STATUS_SUCCESS) ? CSC_DynamicArrayGetCapacity(&pThis->dynamicArray) : (CSC_SIZE_T)0);
}

CSC_SIZE_T CSCMETHOD CSC_AStringGetMaxLength(_In_ CONST CSC_AString* CONST pThis)
{
	return ((CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE) == CSC_STATUS_SUCCESS) ? CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING : (CSC_SIZE_T)0);
}

CSC_PVOID CSCMETHOD CSC_AStringGetAllocator(_In_ CONST CSC_AString* CONST pThis)
{
	return ((CSC_AStringIsValid(pThis, (CSC_BOOLEAN)FALSE) == CSC_STATUS_SUCCESS) ? CSC_DynamicArrayGetAllocator(&pThis->dynamicArray) : NULL);
}


static CSC_STATUS CSCMETHOD CSC_AStringIteratorRegister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CSC_AString* CONST pAString = (CSC_AString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pAString);

	if (!pAString || length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	if (!pIterator)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pAString->pIterator)
		return CSC_STATUS_GENERAL_FAILURE;

	elementSize = CSC_DynamicArrayGetElementSize(&pAString->dynamicArray);

	if (elementSize != sizeof(CSC_CHAR))
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementSize = elementSize;
	pIterator->elementCount = length;
	pIterator->swapByIndex = (CSC_BOOLEAN)TRUE;
	pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
	pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	pIterator->pCurrElement = NULL;
	pIterator->pContainer = pContainer;

	pAString->pIterator = pIterator;

	return CSC_STATUS_SUCCESS;
}

static CSC_STATUS CSCMETHOD CSC_AStringIteratorUnregister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_AString* CONST pAString = (CSC_AString* CONST)pContainer;
	CONST CSC_STATUS status = CSC_AStringIsValid(pAString, (CSC_BOOLEAN)FALSE);

	if (!pAString || status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pAString->pIterator != pIterator)
		return CSC_STATUS_GENERAL_FAILURE;

	pAString->pIterator = NULL;
	CSC_IteratorZeroMemory(pIterator);

	return CSC_STATUS_SUCCESS;
}

static CSC_STATUS CSCMETHOD CSC_AStringIteratorFirstElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_AString* CONST pAString = (CONST CSC_AString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pAString);

	if (!pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pAString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_CHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->pCurrElement = CSC_DynamicArrayFront(&pAString->dynamicArray);

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

static CSC_STATUS CSCMETHOD CSC_AStringIteratorNextElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_AString* CONST pAString = (CONST CSC_AString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pAString);

	if (!pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pAString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_CHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (!pIterator->iterationValid)
		return CSC_STATUS_INVALID_PARAMETER;

	if (length == CSC_STRING_INVALID_LENGTH || !length || pIterator->currentIndex >= length)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return CSC_STATUS_INVALID_PARAMETER;
	}

	++pIterator->currentIndex;
	pIterator->pCurrElement = (pIterator->currentIndex >= length) ? NULL : CSC_DynamicArrayAccessElement(&pAString->dynamicArray, pIterator->currentIndex);

	if (!pIterator->pCurrElement)
	{
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_STATUS CSCMETHOD CSC_AStringIteratorLastElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_AString* CONST pAString = (CONST CSC_AString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pAString);

	if (!pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pAString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_CHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->pCurrElement = CSC_DynamicArrayAccessElement(&pAString->dynamicArray, length - (CSC_SIZE_T)1);

	if (pIterator->pCurrElement)
	{
		pIterator->iterationValid = (CSC_BOOLEAN)TRUE;
		pIterator->currentIndex = length - (CSC_SIZE_T)1;
	}
	else
	{
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_STATUS CSCMETHOD CSC_AStringIteratorPreviousElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_AString* CONST pAString = (CONST CSC_AString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pAString);

	if (!pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pAString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_CHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (!pIterator->iterationValid)
		return CSC_STATUS_INVALID_PARAMETER;

	if (length == CSC_STRING_INVALID_LENGTH || !length || pIterator->currentIndex >= length || !pIterator->currentIndex)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return (!pIterator->currentIndex) ? CSC_STATUS_GENERAL_FAILURE : CSC_STATUS_INVALID_PARAMETER;
	}

	--pIterator->currentIndex;
	pIterator->pCurrElement = CSC_DynamicArrayAccessElement(&pAString->dynamicArray, pIterator->currentIndex);

	if (!pIterator->pCurrElement)
	{
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_STATUS CSCMETHOD CSC_AStringIteratorMoveToIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index)
{
	CSC_SIZE_T elementSize;
	CONST CSC_AString* CONST pAString = (CONST CSC_AString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pAString);

	if (!pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pAString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_CHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length || index >= length)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->pCurrElement = CSC_DynamicArrayAccessElement(&pAString->dynamicArray, index);

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

static CSC_SIZE_T CSCMETHOD CSC_AStringIteratorGetCurrentElementIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_AString* CONST pAString = (CONST CSC_AString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pAString);

	if (!pAString)
		return CSC_ITERATOR_INVALID_INDEX;

	elementSize = CSC_DynamicArrayGetElementSize(&pAString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_CHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pAString)
		return CSC_ITERATOR_INVALID_INDEX;

	pIterator->elementCount = length;

	if (!pIterator->iterationValid)
		return CSC_ITERATOR_INVALID_INDEX;

	if (length == CSC_STRING_INVALID_LENGTH || !length || pIterator->currentIndex >= length)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return CSC_ITERATOR_INVALID_INDEX;
	}

	return pIterator->currentIndex;
}

static CSC_PVOID CSCMETHOD CSC_AStringIteratorGetCurrentElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_AString* CONST pAString = (CONST CSC_AString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pAString);

	if (!pAString)
		return NULL;

	elementSize = CSC_DynamicArrayGetElementSize(&pAString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_CHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pAString)
		return NULL;

	pIterator->elementCount = length;

	if (!pIterator->iterationValid)
		return NULL;

	if (length == CSC_STRING_INVALID_LENGTH || !length || pIterator->currentIndex >= length)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return NULL;
	}

	return CSC_DynamicArrayAccessElement(&pAString->dynamicArray, pIterator->currentIndex);
}

static CSC_PVOID CSCMETHOD CSC_AStringIteratorGetElementAt(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index)
{
	CSC_SIZE_T elementSize;
	CONST CSC_AString* CONST pAString = (CONST CSC_AString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pAString);

	if (!pAString)
		return NULL;

	elementSize = CSC_DynamicArrayGetElementSize(&pAString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_CHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pAString)
		return NULL;

	pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length || index >= length)
		return NULL;

	return CSC_DynamicArrayAccessElement(&pAString->dynamicArray, index);
}

static CSC_SIZE_T CSCMETHOD CSC_AStringIteratorGetElementCount(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_AString* CONST pAString = (CONST CSC_AString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pAString);

	if (!pAString)
		return CSC_ITERATOR_INVALID_LENGTH;

	elementSize = CSC_DynamicArrayGetElementSize(&pAString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_CHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pAString)
		return CSC_ITERATOR_INVALID_LENGTH;

	pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_ITERATOR_INVALID_LENGTH;

	return length;
}

static CSC_STATUS CSCMETHOD CSC_AStringIteratorSwapElementsByIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T secondIndex)
{
	CSC_STATUS status;
	CSC_SIZE_T elementSize;
	CONST CSC_AString* CONST pAString = (CONST CSC_AString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pAString);

	if (!pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pAString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_CHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length || firstIndex >= length || secondIndex >= length)
		return CSC_STATUS_INVALID_PARAMETER;

	if (firstIndex == secondIndex)
		return CSC_STATUS_SUCCESS;

	return CSC_MemoryUtilsSwapValues256(CSC_DynamicArrayAccessElement(&pAString->dynamicArray, firstIndex), CSC_DynamicArrayAccessElement(&pAString->dynamicArray, secondIndex), elementSize);
}

static CSC_STATUS CSCMETHOD CSC_AStringIteratorSwapElementsByPtr(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond)
{
	CSC_STATUS status;
	CSC_SIZE_T elementSize;
	CSC_PVOID pBack, pFront;
	CONST CSC_AString* CONST pAString = (CONST CSC_AString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pAString);

	if (!pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pAString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_CHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length)
		return CSC_STATUS_INVALID_PARAMETER;

	pFront = CSC_DynamicArrayFront(&pAString->dynamicArray);
	pBack = CSC_DynamicArrayAccessElement(&pAString->dynamicArray, length - (CSC_SIZE_T)1);

	if (!pFront || !pBack || (CSC_POINTER_TYPE)pFirst < (CSC_POINTER_TYPE)pFront || (CSC_POINTER_TYPE)pFirst >(CSC_POINTER_TYPE)pBack || (CSC_POINTER_TYPE)pSecond < (CSC_POINTER_TYPE)pFront || (CSC_POINTER_TYPE)pSecond >(CSC_POINTER_TYPE)pBack)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pFirst == pSecond)
		return CSC_STATUS_SUCCESS;

	return CSC_MemoryUtilsSwapValues256(pFirst, pSecond, elementSize);
}


static CSC_STATUS CSCAPI CSC_WStringIsValidSequence(_In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _When_(return == CSC_STATUS_SUCCESS, _Out_opt_) CSC_SIZE_T* CONST pCodePointLength)
{
	CSC_SIZE_T charLength;

	if (!numOfElements)
	{
		if (pCodePointLength)
			*pCodePointLength = CSC_STRING_INVALID_LENGTH;

		return CSC_STATUS_INVALID_PARAMETER;
	}

	charLength = CSC_StringUtilsStrWideCharLenWideString(pArray, numOfElements, (CSC_BOOLEAN)FALSE, pCodePointLength);

	if (charLength == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	if (charLength != numOfElements)
	{
		if (pCodePointLength)
			*pCodePointLength = CSC_STRING_INVALID_LENGTH;

		return CSC_STATUS_INVALID_PARAMETER;
	}

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_WStringInitialize(_Out_ CSC_WString* CONST pThis, _In_ CONST CSC_PVOID pAllocator)
{
	CSC_STATUS status;
	CONST CSC_WCHAR nullTerminator = (CSC_WCHAR)0;

	if (!pThis || !pAllocator)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_WStringZeroMemory(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayInitialize(&pThis->indexArray, sizeof(CSC_SIZE_T), pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayInitializeWithValue(&pThis->dynamicArray, sizeof(CSC_WCHAR), (CSC_SIZE_T)1, (CSC_PCVOID)&nullTerminator, pAllocator);

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_DynamicArrayDestroy(&pThis->indexArray);
		return status;
	}

	pThis->pVTableIterator = (CSC_PCVOID)g_wStringVirtualTable;

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_WStringInitializeSequence(_Out_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PVOID pAllocator)
{
	return CSC_WStringInitializeSequenceImpl(pThis, pArray, numOfElements, pAllocator, (CSC_BOOLEAN)TRUE);
}

static CSC_STATUS CSCMETHOD CSC_WStringInitializeSequenceImpl(_Out_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PVOID pAllocator, _In_ CONST CSC_BOOLEAN validate)
{
	CSC_STATUS status = CSC_WStringInitialize(pThis, pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (validate)
	{
		if (CSC_WStringIsValidSequence(pArray, numOfElements, NULL) != CSC_STATUS_SUCCESS)
		{
			CSC_WStringDestroy(pThis);
			return CSC_STATUS_INVALID_PARAMETER;
		}
	}

	status = CSC_DynamicArrayInsertRange(&pThis->dynamicArray, (CSC_SIZE_T)0, (CSC_PCVOID)pArray, numOfElements);

	if (status != CSC_STATUS_SUCCESS)
		CSC_WStringDestroy(pThis);

	return status;
}

CSC_STATUS CSCMETHOD CSC_WStringInitializeCString(_Out_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr, _In_ CONST CSC_PVOID pAllocator)
{
	CONST CSC_SIZE_T length = CSC_StringUtilsStrWideCharLenWideString(pCStr, CSC_MAXIMUM_STRING_CHARS_WIDE_STRING, (CSC_BOOLEAN)TRUE, NULL);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringInitializeSequenceImpl(pThis, pCStr, length, pAllocator, (CSC_BOOLEAN)FALSE);
}

CSC_STATUS CSCMETHOD CSC_WStringInitializeCopy(_Out_ CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther, _In_ CONST CSC_PVOID pAllocator)
{
	CSC_STATUS status = CSC_WStringInitialize(pThis, pAllocator);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_WStringCopy(pThis, pOther);

	if (status != CSC_STATUS_SUCCESS)
		CSC_WStringDestroy(pThis);

	return status;
}


CSC_STATUS CSCMETHOD CSC_WStringReserveChars(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements)
{
	CSC_STATUS status = CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (numOfElements > CSC_MAXIMUM_STRING_CHARS_WIDE_STRING)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayReserve(&pThis->dynamicArray, numOfElements + (CSC_SIZE_T)1);

	if (status == CSC_STATUS_SUCCESS && pThis->pIterator)
	{
		if (CSC_WStringPrepareForIndexing(pThis) != CSC_STATUS_SUCCESS && pThis->pIterator->iterationValid)
		{
			CSC_IteratorInvalidateIteration(pThis->pIterator);
		}
		else
		{
			if (pThis->pIterator->iterationValid)
			{
				pThis->pIterator->pCurrElement = CSC_WStringLazyAccessCodePoint(pThis, pThis->pIterator->currentIndex);

				if (!pThis->pIterator->pCurrElement)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_WStringShrinkToFit(_Inout_ CSC_WString* CONST pThis)
{
	CSC_STATUS status = CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayShrinkToFit(&pThis->dynamicArray);

	if (status == CSC_STATUS_SUCCESS && pThis->pIterator)
	{
		if (CSC_WStringPrepareForIndexing(pThis) != CSC_STATUS_SUCCESS && pThis->pIterator->iterationValid)
		{
			CSC_IteratorInvalidateIteration(pThis->pIterator);
		}
		else
		{
			if (pThis->pIterator->iterationValid)
			{
				pThis->pIterator->pCurrElement = CSC_WStringLazyAccessCodePoint(pThis, pThis->pIterator->currentIndex);

				if (!pThis->pIterator->pCurrElement)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
		}
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_WStringRepair(_Inout_ CSC_WString* CONST pThis)
{
	CSC_STATUS status;
	CONST CSC_WCHAR* pWChar;
	CSC_SIZE_T arrayLength, charLength, arrayStringCharLength;

	if (!pThis || pThis->pVTableIterator != (CSC_PCVOID)g_wStringVirtualTable || CSC_DynamicArrayGetElementSize(&pThis->dynamicArray) != sizeof(CSC_WCHAR) || CSC_DynamicArrayGetElementSize(&pThis->indexArray) != sizeof(CSC_SIZE_T))
		return CSC_STATUS_INVALID_PARAMETER;

	arrayLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray);

	if (!arrayLength || arrayLength - (CSC_SIZE_T)1 > CSC_MAXIMUM_STRING_CHARS_WIDE_STRING)
	{
		if (arrayLength)
		{
			status = CSC_DynamicArrayErase(&pThis->dynamicArray);

			if (status != CSC_STATUS_SUCCESS)
			{
				if (pThis->pIterator)
				{
					if (pThis->pIterator->iterationValid)
						CSC_IteratorInvalidateIteration(pThis->pIterator);

					pThis->pIterator->elementCount = CSC_ITERATOR_INVALID_LENGTH;
				}

				return status;
			}
		}

		arrayStringCharLength = CSC_STRING_INVALID_LENGTH;
		charLength = CSC_STRING_INVALID_LENGTH;
	}
	else
	{
		arrayStringCharLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1;
		charLength = CSC_StringUtilsStrWideCharLenWideString((CSC_WCHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), arrayStringCharLength, (CSC_BOOLEAN)TRUE, NULL);

		if (charLength == arrayStringCharLength)
		{
			if (CSC_WStringPrepareForIndexing(pThis) != CSC_STATUS_SUCCESS && pThis->pIterator->iterationValid)
			{
				CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
			else
			{
				if (pThis->pIterator->iterationValid)
				{
					pThis->pIterator->pCurrElement = CSC_WStringLazyAccessCodePoint(pThis, pThis->pIterator->currentIndex);

					if (!pThis->pIterator->pCurrElement)
						CSC_IteratorInvalidateIteration(pThis->pIterator);
				}
			}

			return CSC_STATUS_SUCCESS;
		}
	}

	pThis->validCache = (CSC_BOOLEAN)FALSE;
	CSC_DynamicArrayErase(&pThis->indexArray);

	if (pThis->pIterator)
	{
		if (pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);

		pThis->pIterator->elementCount = CSC_ITERATOR_INVALID_LENGTH;
	}

	if (charLength == CSC_STRING_INVALID_LENGTH)
	{
		if (arrayStringCharLength != CSC_STRING_INVALID_LENGTH)
		{
			pWChar = (CONST CSC_WCHAR*)CSC_DynamicArrayBack(&pThis->dynamicArray);

			if (!pWChar)
				return CSC_STATUS_GENERAL_FAILURE;

			if (!*pWChar)
			{
				status = CSC_DynamicArrayErase(&pThis->dynamicArray);

				if (status != CSC_STATUS_SUCCESS)
					return status;
			}
		}

		status = CSC_DynamicArrayPushValue(&pThis->dynamicArray, NULL);

		if (status != CSC_STATUS_SUCCESS)
			return status;

		arrayStringCharLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1;
		charLength = CSC_StringUtilsStrWideCharLenWideString((CSC_WCHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), arrayStringCharLength, (CSC_BOOLEAN)TRUE, NULL);

		if (charLength == arrayStringCharLength)
		{
			CSC_WStringPrepareForIndexing(pThis);
			return CSC_STATUS_SUCCESS;
		}
	}

	if (charLength < arrayStringCharLength)
	{
		status = CSC_DynamicArrayResize(&pThis->dynamicArray, charLength + (CSC_SIZE_T)1);

		if (status == CSC_STATUS_SUCCESS && pThis->pIterator)
			CSC_WStringPrepareForIndexing(pThis);

		return status;
	}

	return CSC_STATUS_GENERAL_FAILURE;
}


CSC_STATUS CSCMETHOD CSC_WStringDestroy(_Inout_ CSC_WString* CONST pThis)
{
	CSC_STATUS status;

	if (!pThis || pThis->pVTableIterator != (CSC_PCVOID)g_wStringVirtualTable)
		return CSC_STATUS_INVALID_PARAMETER;

	pThis->validCache = (CSC_BOOLEAN)FALSE;
	status = CSC_DynamicArrayErase(&pThis->dynamicArray);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayErase(&pThis->indexArray);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis->pIterator)
	{
		status = CSC_IteratorUnregister(pThis->pIterator);

		if (status != CSC_STATUS_SUCCESS)
			return status;
	}

	return CSC_WStringZeroMemory(pThis);
}

CSC_STATUS CSCMETHOD CSC_WStringErase(_Inout_ CSC_WString* CONST pThis)
{
	CONST CSC_WCHAR nullTerminator = (CSC_WCHAR)0;
	CSC_STATUS status = CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayAssign(&pThis->dynamicArray, (CSC_SIZE_T)1, (CSC_PCVOID)&nullTerminator);

	if (status == CSC_STATUS_SUCCESS)
	{
		pThis->validCache = (CSC_BOOLEAN)FALSE;
		CSC_DynamicArrayErase(&pThis->indexArray);

		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = (CSC_SIZE_T)0;

			if (pThis->pIterator->iterationValid)
				CSC_IteratorInvalidateIteration(pThis->pIterator);
		}
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_WStringZeroMemory(_Out_ CSC_WString* CONST pThis)
{
	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	pThis->pIterator = NULL;
	pThis->pVTableIterator = NULL;
	pThis->validCache = (CSC_BOOLEAN)FALSE;
	CSC_DynamicArrayZeroMemory(&pThis->dynamicArray);
	CSC_DynamicArrayZeroMemory(&pThis->indexArray);

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_WStringPrepareForIndexing(_Inout_ CSC_WString* CONST pThis)
{
	CONST CSC_WCHAR* pCStr;
	CSC_CodePointType currentCPT;
	CSC_SIZE_T charLength, iterator;
	CSC_STATUS status = CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
	{
		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = (CSC_SIZE_T)0;

			if (pThis->pIterator->iterationValid)
				CSC_IteratorInvalidateIteration(pThis->pIterator);
		}

		return status;
	}

	if (pThis->validCache)
	{
		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = CSC_DynamicArrayGetSize(&pThis->indexArray);

			if (pThis->pIterator->iterationValid && pThis->pIterator->currentIndex >= pThis->pIterator->elementCount)
				CSC_IteratorInvalidateIteration(pThis->pIterator);
		}

		return CSC_STATUS_SUCCESS;
	}

	pCStr = (CONST CSC_WCHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray);

	if (!pCStr)
	{
		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = (CSC_SIZE_T)0;

			if (pThis->pIterator->iterationValid)
				CSC_IteratorInvalidateIteration(pThis->pIterator);
		}

		return CSC_STATUS_GENERAL_FAILURE;
	}

	charLength = CSC_StringUtilsStrWideCharLenWideString(pCStr, CSC_MAXIMUM_STRING_CHARS_WIDE_STRING, (CSC_BOOLEAN)TRUE, NULL);

	if (charLength == CSC_STRING_INVALID_LENGTH || !charLength)
	{
		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = (CSC_SIZE_T)0;

			if (pThis->pIterator->iterationValid)
				CSC_IteratorInvalidateIteration(pThis->pIterator);
		}

		return CSC_STATUS_GENERAL_FAILURE;
	}

	status = CSC_DynamicArrayReserve(&pThis->indexArray, charLength);

	if (status != CSC_STATUS_SUCCESS)
	{
		if (pThis->pIterator)
			pThis->pIterator->elementCount = (CSC_SIZE_T)0;

		return status;
	}

	status = CSC_DynamicArrayClear(&pThis->indexArray);

	if (status != CSC_STATUS_SUCCESS)
	{
		if (pThis->pIterator)
			pThis->pIterator->elementCount = (CSC_SIZE_T)0;

		return status;
	}

	for (iterator = (CSC_SIZE_T)0; iterator < charLength; ++iterator)
	{
		if (!pCStr[iterator])
		{
			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = (CSC_SIZE_T)0;

				if (pThis->pIterator->iterationValid)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}

			CSC_DynamicArrayErase(&pThis->indexArray);
			return CSC_STATUS_GENERAL_FAILURE;
		}

		currentCPT = CSC_StringUtilsGetCodePointType(pCStr[iterator]);

		if (currentCPT == csc_cpt_invalid || currentCPT == csc_cpt_surrogate_low)
		{
			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = (CSC_SIZE_T)0;

				if (pThis->pIterator->iterationValid)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}

			CSC_DynamicArrayErase(&pThis->indexArray);
			return CSC_STATUS_GENERAL_FAILURE;
		}

		if (currentCPT != csc_cpt_bmp && (iterator >= charLength - (CSC_SIZE_T)1 || !CSC_StringUtilsIsSurrogatePair(pCStr[iterator], pCStr[iterator + (CSC_SIZE_T)1])))
		{
			if (pThis->pIterator)
			{
				pThis->pIterator->elementCount = (CSC_SIZE_T)0;

				if (pThis->pIterator->iterationValid)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}

			CSC_DynamicArrayErase(&pThis->indexArray);
			return CSC_STATUS_GENERAL_FAILURE;
		}

		status = CSC_DynamicArrayPushValue(&pThis->indexArray, (CSC_PCVOID)&iterator);

		if (status != CSC_STATUS_SUCCESS)
		{
			CSC_DynamicArrayErase(&pThis->indexArray);
			return status;
		}

		if (currentCPT != csc_cpt_bmp)
			++iterator;
	}

	pThis->validCache = (CSC_BOOLEAN)TRUE;

	if (pThis->pIterator)
	{
		pThis->pIterator->elementCount = CSC_DynamicArrayGetSize(&pThis->indexArray);

		if (pThis->pIterator->iterationValid && pThis->pIterator->currentIndex >= pThis->pIterator->elementCount)
			CSC_IteratorInvalidateIteration(pThis->pIterator);
	}

	return CSC_STATUS_SUCCESS;
}

CSC_WCHAR* CSCMETHOD CSC_WStringAccessCodePoint(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T codePointIndex)
{
	CSC_WStringPrepareForIndexing(pThis);

	return CSC_WStringLazyAccessCodePoint(pThis, codePointIndex);
}

CSC_WCHAR* CSCMETHOD CSC_WStringLazyAccessCodePoint(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T codePointIndex)
{
	return CSC_WStringLazyAccessCodePointImpl(pThis, codePointIndex, NULL);
}

static CSC_WCHAR* CSCMETHOD CSC_WStringLazyAccessCodePointImpl(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T codePointIndex, _When_(return != NULL, _Out_opt_) CSC_SIZE_T* CONST pArrayCharIndex)
{
	CSC_WCHAR* pResult;
	CSC_SIZE_T iterator, codePointIterator, arrayStringLength;
	CONST CSC_WCHAR* pCStr;
	CONST CSC_SIZE_T* pCharIndex;
	CSC_CodePointType currentCPT;

	if (CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE) != CSC_STATUS_SUCCESS)
		return NULL;

	if (pThis->validCache)
	{
		pCharIndex = (CONST CSC_SIZE_T*)CSC_DynamicArrayAccessElement(&pThis->indexArray, codePointIndex);

		if (!pCharIndex || *pCharIndex >= CSC_MAXIMUM_STRING_CHARS_WIDE_STRING)
			return NULL;

		pResult = (CSC_WCHAR*)CSC_DynamicArrayAccessElement(&pThis->dynamicArray, *pCharIndex);

		if (pArrayCharIndex && pResult)
			*pArrayCharIndex = *pCharIndex;

		return pResult;
	}
	else
	{
		pCStr = (CONST CSC_WCHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray);

		if (!pCStr)
			return NULL;

		arrayStringLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1;

		if (!arrayStringLength || arrayStringLength > CSC_MAXIMUM_STRING_CHARS_WIDE_STRING)
			return NULL;

		for (iterator = (CSC_SIZE_T)0, codePointIterator = (CSC_SIZE_T)0; iterator < arrayStringLength; ++iterator, ++codePointIterator)
		{
			if (!pCStr[iterator])
				return NULL;

			currentCPT = CSC_StringUtilsGetCodePointType(pCStr[iterator]);

			if (currentCPT == csc_cpt_invalid || currentCPT == csc_cpt_surrogate_low)
				return NULL;

			if (currentCPT != csc_cpt_bmp && (iterator >= arrayStringLength - (CSC_SIZE_T)1 || !CSC_StringUtilsIsSurrogatePair(pCStr[iterator], pCStr[iterator + (CSC_SIZE_T)1])))
				return NULL;

			if (codePointIterator == codePointIndex)
			{
				if (pArrayCharIndex)
					*pArrayCharIndex = iterator;

				return (CSC_WCHAR*)&pCStr[iterator];
			}
				
			if (currentCPT != csc_cpt_bmp)
				++iterator;
		}

		return NULL;
	}
}


CSC_STATUS CSCMETHOD CSC_WStringAssignSequence(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements)
{
	return CSC_WStringAssignSequenceImpl(pThis, pArray, numOfElements, (CSC_BOOLEAN)TRUE);
}

static CSC_STATUS CSCMETHOD CSC_WStringAssignSequenceImpl(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_BOOLEAN validate)
{
	CSC_WString stringBuffer;
	CSC_STATUS status = CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_WStringInitializeSequenceImpl(&stringBuffer, pArray, numOfElements, CSC_DynamicArrayGetAllocator(&pThis->dynamicArray), validate);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_WStringCopy(pThis, &stringBuffer);
	CSC_WStringDestroy(&stringBuffer);

	return status;
}

CSC_STATUS CSCMETHOD CSC_WStringAssignCString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr)
{
	CONST CSC_SIZE_T length = CSC_StringUtilsStrWideCharLenWideString(pCStr, CSC_MAXIMUM_STRING_CHARS_WIDE_STRING, (CSC_BOOLEAN)TRUE, NULL);

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringAssignSequenceImpl(pThis, pCStr, length, (CSC_BOOLEAN)FALSE);
}

CSC_STATUS CSCMETHOD CSC_WStringCopy(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pSrc)
{
	CSC_STATUS status = CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis == pSrc)
		return CSC_STATUS_SUCCESS;

	status = CSC_WStringIsValid(pSrc, (CSC_BOOLEAN)TRUE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayCopy(&pThis->dynamicArray, &pSrc->dynamicArray);

	if (status == CSC_STATUS_SUCCESS)
	{
		pThis->validCache = (CSC_BOOLEAN)FALSE;
		CSC_DynamicArrayErase(&pThis->indexArray);

		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = CSC_WStringGetLength(pSrc);

			if (pThis->pIterator->iterationValid)
				CSC_IteratorInvalidateIteration(pThis->pIterator);
		}
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_WStringMove(_Inout_ CSC_WString* CONST pThis, _Inout_ CSC_WString* CONST pSrc)
{
	CONST CSC_STATUS status = CSC_WStringCopy(pThis, pSrc);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (pThis == pSrc)
		return CSC_STATUS_SUCCESS;

	CSC_WStringErase(pSrc);

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_WStringFill(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfCodePoints, _In_ CONST CSC_DWORD codePointValue)
{
	CSC_CodePointType currentCPT;
	CSC_DynamicArray arrayBuffer;
	CSC_STATUS status = CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!numOfCodePoints || numOfCodePoints > CSC_MAXIMUM_STRING_LENGTH_WIDE_STRING || !codePointValue)
		return CSC_STATUS_INVALID_PARAMETER;

	currentCPT = CSC_StringUtilsGetCodePointType((CSC_WCHAR)codePointValue);

	if (currentCPT == csc_cpt_invalid || (currentCPT != csc_cpt_bmp && !CSC_StringUtilsIsSurrogatePair((CSC_WCHAR)codePointValue, (CSC_WCHAR)(codePointValue >> (CSC_DWORD)0x10))))
		return CSC_STATUS_INVALID_PARAMETER;

	if (currentCPT != csc_cpt_bmp)
	{
		status = CSC_DynamicArrayInitializeWithSize(&arrayBuffer, CSC_DynamicArrayGetElementSize(&pThis->dynamicArray), numOfCodePoints << (CSC_SIZE_T)1, CSC_DynamicArrayGetAllocator(&pThis->dynamicArray));

		if (status != CSC_STATUS_SUCCESS)
			return status;

		status = CSC_MemoryUtilsSetArrayValue(CSC_DynamicArrayFront(&arrayBuffer), (CSC_PCVOID)&codePointValue, sizeof(codePointValue), numOfCodePoints);
	}
	else
	{
		status = CSC_DynamicArrayInitializeWithValue(&arrayBuffer, CSC_DynamicArrayGetElementSize(&pThis->dynamicArray), numOfCodePoints, (CSC_PCVOID)&codePointValue, CSC_DynamicArrayGetAllocator(&pThis->dynamicArray));
	}

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayPushValue(&arrayBuffer, NULL);

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_DynamicArrayDestroy(&arrayBuffer);
		return status;
	}

	status = CSC_DynamicArrayCopy(&pThis->dynamicArray, &arrayBuffer);
	CSC_DynamicArrayDestroy(&arrayBuffer);

	if (status == CSC_STATUS_SUCCESS)
	{
		pThis->validCache = (CSC_BOOLEAN)FALSE;
		CSC_DynamicArrayErase(&pThis->indexArray);

		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = numOfCodePoints;

			if (pThis->pIterator->iterationValid)
				CSC_IteratorInvalidateIteration(pThis->pIterator);
		}
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_WStringFillRange(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T firstCodePointIndex, _In_ CONST CSC_SIZE_T numOfCodePoints, _In_ CONST CSC_DWORD codePointValue)
{
	CSC_DynamicArray arrayBuffer;
	CSC_CodePointType currentCPT;
	CSC_STATUS status = CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (!numOfCodePoints || !codePointValue)
		return CSC_STATUS_INVALID_PARAMETER;

	currentCPT = CSC_StringUtilsGetCodePointType((CSC_WCHAR)codePointValue);

	if (currentCPT == csc_cpt_invalid || (currentCPT != csc_cpt_bmp && !CSC_StringUtilsIsSurrogatePair((CSC_WCHAR)codePointValue, (CSC_WCHAR)(codePointValue >> (CSC_DWORD)0x10))))
		return CSC_STATUS_INVALID_PARAMETER;

	if (currentCPT != csc_cpt_bmp)
	{
		status = CSC_DynamicArrayInitializeWithSize(&arrayBuffer, CSC_DynamicArrayGetElementSize(&pThis->dynamicArray), numOfCodePoints << (CSC_SIZE_T)1, CSC_DynamicArrayGetAllocator(&pThis->dynamicArray));

		if (status != CSC_STATUS_SUCCESS)
			return status;

		status = CSC_MemoryUtilsSetArrayValue(CSC_DynamicArrayFront(&arrayBuffer), (CSC_PCVOID)&codePointValue, sizeof(codePointValue), numOfCodePoints);
	}
	else
	{
		status = CSC_DynamicArrayInitializeWithValue(&arrayBuffer, CSC_DynamicArrayGetElementSize(&pThis->dynamicArray), numOfCodePoints, (CSC_PCVOID)&codePointValue, CSC_DynamicArrayGetAllocator(&pThis->dynamicArray));
	}

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_DynamicArrayDestroy(&arrayBuffer);
		return status;
	}

	status = CSC_WStringReplaceSequence(pThis, firstCodePointIndex, CSC_DynamicArrayGetSize(&arrayBuffer), (CSC_WCHAR*)CSC_DynamicArrayFront(&arrayBuffer));
	CSC_DynamicArrayDestroy(&arrayBuffer);

	return status;
}


CSC_STATUS CSCMETHOD CSC_WStringReplaceSequence(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T replaceCodePointIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray)
{
	CSC_STATUS status;
	CSC_WString stringBuffer;
	CSC_SIZE_T codePointLength;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pThis);

	if (pThis->pIterator)
		pThis->pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length)
	{
		if (pThis->pIterator && pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);

		return CSC_STATUS_INVALID_PARAMETER;
	}

	if (CSC_WStringIsValidSequence(pArray, numOfElements, &codePointLength) != CSC_STATUS_SUCCESS)
		return CSC_STATUS_INVALID_PARAMETER;

	if (replaceCodePointIndex + codePointLength > length)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_WStringInitializeCopy(&stringBuffer, pThis, CSC_DynamicArrayGetAllocator(&pThis->dynamicArray));

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_WStringRemoveRange(&stringBuffer, replaceCodePointIndex, codePointLength);

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_WStringDestroy(&stringBuffer);
		return status;
	}

	status = CSC_WStringInsertSequenceImpl(&stringBuffer, replaceCodePointIndex, numOfElements, pArray, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_WStringDestroy(&stringBuffer);
		return status;
	}

	status = CSC_WStringCopy(pThis, &stringBuffer);
	CSC_WStringDestroy(&stringBuffer);

	return status;
}

CSC_STATUS CSCMETHOD CSC_WStringReplaceCString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T replaceCodePointIndex, _In_ CONST CSC_WCHAR* CONST pCStr)
{
	CONST CSC_SIZE_T charLength = CSC_StringUtilsStrWideCharLenWideString(pCStr, CSC_MAXIMUM_STRING_CHARS_WIDE_STRING, (CSC_BOOLEAN)TRUE, NULL);

	if (charLength == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringReplaceSequence(pThis, replaceCodePointIndex, charLength, pCStr);
}

CSC_STATUS CSCMETHOD CSC_WStringReplaceString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T replaceCodePointIndex, _In_ CONST CSC_WString* CONST pOther)
{
	CONST CSC_WCHAR* CONST pCStr = CSC_WStringGetData(pOther);

	if (!pCStr)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringReplaceCString(pThis, replaceCodePointIndex, pCStr);
}


CSC_STATUS CSCMETHOD CSC_WStringInsertSequence(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T insertCodePointIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray)
{
	return CSC_WStringInsertSequenceImpl(pThis, insertCodePointIndex, numOfElements, pArray, (CSC_BOOLEAN)TRUE);
}

static CSC_STATUS CSCMETHOD CSC_WStringInsertSequenceImpl(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T insertCodePointIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_BOOLEAN validate)
{
	CSC_STATUS status;
	CSC_SIZE_T insertIndex, charLength, arrayStringLength, codePointLength;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pThis);

	if (pThis->pIterator)
		pThis->pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH)
	{
		if (pThis->pIterator && pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);

		return CSC_STATUS_INVALID_PARAMETER;
	}

	insertIndex = CSC_STRING_INVALID_INDEX;
	codePointLength = CSC_STRING_INVALID_LENGTH;

	if (insertCodePointIndex > length || (validate && CSC_WStringIsValidSequence(pArray, numOfElements, &codePointLength) != CSC_STATUS_SUCCESS) || (insertCodePointIndex != length && !CSC_WStringLazyAccessCodePointImpl(pThis, insertCodePointIndex, &insertIndex)))
		return CSC_STATUS_INVALID_PARAMETER;

	arrayStringLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1;

	if (insertCodePointIndex == length)
		insertIndex = arrayStringLength;

	charLength = CSC_StringUtilsStrWideCharLenWideString((CSC_WCHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), arrayStringLength, (CSC_BOOLEAN)TRUE, NULL);

	if (charLength != arrayStringLength || insertIndex > charLength || charLength + numOfElements > CSC_MAXIMUM_STRING_CHARS_WIDE_STRING)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayInsertRange(&pThis->dynamicArray, insertIndex, (CSC_PCVOID)pArray, numOfElements);

	if (status == CSC_STATUS_SUCCESS)
	{
		pThis->validCache = (CSC_BOOLEAN)FALSE;
		CSC_DynamicArrayErase(&pThis->indexArray);

		if (pThis->pIterator)
		{
			if (codePointLength == CSC_STRING_INVALID_LENGTH && CSC_StringUtilsStrWideCharLenWideString(pArray, numOfElements, (CSC_BOOLEAN)FALSE, &codePointLength) != numOfElements)
			{
				pThis->pIterator->elementCount = (CSC_SIZE_T)0;

				if (pThis->pIterator->iterationValid)
					CSC_IteratorInvalidateIteration(pThis->pIterator);
			}
			else
			{
				pThis->pIterator->elementCount += codePointLength;

				if (pThis->pIterator->iterationValid)
				{
					if (pThis->pIterator->currentIndex >= pThis->pIterator->elementCount)
					{
						CSC_IteratorInvalidateIteration(pThis->pIterator);
					}
					else 
					{
						if (pThis->pIterator->currentIndex >= insertCodePointIndex)
							pThis->pIterator->currentIndex += codePointLength;

						pThis->pIterator->pCurrElement = CSC_WStringLazyAccessCodePoint(pThis, pThis->pIterator->currentIndex);

						if (!pThis->pIterator->pCurrElement)
							CSC_IteratorInvalidateIteration(pThis->pIterator);
					}
				}
			}
		}
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_WStringInsertCString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T insertCodePointIndex, _In_ CONST CSC_WCHAR* CONST pCStr)
{
	CONST CSC_SIZE_T charLength = CSC_StringUtilsStrWideCharLenWideString(pCStr, CSC_MAXIMUM_STRING_CHARS_WIDE_STRING, (CSC_BOOLEAN)TRUE, NULL);

	if (charLength == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringInsertSequenceImpl(pThis, insertCodePointIndex, charLength, pCStr, (CSC_BOOLEAN)FALSE);
}

CSC_STATUS CSCMETHOD CSC_WStringInsertString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T insertCodePointIndex, _In_ CONST CSC_WString* CONST pOther)
{
	CONST CSC_WCHAR* CONST pCStr = CSC_WStringGetData(pOther);

	if (!pCStr)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringInsertCString(pThis, insertCodePointIndex, pCStr);
}


CSC_STATUS CSCMETHOD CSC_WStringAppendSequence(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray)
{
	CONST CSC_SIZE_T insertCodePointIndex = CSC_WStringGetLength(pThis);

	if (insertCodePointIndex == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringInsertSequence(pThis, insertCodePointIndex, numOfElements, pArray);
}

CSC_STATUS CSCMETHOD CSC_WStringAppendCString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr)
{
	CONST CSC_SIZE_T insertCodePointIndex = CSC_WStringGetLength(pThis);

	if (insertCodePointIndex == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringInsertCString(pThis, insertCodePointIndex, pCStr);
}

CSC_STATUS CSCMETHOD CSC_WStringAppendString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther)
{
	CONST CSC_SIZE_T insertCodePointIndex = CSC_WStringGetLength(pThis);

	if (insertCodePointIndex == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringInsertString(pThis, insertCodePointIndex, pOther);
}


CSC_STATUS CSCMETHOD CSC_WStringPrependSequence(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray)
{
	return CSC_WStringInsertSequence(pThis, (CSC_SIZE_T)0, numOfElements, pArray);
}

CSC_STATUS CSCMETHOD CSC_WStringPrependCString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr)
{
	return CSC_WStringInsertCString(pThis, (CSC_SIZE_T)0, pCStr);
}

CSC_STATUS CSCMETHOD CSC_WStringPrependString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther)
{
	return CSC_WStringInsertString(pThis, (CSC_SIZE_T)0, pOther);
}


CSC_STATUS CSCMETHOD CSC_WStringReduceToSubstring(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T firstCodePointIndex, _In_ CONST CSC_SIZE_T lastCodePointIndex)
{
	CSC_STATUS status;
	CSC_SIZE_T firstCharIndex, sizeIndexBuffer;
	CSC_WString stringBuffer;
	CONST CSC_WCHAR* pSequence;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pThis);

	if (pThis->pIterator)
		pThis->pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length)
	{
		if (pThis->pIterator && pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);

		return CSC_STATUS_INVALID_PARAMETER;
	}

	if (firstCodePointIndex > lastCodePointIndex || lastCodePointIndex >= length)
		return CSC_STATUS_INVALID_PARAMETER;

	if (!firstCodePointIndex && lastCodePointIndex == length - (CSC_SIZE_T)1)
		return CSC_STATUS_SUCCESS;

	pSequence = CSC_WStringLazyAccessCodePointImpl(pThis, firstCodePointIndex, &firstCharIndex);

	if (!pSequence)
		return CSC_STATUS_GENERAL_FAILURE;

	if (lastCodePointIndex == length - (CSC_SIZE_T)1)
	{
		sizeIndexBuffer = CSC_StringUtilsStrWideCharLenWideString(pSequence, (length - firstCodePointIndex) << (CSC_SIZE_T)1, (CSC_BOOLEAN)TRUE, NULL);

		if (sizeIndexBuffer == CSC_STRING_INVALID_LENGTH || sizeIndexBuffer < (length - firstCodePointIndex))
			return CSC_STATUS_GENERAL_FAILURE;

		status = CSC_WStringInitializeSequenceImpl(&stringBuffer, (CSC_WCHAR*)CSC_DynamicArrayAccessElement(&pThis->dynamicArray, firstCharIndex), sizeIndexBuffer, CSC_DynamicArrayGetAllocator(&pThis->dynamicArray), (CSC_BOOLEAN)FALSE);
	}
	else
	{
		if (!CSC_WStringLazyAccessCodePointImpl(pThis, lastCodePointIndex + (CSC_SIZE_T)1, &sizeIndexBuffer))
			return CSC_STATUS_GENERAL_FAILURE;

		status = CSC_WStringInitializeSequenceImpl(&stringBuffer, (CSC_WCHAR*)CSC_DynamicArrayAccessElement(&pThis->dynamicArray, firstCharIndex), sizeIndexBuffer - firstCharIndex, CSC_DynamicArrayGetAllocator(&pThis->dynamicArray), (CSC_BOOLEAN)FALSE);
	}

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_WStringCopy(pThis, &stringBuffer);
	CSC_WStringDestroy(&stringBuffer);

	return status;
}

CSC_STATUS CSCMETHOD CSC_WStringReduceToSubstringRange(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T firstCodePointIndex, _In_ CONST CSC_SIZE_T numOfCodePoints)
{
	if (!numOfCodePoints)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringReduceToSubstring(pThis, firstCodePointIndex, firstCodePointIndex + numOfCodePoints - (CSC_SIZE_T)1);
}

CSC_STATUS CSCMETHOD CSC_WStringRemove(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T firstCodePointIndex, _In_ CONST CSC_SIZE_T lastCodePointIndex)
{
	CSC_SIZE_T charLength, arrayStringLength, firstCharIndex, lastCharIndex, numOfCodePoints, codePointLength;
	CSC_STATUS status = CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	arrayStringLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1;
	charLength = CSC_StringUtilsStrWideCharLenWideString((CSC_WCHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), arrayStringLength, (CSC_BOOLEAN)TRUE, &codePointLength);

	if (charLength != arrayStringLength)
	{
		if (pThis->pIterator)
		{
			pThis->pIterator->elementCount = (CSC_SIZE_T)0;

			if (pThis->pIterator->iterationValid)
				CSC_IteratorInvalidateIteration(pThis->pIterator);
		}

		return CSC_STATUS_INVALID_PARAMETER;
	}
		
	if (pThis->pIterator)
		pThis->pIterator->elementCount = codePointLength;

	if (firstCodePointIndex > lastCodePointIndex || !CSC_WStringLazyAccessCodePointImpl(pThis, firstCodePointIndex, &firstCharIndex) || !CSC_WStringLazyAccessCodePointImpl(pThis, lastCodePointIndex, &lastCharIndex))
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_DynamicArrayRemoveRange(&pThis->dynamicArray, firstCharIndex, lastCharIndex - firstCharIndex + (CSC_SIZE_T)1);

	if (status == CSC_STATUS_SUCCESS)
	{
		pThis->validCache = (CSC_BOOLEAN)FALSE;
		CSC_DynamicArrayErase(&pThis->indexArray);

		if (pThis->pIterator)
		{
			numOfCodePoints = lastCodePointIndex - firstCodePointIndex + (CSC_SIZE_T)1;
			pThis->pIterator->elementCount -= numOfCodePoints;

			if (pThis->pIterator->iterationValid)
			{
				if (pThis->pIterator->currentIndex >= firstCodePointIndex && pThis->pIterator->currentIndex <= lastCodePointIndex)
				{
					CSC_IteratorInvalidateIteration(pThis->pIterator);
				}
				else
				{
					if (pThis->pIterator->currentIndex >= firstCodePointIndex)
						pThis->pIterator->currentIndex -= numOfCodePoints;

					pThis->pIterator->pCurrElement = CSC_WStringLazyAccessCodePoint(pThis, pThis->pIterator->currentIndex);

					if (!pThis->pIterator->pCurrElement)
						CSC_IteratorInvalidateIteration(pThis->pIterator);
				}
			}
		}
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_WStringRemoveRange(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T removeCodePointIndex, _In_ CONST CSC_SIZE_T numOfCodePoints)
{
	if (!numOfCodePoints)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringRemove(pThis, removeCodePointIndex, removeCodePointIndex + numOfCodePoints - (CSC_SIZE_T)1);
}


CSC_STATUS CSCMETHOD CSC_WStringReverse(_Inout_ CSC_WString* CONST pThis)
{
	CSC_CodePointType currentCPT;
	CONST CSC_WCHAR* pCurrChar;
	CSC_DynamicArray arrayBuffer;
	CSC_SIZE_T length, iterator;
	CSC_STATUS status = CSC_WStringPrepareForIndexing(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	length = CSC_WStringGetLength(pThis);

	if (pThis->pIterator)
		pThis->pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length)
	{
		if (pThis->pIterator && pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);

		return CSC_STATUS_INVALID_PARAMETER;
	}

	status = CSC_DynamicArrayInitialize(&arrayBuffer, CSC_DynamicArrayGetElementSize(&pThis->dynamicArray), CSC_DynamicArrayGetAllocator(&pThis->dynamicArray));

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayReserve(&arrayBuffer, length << (CSC_SIZE_T)1);

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_DynamicArrayDestroy(&arrayBuffer);
		return status;
	}

	for (iterator = length; iterator > (CSC_SIZE_T)0; --iterator)
	{
		pCurrChar = CSC_WStringLazyAccessCodePoint(pThis, iterator - (CSC_SIZE_T)1);

		if (!pCurrChar)
		{
			CSC_DynamicArrayDestroy(&arrayBuffer);
			return CSC_STATUS_GENERAL_FAILURE;
		}

		currentCPT = CSC_StringUtilsGetCodePointType(*pCurrChar);

		if (currentCPT == csc_cpt_invalid || currentCPT == csc_cpt_surrogate_low || (currentCPT != csc_cpt_bmp && !CSC_StringUtilsIsSurrogatePair(*pCurrChar, pCurrChar[(CSC_SIZE_T)1])))
		{
			CSC_DynamicArrayDestroy(&arrayBuffer);
			return CSC_STATUS_GENERAL_FAILURE;
		}

		status = CSC_DynamicArrayPushValue(&arrayBuffer, (CSC_PCVOID)pCurrChar);

		if (status != CSC_STATUS_SUCCESS)
		{
			CSC_DynamicArrayDestroy(&arrayBuffer);
			return CSC_STATUS_GENERAL_FAILURE;
		}

		if (currentCPT != csc_cpt_bmp)
		{
			status = CSC_DynamicArrayPushValue(&arrayBuffer, (CSC_PCVOID)&pCurrChar[(CSC_SIZE_T)1]);

			if (status != CSC_STATUS_SUCCESS)
			{
				CSC_DynamicArrayDestroy(&arrayBuffer);
				return CSC_STATUS_GENERAL_FAILURE;
			}
		}
	}

	status = CSC_DynamicArrayPushValue(&arrayBuffer, NULL);

	if (status != CSC_STATUS_SUCCESS)
	{
		CSC_DynamicArrayDestroy(&arrayBuffer);
		return CSC_STATUS_GENERAL_FAILURE;
	}

	status = CSC_DynamicArrayCopy(&pThis->dynamicArray, &arrayBuffer);
	CSC_DynamicArrayDestroy(&arrayBuffer);

	if (status == CSC_STATUS_SUCCESS)
	{
		pThis->validCache = (CSC_BOOLEAN)FALSE;
		CSC_DynamicArrayErase(&pThis->indexArray);

		if (pThis->pIterator && pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);
	}

	return status;
}


CSC_SIZE_T CSCMETHOD CSC_WStringFindSequence(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray)
{
	return CSC_WStringFindSequenceImpl(pThis, numOfElements, pArray, (CSC_BOOLEAN)FALSE, (CSC_BOOLEAN)FALSE);
}

static CSC_SIZE_T CSCMETHOD CSC_WStringFindSequenceImpl(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_BOOLEAN reverseDir, _In_ CONST CSC_BOOLEAN reverseIndexDir)
{
	CSC_BOOLEAN match;
	CSC_CodePointType currentCPT, sequenceCurrentCPT;
	CSC_SIZE_T iterator, sequenceLength, charIterator, codePointIterator;
	CONST CSC_WCHAR* pCurrElement;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pThis);

	if (pThis->pIterator)
		pThis->pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length)
	{
		if (pThis->pIterator && pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);

		return CSC_STRING_INVALID_INDEX;
	}

	if (CSC_WStringIsValidSequence(pArray, numOfElements, &sequenceLength) != CSC_STATUS_SUCCESS || sequenceLength > length || CSC_WStringPrepareForIndexing(pThis) != CSC_STATUS_SUCCESS)
		return CSC_STRING_INVALID_INDEX;

	for (iterator = (CSC_SIZE_T)0; iterator <= length - sequenceLength; ++iterator)
	{
		match = (CSC_BOOLEAN)TRUE;

		for (charIterator = (CSC_SIZE_T)0, codePointIterator = (CSC_SIZE_T)0; charIterator < numOfElements; ++charIterator, ++codePointIterator)
		{
			if (codePointIterator >= sequenceLength)
			{
				match = (CSC_BOOLEAN)FALSE;
				break;
			}

			if (reverseDir)
				pCurrElement = CSC_WStringLazyAccessCodePoint(pThis, length - sequenceLength - iterator + codePointIterator);
			else
				pCurrElement = CSC_WStringLazyAccessCodePoint(pThis, iterator + codePointIterator);

			if (!pCurrElement)
				return CSC_STRING_INVALID_INDEX;

			currentCPT = CSC_StringUtilsGetCodePointType(*pCurrElement);
			sequenceCurrentCPT = CSC_StringUtilsGetCodePointType(pArray[charIterator]);

			if (currentCPT == csc_cpt_invalid || sequenceCurrentCPT == csc_cpt_invalid || currentCPT == csc_cpt_surrogate_low || sequenceCurrentCPT == csc_cpt_surrogate_low)
				return CSC_STRING_INVALID_INDEX;

			if (currentCPT != sequenceCurrentCPT || *pCurrElement != pArray[charIterator])
			{
				match = (CSC_BOOLEAN)FALSE;
				break;
			}

			if (sequenceCurrentCPT == csc_cpt_surrogate_high)
			{
				if (charIterator >= numOfElements - (CSC_SIZE_T)1)
					return CSC_STRING_INVALID_INDEX;

				currentCPT = CSC_StringUtilsGetCodePointType(pCurrElement[(CSC_SIZE_T)1]);
				sequenceCurrentCPT = CSC_StringUtilsGetCodePointType(pArray[charIterator + (CSC_SIZE_T)1]);

				if (currentCPT != csc_cpt_surrogate_low || sequenceCurrentCPT != csc_cpt_surrogate_low)
					return CSC_STRING_INVALID_INDEX;

				if (pCurrElement[(CSC_SIZE_T)1] != pArray[charIterator + (CSC_SIZE_T)1])
				{
					match = (CSC_BOOLEAN)FALSE;
					break;
				}

				++charIterator;
			}
		}

		if (!match)
			continue;

		return (reverseDir == reverseIndexDir) ? iterator : length - sequenceLength - iterator;
	}

	return CSC_STRING_INVALID_INDEX;
}

CSC_SIZE_T CSCMETHOD CSC_WStringFindCString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr)
{
	CONST CSC_SIZE_T charLength = CSC_StringUtilsStrWideCharLenWideString(pCStr, CSC_MAXIMUM_STRING_CHARS_WIDE_STRING, (CSC_BOOLEAN)TRUE, NULL);

	if (charLength == CSC_STRING_INVALID_LENGTH)
		return CSC_STRING_INVALID_INDEX;

	return CSC_WStringFindSequence(pThis, charLength, pCStr);
}

CSC_SIZE_T CSCMETHOD CSC_WStringFindString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther)
{
	CONST CSC_WCHAR* CONST pCStr = CSC_WStringGetData(pOther);

	if (!pCStr)
		return CSC_STRING_INVALID_INDEX;

	return CSC_WStringFindCString(pThis, pCStr);
}


CSC_SIZE_T CSCMETHOD CSC_WStringRFindSequence(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray)
{
	return CSC_WStringFindSequenceImpl(pThis, numOfElements, pArray, (CSC_BOOLEAN)TRUE, (CSC_BOOLEAN)FALSE);
}

CSC_SIZE_T CSCMETHOD CSC_WStringRFindCString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr)
{
	CONST CSC_SIZE_T charLength = CSC_StringUtilsStrWideCharLenWideString(pCStr, CSC_MAXIMUM_STRING_CHARS_WIDE_STRING, (CSC_BOOLEAN)TRUE, NULL);

	if (charLength == CSC_STRING_INVALID_LENGTH)
		return CSC_STRING_INVALID_INDEX;

	return CSC_WStringRFindSequence(pThis, charLength, pCStr);
}

CSC_SIZE_T CSCMETHOD CSC_WStringRFindString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther)
{
	CONST CSC_WCHAR* CONST pCStr = CSC_WStringGetData(pOther);

	if (!pCStr)
		return CSC_STRING_INVALID_INDEX;

	return CSC_WStringRFindCString(pThis, pCStr);
}


CSC_STATUS CSCMETHOD CSC_WStringContainsSequence(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray)
{
	return ((CSC_WStringFindSequence(pThis, numOfElements, pArray) != CSC_STRING_INVALID_INDEX) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_STATUS CSCMETHOD CSC_WStringContainsCString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr)
{
	return ((CSC_WStringFindCString(pThis, pCStr) != CSC_STRING_INVALID_INDEX) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_STATUS CSCMETHOD CSC_WStringContainsString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther)
{
	return ((CSC_WStringFindString(pThis, pOther) != CSC_STRING_INVALID_INDEX) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}


CSC_STATUS CSCMETHOD CSC_WStringStartsWithSequence(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray)
{
	return ((CSC_WStringFindSequence(pThis, numOfElements, pArray) == (CSC_SIZE_T)0) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_STATUS CSCMETHOD CSC_WStringStartsWithCString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr)
{
	return ((CSC_WStringFindCString(pThis, pCStr) == (CSC_SIZE_T)0) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_STATUS CSCMETHOD CSC_WStringStartsWithString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther)
{
	return ((CSC_WStringFindString(pThis, pOther) == (CSC_SIZE_T)0) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}


CSC_STATUS CSCMETHOD CSC_WStringEndsWithSequence(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray)
{
	return ((CSC_WStringFindSequenceImpl(pThis, numOfElements, pArray, (CSC_BOOLEAN)TRUE, (CSC_BOOLEAN)TRUE) == (CSC_SIZE_T)0) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_STATUS CSCMETHOD CSC_WStringEndsWithCString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr)
{
	CONST CSC_SIZE_T charLength = CSC_StringUtilsStrWideCharLenWideString(pCStr, CSC_MAXIMUM_STRING_CHARS_WIDE_STRING, (CSC_BOOLEAN)TRUE, NULL);

	if (charLength == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringEndsWithSequence(pThis, charLength, pCStr);
}

CSC_STATUS CSCMETHOD CSC_WStringEndsWithString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther)
{
	CONST CSC_WCHAR* CONST pCStr = CSC_WStringGetData(pOther);

	if (!pCStr)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringEndsWithCString(pThis, pCStr);
}


CSC_STATUS CSCMETHOD CSC_WStringCompareWithSequence(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray)
{
	CSC_CodePointType currentCPT, sequenceCurrentCPT;
	CSC_SIZE_T sequenceLength, charIterator, codePointIterator;
	CONST CSC_WCHAR* pCurrElement;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pThis);

	if (pThis->pIterator)
		pThis->pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length)
	{
		if (pThis->pIterator && pThis->pIterator->iterationValid)
			CSC_IteratorInvalidateIteration(pThis->pIterator);

		return CSC_STATUS_INVALID_PARAMETER;
	}

	if (CSC_WStringIsValidSequence(pArray, numOfElements, &sequenceLength) != CSC_STATUS_SUCCESS || (strict && length != sequenceLength) || CSC_WStringPrepareForIndexing(pThis) != CSC_STATUS_SUCCESS)
		return CSC_STATUS_GENERAL_FAILURE;

	for (charIterator = (CSC_SIZE_T)0, codePointIterator = (CSC_SIZE_T)0; charIterator < numOfElements; ++charIterator, ++codePointIterator)
	{
		if (codePointIterator >= ((length < sequenceLength) ? length : sequenceLength))
			break;

		pCurrElement = CSC_WStringLazyAccessCodePoint(pThis, codePointIterator);

		if (!pCurrElement)
			return CSC_STATUS_GENERAL_FAILURE;

		currentCPT = CSC_StringUtilsGetCodePointType(*pCurrElement);
		sequenceCurrentCPT = CSC_StringUtilsGetCodePointType(pArray[charIterator]);

		if (currentCPT == csc_cpt_invalid || currentCPT == csc_cpt_surrogate_low || currentCPT != sequenceCurrentCPT || *pCurrElement != pArray[charIterator])
			return CSC_STATUS_GENERAL_FAILURE;

		if (sequenceCurrentCPT == csc_cpt_surrogate_high)
		{
			if (charIterator == numOfElements - (CSC_SIZE_T)1 || pCurrElement[(CSC_SIZE_T)1] != pArray[charIterator + (CSC_SIZE_T)1] || CSC_StringUtilsGetCodePointType(pCurrElement[(CSC_SIZE_T)1]) != csc_cpt_surrogate_low)
				return CSC_STATUS_GENERAL_FAILURE;

			++charIterator;
		}
	}

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_WStringCompareWithCString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_WCHAR* CONST pCStr)
{
	CONST CSC_SIZE_T charLength = CSC_StringUtilsStrWideCharLenWideString(pCStr, CSC_MAXIMUM_STRING_CHARS_WIDE_STRING, (CSC_BOOLEAN)TRUE, NULL);

	if (charLength == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringCompareWithSequence(pThis, strict, charLength, pCStr);
}

CSC_STATUS CSCMETHOD CSC_WStringCompareWithString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_WString* CONST pOther)
{
	CONST CSC_WCHAR* CONST pCStr = CSC_WStringGetData(pOther);

	if (!pCStr)
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_WStringCompareWithCString(pThis, strict, pCStr);
}


CSC_WCHAR* CSCMETHOD CSC_WStringGetData(_In_ CONST CSC_WString* CONST pThis)
{
	return ((CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE) == CSC_STATUS_SUCCESS) ? (CSC_WCHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray) : NULL);
}


CSC_STATUS CSCMETHOD CSC_WStringIsEmpty(_In_ CONST CSC_WString* CONST pThis)
{
	return ((CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE) != CSC_STATUS_SUCCESS || *(CSC_WCHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray) == (CSC_WCHAR)0) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_STATUS CSCMETHOD CSC_WStringIsValid(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_BOOLEAN checkLength)
{
	CSC_SIZE_T arrayLength;

	if (!pThis || pThis->pVTableIterator != (CSC_PCVOID)g_wStringVirtualTable || CSC_DynamicArrayGetElementSize(&pThis->dynamicArray) != sizeof(CSC_WCHAR) || CSC_DynamicArrayGetElementSize(&pThis->indexArray) != sizeof(CSC_SIZE_T))
		return CSC_STATUS_INVALID_PARAMETER;

	arrayLength = CSC_DynamicArrayGetSize(&pThis->dynamicArray);

	if (!arrayLength || arrayLength - (CSC_SIZE_T)1 > CSC_MAXIMUM_STRING_CHARS_WIDE_STRING || (checkLength && CSC_StringUtilsStrWideCharLenWideString((CSC_WCHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), arrayLength - (CSC_SIZE_T)1, (CSC_BOOLEAN)TRUE, NULL) != arrayLength - (CSC_SIZE_T)1))
		return CSC_STATUS_INVALID_PARAMETER;

	return CSC_STATUS_SUCCESS;
}


CSC_SIZE_T CSCMETHOD CSC_WStringGetLength(_In_ CONST CSC_WString* CONST pThis)
{
	if (CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE) != CSC_STATUS_SUCCESS)
		return CSC_STRING_INVALID_LENGTH;

	return ((pThis->validCache) ? CSC_DynamicArrayGetSize(&pThis->indexArray) : CSC_StringUtilsStrLenWideString((CSC_WCHAR*)CSC_DynamicArrayFront(&pThis->dynamicArray), CSC_DynamicArrayGetSize(&pThis->dynamicArray) - (CSC_SIZE_T)1, (CSC_BOOLEAN)TRUE));
}

CSC_SIZE_T CSCMETHOD CSC_WStringGetCapacityChars(_In_ CONST CSC_WString* CONST pThis)
{
	return ((CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE) == CSC_STATUS_SUCCESS) ? CSC_DynamicArrayGetCapacity(&pThis->dynamicArray) : (CSC_SIZE_T)0);
}

CSC_SIZE_T CSCMETHOD CSC_WStringGetMaxLength(_In_ CONST CSC_WString* CONST pThis)
{
	return ((CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE) == CSC_STATUS_SUCCESS) ? CSC_MAXIMUM_STRING_LENGTH_WIDE_STRING : (CSC_SIZE_T)0);
}

CSC_PVOID CSCMETHOD CSC_WStringGetAllocator(_In_ CONST CSC_WString* CONST pThis)
{
	return ((CSC_WStringIsValid(pThis, (CSC_BOOLEAN)FALSE) == CSC_STATUS_SUCCESS) ? CSC_DynamicArrayGetAllocator(&pThis->dynamicArray) : NULL);
}


static CSC_STATUS CSCMETHOD CSC_WStringIteratorRegister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CSC_WString* CONST pWString = (CSC_WString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pWString);

	if (!pWString || length == CSC_STRING_INVALID_LENGTH)
		return CSC_STATUS_INVALID_PARAMETER;

	if (!pIterator)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pWString->pIterator)
		return CSC_STATUS_GENERAL_FAILURE;

	elementSize = CSC_DynamicArrayGetElementSize(&pWString->dynamicArray);

	if (elementSize != sizeof(CSC_WCHAR))
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementSize = elementSize;
	pIterator->elementCount = length;
	pIterator->swapByIndex = (CSC_BOOLEAN)TRUE;
	pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
	pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	pIterator->pCurrElement = NULL;
	pIterator->pContainer = pContainer;

	pWString->pIterator = pIterator;

	return CSC_STATUS_SUCCESS;
}

static CSC_STATUS CSCMETHOD CSC_WStringIteratorUnregister(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_WString* CONST pWString = (CSC_WString* CONST)pContainer;
	CONST CSC_STATUS status = CSC_WStringIsValid(pWString, (CSC_BOOLEAN)FALSE);

	if (!pWString || status != CSC_STATUS_SUCCESS)
		return status;

	if (!pIterator)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pWString->pIterator != pIterator)
		return CSC_STATUS_GENERAL_FAILURE;

	pWString->pIterator = NULL;
	CSC_IteratorZeroMemory(pIterator);

	return CSC_STATUS_SUCCESS;
}

static CSC_STATUS CSCMETHOD CSC_WStringIteratorFirstElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_WString* CONST pWString = (CONST CSC_WString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pWString);

	if (!pWString)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pWString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_WCHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pWString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->pCurrElement = CSC_DynamicArrayFront(&pWString->dynamicArray);

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

static CSC_STATUS CSCMETHOD CSC_WStringIteratorNextElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_WString* CONST pWString = (CONST CSC_WString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pWString);

	if (!pWString)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pWString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_WCHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pWString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (!pIterator->iterationValid)
		return CSC_STATUS_INVALID_PARAMETER;

	if (length == CSC_STRING_INVALID_LENGTH || !length || pIterator->currentIndex >= length)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return CSC_STATUS_INVALID_PARAMETER;
	}

	++pIterator->currentIndex;
	pIterator->pCurrElement = (CSC_PVOID)CSC_WStringLazyAccessCodePoint(pWString, pIterator->currentIndex);

	if (!pIterator->pCurrElement)
	{
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_STATUS CSCMETHOD CSC_WStringIteratorLastElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_WString* CONST pWString = (CONST CSC_WString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pWString);

	if (!pWString)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pWString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_WCHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pWString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->pCurrElement = (CSC_PVOID)CSC_WStringLazyAccessCodePoint(pWString, length - (CSC_SIZE_T)1);

	if (pIterator->pCurrElement)
	{
		pIterator->iterationValid = (CSC_BOOLEAN)TRUE;
		pIterator->currentIndex = length - (CSC_SIZE_T)1;
	}
	else
	{
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_STATUS CSCMETHOD CSC_WStringIteratorPreviousElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_WString* CONST pWString = (CONST CSC_WString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pWString);

	if (!pWString)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pWString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_WCHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pWString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (!pIterator->iterationValid)
		return CSC_STATUS_INVALID_PARAMETER;

	if (length == CSC_STRING_INVALID_LENGTH || !length || pIterator->currentIndex >= length || !pIterator->currentIndex)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return (!pIterator->currentIndex) ? CSC_STATUS_GENERAL_FAILURE : CSC_STATUS_INVALID_PARAMETER;
	}

	--pIterator->currentIndex;
	pIterator->pCurrElement = (CSC_PVOID)CSC_WStringLazyAccessCodePoint(pWString, pIterator->currentIndex);

	if (!pIterator->pCurrElement)
	{
		pIterator->iterationValid = (CSC_BOOLEAN)FALSE;
		pIterator->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	}

	return ((pIterator->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

static CSC_STATUS CSCMETHOD CSC_WStringIteratorMoveToIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index)
{
	CSC_SIZE_T elementSize;
	CONST CSC_WString* CONST pWString = (CONST CSC_WString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pWString);

	if (!pWString)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pWString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_WCHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pWString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length || index >= length)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->pCurrElement = (CSC_PVOID)CSC_WStringLazyAccessCodePoint(pWString, index);

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

static CSC_SIZE_T CSCMETHOD CSC_WStringIteratorGetCurrentElementIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_WString* CONST pWString = (CONST CSC_WString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pWString);

	if (!pWString)
		return CSC_ITERATOR_INVALID_INDEX;

	elementSize = CSC_DynamicArrayGetElementSize(&pWString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_WCHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pWString)
		return CSC_ITERATOR_INVALID_INDEX;

	pIterator->elementCount = length;

	if (!pIterator->iterationValid)
		return CSC_ITERATOR_INVALID_INDEX;

	if (length == CSC_STRING_INVALID_LENGTH || !length || pIterator->currentIndex >= length)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return CSC_ITERATOR_INVALID_INDEX;
	}

	return pIterator->currentIndex;
}

static CSC_PVOID CSCMETHOD CSC_WStringIteratorGetCurrentElement(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_WString* CONST pWString = (CONST CSC_WString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pWString);

	if (!pWString)
		return NULL;

	elementSize = CSC_DynamicArrayGetElementSize(&pWString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_WCHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pWString)
		return NULL;

	pIterator->elementCount = length;

	if (!pIterator->iterationValid)
		return NULL;

	if (length == CSC_STRING_INVALID_LENGTH || !length || pIterator->currentIndex >= length)
	{
		CSC_IteratorInvalidateIteration(pIterator);
		return NULL;
	}

	return (CSC_PVOID)CSC_WStringLazyAccessCodePoint(pWString, pIterator->currentIndex);
}

static CSC_PVOID CSCMETHOD CSC_WStringIteratorGetElementAt(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index)
{
	CSC_SIZE_T elementSize;
	CONST CSC_WString* CONST pWString = (CONST CSC_WString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pWString);

	if (!pWString)
		return NULL;

	elementSize = CSC_DynamicArrayGetElementSize(&pWString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_WCHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pWString)
		return NULL;

	pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH || !length || index >= length)
		return NULL;

	return (CSC_PVOID)CSC_WStringLazyAccessCodePoint(pWString, index);
}

static CSC_SIZE_T CSCMETHOD CSC_WStringIteratorGetElementCount(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator)
{
	CSC_SIZE_T elementSize;
	CONST CSC_WString* CONST pWString = (CONST CSC_WString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pWString);

	if (!pWString)
		return CSC_ITERATOR_INVALID_LENGTH;

	elementSize = CSC_DynamicArrayGetElementSize(&pWString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_WCHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pWString)
		return CSC_ITERATOR_INVALID_LENGTH;

	pIterator->elementCount = length;

	if (length == CSC_STRING_INVALID_LENGTH)
		return CSC_ITERATOR_INVALID_LENGTH;

	return length;
}

static CSC_STATUS CSCMETHOD CSC_WStringIteratorSwapElementsByIndex(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T secondIndex)
{
	CSC_DWORD buffer;
	CSC_STATUS status;
	CSC_CodePointType lowerCPT, higherCPT;
	CSC_SIZE_T elementSize, lowerIndex, higherIndex, copySize;
	CSC_PVOID pLowerElement, pHigherElement, pDstCopy, pSrcCopy;
	CSC_WString* CONST pWString = (CSC_WString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pWString);

	if (length == CSC_STRING_INVALID_LENGTH || !length)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pWString->dynamicArray);

	if (!pIterator || elementSize != sizeof(CSC_WCHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pWString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (firstIndex >= length || secondIndex >= length)
		return CSC_STATUS_INVALID_PARAMETER;

	if (firstIndex == secondIndex)
		return CSC_STATUS_SUCCESS;

	lowerIndex = (secondIndex > firstIndex) ? firstIndex : secondIndex;
	higherIndex = (secondIndex > firstIndex) ? secondIndex : firstIndex;

	pLowerElement = CSC_WStringLazyAccessCodePoint(pWString, lowerIndex);
	pHigherElement = CSC_WStringLazyAccessCodePoint(pWString, higherIndex);

	if (!pLowerElement || !pHigherElement)
		return CSC_STATUS_GENERAL_FAILURE;

	lowerCPT = CSC_StringUtilsGetCodePointType(*(CONST CSC_WCHAR* CONST)pLowerElement);
	higherCPT = CSC_StringUtilsGetCodePointType(*(CONST CSC_WCHAR* CONST)pHigherElement);

	if (lowerCPT == csc_cpt_invalid || lowerCPT == csc_cpt_surrogate_low || higherCPT == csc_cpt_invalid || higherCPT == csc_cpt_surrogate_low)
		return CSC_STATUS_GENERAL_FAILURE;

	if (lowerCPT == higherCPT)
	{
		if (lowerCPT == csc_cpt_surrogate_high)
		{
			if (CSC_StringUtilsGetCodePointType(((CONST CSC_WCHAR* CONST)pLowerElement)[(CSC_SIZE_T)1]) != csc_cpt_surrogate_low || CSC_StringUtilsGetCodePointType(((CONST CSC_WCHAR* CONST)pHigherElement)[(CSC_SIZE_T)1]) != csc_cpt_surrogate_low)
				return CSC_STATUS_GENERAL_FAILURE;

			buffer = *(CSC_DWORD* CONST)pLowerElement;
			*(CSC_DWORD* CONST)pLowerElement = *(CSC_DWORD* CONST)pHigherElement;
			*(CSC_DWORD* CONST)pHigherElement = buffer;
		}
		else
		{
			buffer = (CSC_DWORD)*(CSC_WCHAR* CONST)pLowerElement;
			*(CSC_WCHAR* CONST)pLowerElement = *(CSC_WCHAR* CONST)pHigherElement;
			*(CSC_WCHAR* CONST)pHigherElement = (CSC_WCHAR)buffer;
		}
	}
	else
	{
		if (lowerCPT == csc_cpt_surrogate_high)
		{
			if (CSC_StringUtilsGetCodePointType(((CONST CSC_WCHAR* CONST)pLowerElement)[(CSC_SIZE_T)1]) != csc_cpt_surrogate_low)
				return CSC_STATUS_GENERAL_FAILURE;

			pDstCopy = (CSC_PVOID)((CONST CSC_WCHAR* CONST)pLowerElement + (CSC_SIZE_T)1);
			pSrcCopy = (CSC_PVOID)((CONST CSC_WCHAR* CONST)pLowerElement + (CSC_SIZE_T)2);
			copySize = (CSC_SIZE_T)((CONST CSC_WCHAR* CONST)pHigherElement - (CONST CSC_WCHAR* CONST)pSrcCopy);
			buffer = *(CSC_DWORD* CONST)pLowerElement;

			if (copySize)
			{
				status = CSC_MemoryUtilsCopyMemory(pDstCopy, (CSC_PCVOID)pSrcCopy, copySize * sizeof(CSC_WCHAR));

				if (status != CSC_STATUS_SUCCESS)
					return status;
			}

			*(CSC_WCHAR* CONST)pLowerElement = *(CONST CSC_WCHAR* CONST)pHigherElement;
			*(CSC_DWORD* CONST)((CONST CSC_WCHAR* CONST)pDstCopy + copySize) = buffer;
		}
		else
		{
			if (CSC_StringUtilsGetCodePointType(((CONST CSC_WCHAR* CONST)pHigherElement)[(CSC_SIZE_T)1]) != csc_cpt_surrogate_low)
				return CSC_STATUS_GENERAL_FAILURE;

			pDstCopy = (CSC_PVOID)((CONST CSC_WCHAR* CONST)pLowerElement + (CSC_SIZE_T)2);
			pSrcCopy = (CSC_PVOID)((CONST CSC_WCHAR* CONST)pLowerElement + (CSC_SIZE_T)1);
			copySize = (CSC_SIZE_T)((CONST CSC_WCHAR* CONST)pHigherElement - (CONST CSC_WCHAR* CONST)pSrcCopy);
			buffer = *(CSC_DWORD* CONST)pHigherElement;

			if (copySize)
			{
				status = CSC_MemoryUtilsCopyMemory(pDstCopy, (CSC_PCVOID)pSrcCopy, copySize * sizeof(CSC_WCHAR));

				if (status != CSC_STATUS_SUCCESS)
					return status;
			}			

			*((CSC_WCHAR* CONST)pDstCopy + copySize) = *(CONST CSC_WCHAR* CONST)pLowerElement;
			*(CSC_DWORD* CONST)pLowerElement = buffer;
		}

		pWString->validCache = (CSC_BOOLEAN)FALSE;
		CSC_DynamicArrayErase(&pWString->indexArray);

		if (pIterator->iterationValid)
		{
			pIterator->pCurrElement = CSC_WStringLazyAccessCodePoint(pWString, pIterator->currentIndex);

			if (!pIterator->pCurrElement)
				CSC_IteratorInvalidateIteration(pIterator);
		}
	}

	return CSC_STATUS_SUCCESS;
}

static CSC_STATUS CSCMETHOD CSC_WStringIteratorSwapElementsByPtr(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond)
{
	CSC_DWORD buffer;
	CSC_STATUS status;
	CSC_SIZE_T elementSize, copySize;
	CSC_CodePointType lowerCPT, higherCPT;
	CSC_PVOID pFront, pBack, pLowerElement, pHigherElement, pDstCopy, pSrcCopy;
	
	CSC_WString* CONST pWString = (CSC_WString* CONST)pContainer;
	CONST CSC_SIZE_T length = CSC_WStringGetLength(pWString);

	if (length == CSC_STRING_INVALID_LENGTH || !length)
		return CSC_STATUS_INVALID_PARAMETER;

	elementSize = CSC_DynamicArrayGetElementSize(&pWString->dynamicArray);
	pFront = (CSC_PVOID)CSC_WStringLazyAccessCodePoint(pWString, (CSC_SIZE_T)0);
	pBack = (CSC_PVOID)CSC_WStringLazyAccessCodePoint(pWString, length - (CSC_SIZE_T)1);

	if (!pIterator || elementSize != sizeof(CSC_WCHAR) || pIterator->elementSize != elementSize || pIterator->pContainer != (CSC_PVOID)pWString)
		return CSC_STATUS_INVALID_PARAMETER;

	pIterator->elementCount = length;

	if (!pFront || !pBack || (CSC_POINTER_TYPE)pFirst < (CSC_POINTER_TYPE)pFront || (CSC_POINTER_TYPE)pFirst >(CSC_POINTER_TYPE)pBack || (CSC_POINTER_TYPE)pSecond < (CSC_POINTER_TYPE)pFront || (CSC_POINTER_TYPE)pSecond >(CSC_POINTER_TYPE)pBack)
		return CSC_STATUS_INVALID_PARAMETER;

	if (pFirst == pSecond)
		return CSC_STATUS_SUCCESS;

	pLowerElement = ((CSC_POINTER_TYPE)pSecond > (CSC_POINTER_TYPE)pFirst) ? pFirst : pSecond;
	pHigherElement = ((CSC_POINTER_TYPE)pSecond > (CSC_POINTER_TYPE)pFirst) ? pSecond : pFirst;

	lowerCPT = CSC_StringUtilsGetCodePointType(*(CONST CSC_WCHAR* CONST)pLowerElement);
	higherCPT = CSC_StringUtilsGetCodePointType(*(CONST CSC_WCHAR* CONST)pHigherElement);

	if (lowerCPT == csc_cpt_invalid || lowerCPT == csc_cpt_surrogate_low || higherCPT == csc_cpt_invalid || higherCPT == csc_cpt_surrogate_low)
		return CSC_STATUS_GENERAL_FAILURE;

	if (lowerCPT == higherCPT)
	{
		if (lowerCPT == csc_cpt_surrogate_high)
		{
			if (CSC_StringUtilsGetCodePointType(((CONST CSC_WCHAR* CONST)pLowerElement)[(CSC_SIZE_T)1]) != csc_cpt_surrogate_low || CSC_StringUtilsGetCodePointType(((CONST CSC_WCHAR* CONST)pHigherElement)[(CSC_SIZE_T)1]) != csc_cpt_surrogate_low)
				return CSC_STATUS_GENERAL_FAILURE;

			buffer = *(CSC_DWORD* CONST)pLowerElement;
			*(CSC_DWORD* CONST)pLowerElement = *(CSC_DWORD* CONST)pHigherElement;
			*(CSC_DWORD* CONST)pHigherElement = buffer;
		}
		else
		{
			buffer = (CSC_DWORD)*(CSC_WCHAR* CONST)pLowerElement;
			*(CSC_WCHAR* CONST)pLowerElement = *(CSC_WCHAR* CONST)pHigherElement;
			*(CSC_WCHAR* CONST)pHigherElement = (CSC_WCHAR)buffer;
		}
	}
	else
	{
		if (lowerCPT == csc_cpt_surrogate_high)
		{
			if (CSC_StringUtilsGetCodePointType(((CONST CSC_WCHAR * CONST)pLowerElement)[(CSC_SIZE_T)1]) != csc_cpt_surrogate_low)
				return CSC_STATUS_GENERAL_FAILURE;

			pDstCopy = (CSC_PVOID)((CONST CSC_WCHAR* CONST)pLowerElement + (CSC_SIZE_T)1);
			pSrcCopy = (CSC_PVOID)((CONST CSC_WCHAR* CONST)pLowerElement + (CSC_SIZE_T)2);
			copySize = (CSC_SIZE_T)((CONST CSC_WCHAR* CONST)pHigherElement - (CONST CSC_WCHAR* CONST)pSrcCopy);
			buffer = *(CSC_DWORD* CONST)pLowerElement;

			if (copySize)
			{
				status = CSC_MemoryUtilsCopyMemory(pDstCopy, (CSC_PCVOID)pSrcCopy, copySize * sizeof(CSC_WCHAR));

				if (status != CSC_STATUS_SUCCESS)
					return status;
			}

			*(CSC_WCHAR* CONST)pLowerElement = *(CONST CSC_WCHAR* CONST)pHigherElement;
			*(CSC_DWORD* CONST)((CONST CSC_WCHAR* CONST)pDstCopy + copySize) = buffer;
		}
		else
		{
			if (CSC_StringUtilsGetCodePointType(((CONST CSC_WCHAR * CONST)pHigherElement)[(CSC_SIZE_T)1]) != csc_cpt_surrogate_low)
				return CSC_STATUS_GENERAL_FAILURE;

			pDstCopy = (CSC_PVOID)((CONST CSC_WCHAR* CONST)pLowerElement + (CSC_SIZE_T)2);
			pSrcCopy = (CSC_PVOID)((CONST CSC_WCHAR* CONST)pLowerElement + (CSC_SIZE_T)1);
			copySize = (CSC_SIZE_T)((CONST CSC_WCHAR* CONST)pHigherElement - (CONST CSC_WCHAR* CONST)pSrcCopy);
			buffer = *(CSC_DWORD* CONST)pHigherElement;

			if (copySize)
			{
				status = CSC_MemoryUtilsCopyMemory(pDstCopy, (CSC_PCVOID)pSrcCopy, copySize * sizeof(CSC_WCHAR));

				if (status != CSC_STATUS_SUCCESS)
					return status;
			}

			*((CSC_WCHAR* CONST)pDstCopy + copySize) = *(CONST CSC_WCHAR* CONST)pLowerElement;
			*(CSC_DWORD* CONST)pLowerElement = buffer;
		}

		pWString->validCache = (CSC_BOOLEAN)FALSE;
		CSC_DynamicArrayErase(&pWString->indexArray);

		if (pIterator->iterationValid)
		{
			pIterator->pCurrElement = CSC_WStringLazyAccessCodePoint(pWString, pIterator->currentIndex);

			if (!pIterator->pCurrElement)
				CSC_IteratorInvalidateIteration(pIterator);
		}
	}

	return CSC_STATUS_SUCCESS;
}


CSC_STATUS CSCMETHOD CSC_AStringConvertToWString(_In_ CONST CSC_AString* CONST pAString, _When_(return == CSC_STATUS_SUCCESS, _Out_) CSC_WString* CONST pWString)
{
	CSC_STATUS status;
	CSC_SIZE_T iterator;
	CSC_DynamicArray sequenceBuffer;
	CSC_WCHAR* pSequence;
	CONST CSC_CHAR* pCStr;
	CONST CSC_SIZE_T length = CSC_AStringGetLength(pAString);

	if (length == CSC_STRING_INVALID_LENGTH || !length || !pWString)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_WStringZeroMemory(pWString);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	status = CSC_DynamicArrayInitializeWithSize(&sequenceBuffer, sizeof(CSC_WCHAR), length, CSC_AStringGetAllocator(pAString));

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pSequence = CSC_DynamicArrayFront(&sequenceBuffer);
	pCStr = CSC_AStringGetData(pAString);

	if (!pSequence || !pCStr)
	{
		CSC_DynamicArrayDestroy(&sequenceBuffer);
		return CSC_STATUS_GENERAL_FAILURE;
	}

	for (iterator = (CSC_SIZE_T)0; iterator < length; ++iterator)
	{
		pSequence[iterator] = (CSC_WCHAR)pCStr[iterator];
	}

	status = CSC_WStringInitializeSequenceImpl(pWString, pSequence, length, CSC_AStringGetAllocator(pAString), (CSC_BOOLEAN)FALSE);
	CSC_DynamicArrayDestroy(&sequenceBuffer);

	return status;
}

CSC_STATUS CSCMETHOD CSC_WStringConvertToAString(_In_ CONST CSC_WString* CONST pWString, _When_(return == CSC_STATUS_SUCCESS, _Out_) CSC_AString* CONST pAString)
{
	CSC_STATUS status;
	CSC_CodePointType currentCPT;
	CSC_SIZE_T iterator;
	CSC_DynamicArray sequenceBuffer;
	CSC_CHAR* pSequence;
	CONST CSC_WCHAR* pCStr;
	CSC_SIZE_T length;

	CSC_WStringPrepareForIndexing(pWString);

	length = CSC_WStringGetLength(pWString);

	if (length == CSC_STRING_INVALID_LENGTH || !length || !pAString)
		return CSC_STATUS_INVALID_PARAMETER;

	status = CSC_AStringZeroMemory(pAString);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	if (length != CSC_DynamicArrayGetSize(&pWString->dynamicArray) - (CSC_SIZE_T)1)
		return CSC_STATUS_GENERAL_FAILURE;

	status = CSC_DynamicArrayInitializeWithSize(&sequenceBuffer, sizeof(CSC_CHAR), length, CSC_WStringGetAllocator(pWString));

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pSequence = CSC_DynamicArrayFront(&sequenceBuffer);
	pCStr = CSC_WStringGetData(pWString);

	if (!pSequence || !pCStr)
	{
		CSC_DynamicArrayDestroy(&sequenceBuffer);
		return CSC_STATUS_GENERAL_FAILURE;
	}

	for (iterator = (CSC_SIZE_T)0; iterator < length; ++iterator)
	{
		currentCPT = CSC_StringUtilsGetCodePointType(pCStr[iterator]);

		if (currentCPT != csc_cpt_bmp)
		{
			CSC_DynamicArrayDestroy(&sequenceBuffer);
			return CSC_STATUS_GENERAL_FAILURE;
		}

		if ((CSC_WORD)pCStr[iterator] > (CSC_WORD)0xFF)
		{
			CSC_DynamicArrayDestroy(&sequenceBuffer);
			return CSC_STATUS_GENERAL_FAILURE;
		}

		pSequence[iterator] = (CSC_CHAR)pCStr[iterator];
	}

	status = CSC_AStringInitializeSequenceImpl(pAString, pSequence, length, CSC_WStringGetAllocator(pWString), (CSC_BOOLEAN)FALSE);
	CSC_DynamicArrayDestroy(&sequenceBuffer);

	return status;
}
