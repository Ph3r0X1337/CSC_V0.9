#include "Iterator.h"
#include "../Utils/MemoryUtils.h"

CSC_STATUS CSCMETHOD CSC_IteratorRegister(_Inout_ CSC_Iterator* CONST pThis, _Inout_ CONST CSC_PVOID pContainer)
{
	CSC_P_VFN_ITERATOR_REGISTER pVfnRegister;

	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	if (CSC_IteratorIsValid(pThis) == CSC_STATUS_SUCCESS)
		return CSC_STATUS_INVALID_PARAMETER;

	if (!pContainer || !(*(CSC_PCVOID**)pContainer))
		return CSC_STATUS_INVALID_PARAMETER;

	pVfnRegister = (CSC_P_VFN_ITERATOR_REGISTER)(*(CSC_PCVOID**)pContainer)[csc_vfn_it_register];

	if (!pVfnRegister)
		return CSC_STATUS_INVALID_PARAMETER;

	return pVfnRegister(pContainer, pThis);
}

CSC_STATUS CSCMETHOD CSC_IteratorUnregister(_Inout_ CSC_Iterator* CONST pThis)
{
	CSC_P_VFN_ITERATOR_UNREGISTER pVfnUnregister;
	CONST CSC_STATUS status = CSC_IteratorIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pVfnUnregister = (CSC_P_VFN_ITERATOR_UNREGISTER)(*(CSC_PCVOID**)pThis->pContainer)[csc_vfn_it_unregister];

	if (!pVfnUnregister)
		return CSC_STATUS_INVALID_PARAMETER;

	return pVfnUnregister(pThis->pContainer, pThis);
}

CSC_STATUS CSCMETHOD CSC_IteratorFirstElement(_Inout_ CSC_Iterator* CONST pThis)
{
	CSC_P_VFN_ITERATOR_FIRST_ELEMENT pVfnFirstElement;
	CONST CSC_STATUS status = CSC_IteratorIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pVfnFirstElement = (CSC_P_VFN_ITERATOR_FIRST_ELEMENT)(*(CSC_PCVOID**)pThis->pContainer)[csc_vfn_it_first];

	if (!pVfnFirstElement)
		return CSC_STATUS_INVALID_PARAMETER;

	return pVfnFirstElement(pThis->pContainer, pThis);
}

CSC_STATUS CSCMETHOD CSC_IteratorNextElement(_Inout_ CSC_Iterator* CONST pThis)
{
	CSC_P_VFN_ITERATOR_NEXT_ELEMENT pVfnNextElement;
	CONST CSC_STATUS status = CSC_IteratorIsIterationValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pVfnNextElement = (CSC_P_VFN_ITERATOR_NEXT_ELEMENT)(*(CSC_PCVOID**)pThis->pContainer)[csc_vfn_it_next];

	if (!pVfnNextElement)
		return CSC_STATUS_INVALID_PARAMETER;

	return pVfnNextElement(pThis->pContainer, pThis);
}

CSC_STATUS CSCMETHOD CSC_IteratorLastElement(_Inout_ CSC_Iterator* CONST pThis)
{
	CSC_P_VFN_ITERATOR_LAST_ELEMENT pVfnLastElement;
	CONST CSC_STATUS status = CSC_IteratorIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pVfnLastElement = (CSC_P_VFN_ITERATOR_LAST_ELEMENT)(*(CSC_PCVOID**)pThis->pContainer)[csc_vfn_it_last];

	if (!pVfnLastElement)
		return CSC_STATUS_INVALID_PARAMETER;

	return pVfnLastElement(pThis->pContainer, pThis);
}

CSC_STATUS CSCMETHOD CSC_IteratorPreviousElement(_Inout_ CSC_Iterator* CONST pThis)
{
	CSC_P_VFN_ITERATOR_PREVIOUS_ELEMENT pVfnPreviousElement;
	CONST CSC_STATUS status = CSC_IteratorIsIterationValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pVfnPreviousElement = (CSC_P_VFN_ITERATOR_PREVIOUS_ELEMENT)(*(CSC_PCVOID**)pThis->pContainer)[csc_vfn_it_previous];

	if (!pVfnPreviousElement)
		return CSC_STATUS_INVALID_PARAMETER;

	return pVfnPreviousElement(pThis->pContainer, pThis);
}

