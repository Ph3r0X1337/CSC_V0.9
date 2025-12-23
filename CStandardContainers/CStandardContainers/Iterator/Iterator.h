#ifndef CSC_ITERATOR
#define CSC_ITERATOR

#include "../Configuration/Configuration.h"
#include "../Utils/StringUtils.h"

#define CSC_ITERATOR_INVALID_LENGTH CSC_STRING_INVALID_LENGTH
#define CSC_ITERATOR_INVALID_INDEX CSC_ITERATOR_INVALID_LENGTH

typedef enum _CSC_IteratorVirtualFunction
{
	csc_vfn_it_register = 0,
	csc_vfn_it_unregister,
	csc_vfn_it_first,
	csc_vfn_it_next,
	csc_vfn_it_last,
	csc_vfn_it_previous,
	csc_vfn_it_move_to_index,
	csc_vfn_it_current_index,
	csc_vfn_it_current_ptr,
	csc_vfn_it_at,
	csc_vfn_it_count,
	csc_vfn_it_swap_index,
	csc_vfn_it_swap_ptr,
	csc_vfn_it_num_of_entries
} CSC_IteratorVirtualFunction;

typedef struct _CSC_Iterator
{
	CSC_PVOID pContainer;
	CSC_PVOID pCurrElement;
	CSC_SIZE_T elementSize;
	CSC_SIZE_T elementCount;
	CSC_SIZE_T currentIndex;
	CSC_BOOLEAN iterationValid;
	CSC_BOOLEAN swapByIndex;
} CSC_Iterator;

typedef CSC_STATUS(CSCMETHOD* CSC_P_VFN_ITERATOR_REGISTER)(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
typedef CSC_STATUS(CSCMETHOD* CSC_P_VFN_ITERATOR_UNREGISTER)(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
typedef CSC_STATUS(CSCMETHOD* CSC_P_VFN_ITERATOR_FIRST_ELEMENT)(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
typedef CSC_STATUS(CSCMETHOD* CSC_P_VFN_ITERATOR_NEXT_ELEMENT)(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
typedef CSC_STATUS(CSCMETHOD* CSC_P_VFN_ITERATOR_LAST_ELEMENT)(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
typedef CSC_STATUS(CSCMETHOD* CSC_P_VFN_ITERATOR_PREVIOUS_ELEMENT)(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
typedef CSC_STATUS(CSCMETHOD* CSC_P_VFN_ITERATOR_MOVE_TO_ELEMENT_INDEX)(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index);
typedef CSC_SIZE_T(CSCMETHOD* CSC_P_VFN_ITERATOR_GET_CURRENT_ELEMENT_INDEX)(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
typedef CSC_PVOID(CSCMETHOD* CSC_P_VFN_ITERATOR_GET_CURRENT_ELEMENT_PTR)(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
typedef CSC_PVOID(CSCMETHOD* CSC_P_VFN_ITERATOR_GET_ELEMENT_AT)(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T index);
typedef CSC_SIZE_T(CSCMETHOD* CSC_P_VFN_ITERATOR_GET_ELEMENT_COUNT)(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator);
typedef CSC_STATUS(CSCMETHOD* CSC_P_VFN_ITERATOR_SWAP_ELEMENTS_INDEX)(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T secondIndex);
typedef CSC_STATUS(CSCMETHOD* CSC_P_VFN_ITERATOR_SWAP_ELEMENTS_PTR)(_Inout_ CONST CSC_PVOID pContainer, _Inout_ CSC_Iterator* CONST pIterator, _Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond);

CSC_STATUS CSCMETHOD CSC_IteratorRegister(_Inout_ CSC_Iterator* CONST pThis, _Inout_ CONST CSC_PVOID pContainer);
CSC_STATUS CSCMETHOD CSC_IteratorUnregister(_Inout_ CSC_Iterator* CONST pThis);
CSC_STATUS CSCMETHOD CSC_IteratorFirstElement(_Inout_ CSC_Iterator* CONST pThis);
CSC_STATUS CSCMETHOD CSC_IteratorNextElement(_Inout_ CSC_Iterator* CONST pThis);
CSC_STATUS CSCMETHOD CSC_IteratorLastElement(_Inout_ CSC_Iterator* CONST pThis);
CSC_STATUS CSCMETHOD CSC_IteratorPreviousElement(_Inout_ CSC_Iterator* CONST pThis);
CSC_STATUS CSCMETHOD CSC_IteratorMoveToIndex(_Inout_ CSC_Iterator* CONST pThis, _In_ CONST CSC_SIZE_T index);
CSC_SIZE_T CSCMETHOD CSC_IteratorGetCurrentElementIndex(_Inout_ CSC_Iterator* CONST pThis);
CSC_PVOID CSCMETHOD CSC_IteratorGetCurrentElement(_Inout_ CSC_Iterator* CONST pThis);
CSC_PVOID CSCMETHOD CSC_IteratorGetElementAt(_Inout_ CSC_Iterator* CONST pThis, _In_ CONST CSC_SIZE_T index);
CSC_SIZE_T CSCMETHOD CSC_IteratorGetElementCount(_Inout_ CSC_Iterator* CONST pThis);
CSC_STATUS CSCMETHOD CSC_IteratorSwapElementsByIndex(_Inout_ CSC_Iterator* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T secondIndex);
CSC_STATUS CSCMETHOD CSC_IteratorSwapElementsByPtr(_Inout_ CSC_Iterator* CONST pThis, _Inout_ CONST CSC_PVOID pFirst, _Inout_ CONST CSC_PVOID pSecond);

CSC_STATUS CSCMETHOD CSC_IteratorZeroMemory(_Out_ CSC_Iterator* CONST pThis);
CSC_STATUS CSCMETHOD CSC_IteratorInitialize(_Out_ CSC_Iterator* CONST pThis);
CSC_STATUS CSCMETHOD CSC_IteratorDestroy(_Inout_ CSC_Iterator* CONST pThis);
CSC_STATUS CSCMETHOD CSC_IteratorInvalidateIteration(_Inout_ CSC_Iterator* CONST pThis);
CSC_STATUS CSCMETHOD CSC_IteratorIsValid(_In_ CONST CSC_Iterator* CONST pThis);
CSC_STATUS CSCMETHOD CSC_IteratorIsIterationValid(_In_ CONST CSC_Iterator* CONST pThis);
CSC_STATUS CSCMETHOD CSC_IteratorIsSwapByIndex(_In_ CONST CSC_Iterator* CONST pThis);

#endif CSC_ITERATOR