CSC_STATUS CSCMETHOD CSC_IteratorMoveToIndex(_Inout_ CSC_Iterator* CONST pThis, _In_ CONST CSC_SIZE_T index)
{
	CSC_P_VFN_ITERATOR_MOVE_TO_ELEMENT_INDEX pVfnMoveToIndex;
	CONST CSC_STATUS status = CSC_IteratorIsIterationValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pVfnMoveToIndex = (CSC_P_VFN_ITERATOR_MOVE_TO_ELEMENT_INDEX)(*(CSC_PCVOID**)pThis->pContainer)[csc_vfn_it_move_to_index];

	if (!pVfnMoveToIndex)
		return CSC_STATUS_INVALID_PARAMETER;

	return pVfnMoveToIndex(pThis->pContainer, pThis, index);
}

CSC_SIZE_T CSCMETHOD CSC_IteratorGetCurrentElementIndex(_Inout_ CSC_Iterator* CONST pThis)
{
	CSC_P_VFN_ITERATOR_GET_CURRENT_ELEMENT_INDEX pVfnGetCurrentElementIndex;
	CONST CSC_STATUS status = CSC_IteratorIsIterationValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return CSC_ITERATOR_INVALID_INDEX;

	pVfnGetCurrentElementIndex = (CSC_P_VFN_ITERATOR_GET_CURRENT_ELEMENT_INDEX)(*(CSC_PCVOID**)pThis->pContainer)[csc_vfn_it_current_index];

	if (!pVfnGetCurrentElementIndex)
		return CSC_ITERATOR_INVALID_INDEX;

	return pVfnGetCurrentElementIndex(pThis->pContainer, pThis);
}

CSC_PVOID CSCMETHOD CSC_IteratorGetCurrentElement(_Inout_ CSC_Iterator* CONST pThis)
{
	CSC_P_VFN_ITERATOR_GET_CURRENT_ELEMENT_PTR pVfnGetCurrentElementPtr;
	CONST CSC_STATUS status = CSC_IteratorIsIterationValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return NULL;

	pVfnGetCurrentElementPtr = (CSC_P_VFN_ITERATOR_GET_CURRENT_ELEMENT_PTR)(*(CSC_PCVOID**)pThis->pContainer)[csc_vfn_it_current_ptr];

	if (!pVfnGetCurrentElementPtr)
		return NULL;

	return pVfnGetCurrentElementPtr(pThis->pContainer, pThis);
}

CSC_PVOID CSCMETHOD CSC_IteratorGetElementAt(_Inout_ CSC_Iterator* CONST pThis, _In_ CONST CSC_SIZE_T index)
{
	CSC_P_VFN_ITERATOR_GET_ELEMENT_AT pVfnGetElementAt;
	CONST CSC_STATUS status = CSC_IteratorIsIterationValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return NULL;

	pVfnGetElementAt = (CSC_P_VFN_ITERATOR_GET_ELEMENT_AT)(*(CSC_PCVOID**)pThis->pContainer)[csc_vfn_it_at];

	if (!pVfnGetElementAt)
		return NULL;

	return pVfnGetElementAt(pThis->pContainer, pThis, index);
}

CSC_SIZE_T CSCMETHOD CSC_IteratorGetElementCount(_Inout_ CSC_Iterator* CONST pThis)
{
	CSC_P_VFN_ITERATOR_GET_ELEMENT_COUNT pVfnGetElementCount;
	CONST CSC_STATUS status = CSC_IteratorIsIterationValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return CSC_ITERATOR_INVALID_LENGTH;

	pVfnGetElementCount = (CSC_P_VFN_ITERATOR_GET_ELEMENT_COUNT)(*(CSC_PCVOID**)pThis->pContainer)[csc_vfn_it_count];

	if (!pVfnGetElementCount)
		return CSC_ITERATOR_INVALID_LENGTH;

	return pVfnGetElementCount(pThis->pContainer, pThis);
}

CSC_STATUS CSCMETHOD CSC_IteratorSwapElementsByIndex(_Inout_ CSC_Iterator* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T secondIndex)
{
	CSC_P_VFN_ITERATOR_SWAP_ELEMENTS_INDEX pVfnSwapByIndex;
	CONST CSC_STATUS status = CSC_IteratorIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pVfnSwapByIndex = (CSC_P_VFN_ITERATOR_SWAP_ELEMENTS_INDEX)(*(CSC_PCVOID**)pThis->pContainer)[csc_vfn_it_swap_index];

	if (!pVfnSwapByIndex)
		return CSC_STATUS_INVALID_PARAMETER;

	return pVfnSwapByIndex(pThis->pContainer, pThis, firstIndex, secondIndex);
}

CSC_STATUS CSCMETHOD CSC_IteratorSwapElementsByPtr(_Inout_ CSC_Iterator* CONST pThis, _Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond)
{
	CSC_P_VFN_ITERATOR_SWAP_ELEMENTS_PTR pVfnSwapByPtr;
	CONST CSC_STATUS status = CSC_IteratorIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pVfnSwapByPtr = (CSC_P_VFN_ITERATOR_SWAP_ELEMENTS_PTR)(*(CSC_PCVOID**)pThis->pContainer)[csc_vfn_it_swap_ptr];

	if (!pVfnSwapByPtr)
		return CSC_STATUS_INVALID_PARAMETER;

	return pVfnSwapByPtr(pThis->pContainer, pThis, pFirst, pSecond);
}


CSC_STATUS CSCMETHOD CSC_IteratorZeroMemory(_Out_ CSC_Iterator* CONST pThis)
{
	if (!pThis)
		return CSC_STATUS_INVALID_PARAMETER;

	pThis->pContainer = NULL;
	pThis->pCurrElement = NULL;
	pThis->currentIndex = (CSC_SIZE_T)0;
	pThis->elementSize = (CSC_SIZE_T)0;
	pThis->elementCount = (CSC_SIZE_T)0;
	pThis->iterationValid = (CSC_BOOLEAN)FALSE;
	pThis->swapByIndex = (CSC_BOOLEAN)FALSE;

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_IteratorInitialize(_Out_ CSC_Iterator* CONST pThis)
{
	CONST CSC_STATUS status = CSC_IteratorZeroMemory(pThis);

	if (status == CSC_STATUS_SUCCESS)
	{
		pThis->currentIndex = CSC_ITERATOR_INVALID_INDEX;
		pThis->elementCount = CSC_ITERATOR_INVALID_LENGTH;
	}

	return status;
}

CSC_STATUS CSCMETHOD CSC_IteratorDestroy(_Inout_ CSC_Iterator* CONST pThis)
{
	CSC_STATUS status;

	if (CSC_IteratorIsValid(pThis) == CSC_STATUS_SUCCESS)
	{
		status = CSC_IteratorUnregister(pThis);

		if (status != CSC_STATUS_SUCCESS)
			return status;
	}

	return CSC_IteratorZeroMemory(pThis);
}

CSC_STATUS CSCMETHOD CSC_IteratorInvalidateIteration(_Inout_ CSC_Iterator* CONST pThis)
{
	CONST CSC_STATUS status = CSC_IteratorIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	pThis->iterationValid = (CSC_BOOLEAN)FALSE;
	pThis->currentIndex = CSC_ITERATOR_INVALID_INDEX;
	pThis->pCurrElement = NULL;

	return CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_IteratorIsValid(_In_ CONST CSC_Iterator* CONST pThis)
{
	return (!pThis || !pThis->elementSize || !pThis->pContainer || !*(CSC_PCVOID*)pThis->pContainer) ? CSC_STATUS_INVALID_PARAMETER : CSC_STATUS_SUCCESS;
}

CSC_STATUS CSCMETHOD CSC_IteratorIsIterationValid(_In_ CONST CSC_Iterator* CONST pThis)
{
	CONST CSC_STATUS status = CSC_IteratorIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	return ((pThis->iterationValid) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}

CSC_STATUS CSCMETHOD CSC_IteratorIsSwapByIndex(_In_ CONST CSC_Iterator* CONST pThis)
{
	CONST CSC_STATUS status = CSC_IteratorIsValid(pThis);

	if (status != CSC_STATUS_SUCCESS)
		return status;

	return ((pThis->swapByIndex) ? CSC_STATUS_SUCCESS : CSC_STATUS_GENERAL_FAILURE);
}
