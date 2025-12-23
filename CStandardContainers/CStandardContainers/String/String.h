#ifndef CSC_STRING
#define CSC_STRING

#include "../Configuration/Configuration.h"
#include "../DynamicArray/DynamicArray.h"
#include "../Utils/StringUtils.h"
#include "../Iterator/Iterator.h"

typedef struct _CSC_AString
{
	CSC_PCVOID pVTableIterator;
	CSC_DynamicArray dynamicArray;
	CSC_Iterator* pIterator;
	CSC_SIZE_T cachedLength;
} CSC_AString;

typedef struct _CSC_WString
{
	CSC_PCVOID pVTableIterator;
	CSC_DynamicArray dynamicArray;
	CSC_DynamicArray indexArray;
	CSC_Iterator* pIterator;
	CSC_BOOLEAN validCache;
} CSC_WString;


CSC_STATUS CSCMETHOD CSC_AStringInitialize(_Out_ CSC_AString* CONST pThis, _In_ CONST CSC_PVOID pAllocator);
CSC_STATUS CSCMETHOD CSC_AStringInitializeSequence(_Out_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PVOID pAllocator);
CSC_STATUS CSCMETHOD CSC_AStringInitializeCString(_Out_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr, _In_ CONST CSC_PVOID pAllocator);
CSC_STATUS CSCMETHOD CSC_AStringInitializeCopy(_Out_ CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther, _In_ CONST CSC_PVOID pAllocator);

CSC_STATUS CSCMETHOD CSC_AStringReserve(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements);
CSC_STATUS CSCMETHOD CSC_AStringShrinkToFit(_Inout_ CSC_AString* CONST pThis);
CSC_STATUS CSCMETHOD CSC_AStringRepair(_Inout_ CSC_AString* CONST pThis);

CSC_STATUS CSCMETHOD CSC_AStringDestroy(_Inout_ CSC_AString* CONST pThis);
CSC_STATUS CSCMETHOD CSC_AStringErase(_Inout_ CSC_AString* CONST pThis);
CSC_STATUS CSCMETHOD CSC_AStringZeroMemory(_Out_ CSC_AString* CONST pThis);

CSC_STATUS CSCMETHOD CSC_AStringPrepareForIndexing(_Inout_ CSC_AString* CONST pThis);
CSC_CHAR* CSCMETHOD CSC_AStringAccessElement(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T index);
CSC_CHAR* CSCMETHOD CSC_AStringLazyAccessElement(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T index);

CSC_STATUS CSCMETHOD CSC_AStringAssignSequence(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements);
//CSC_STATUS CSCMETHOD CSC_AStringLazyAssignSequence(CSC_AString* CONST pThis, CONST CSC_CHAR* CONST pArray, CONST CSC_SIZE_T numOfElements);
CSC_STATUS CSCMETHOD CSC_AStringAssignCString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_AStringCopy(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pSrc);
CSC_STATUS CSCMETHOD CSC_AStringMove(_Inout_ CSC_AString* CONST pThis, _Inout_ CSC_AString* CONST pSrc);

CSC_STATUS CSCMETHOD CSC_AStringFill(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR value);
CSC_STATUS CSCMETHOD CSC_AStringFillRange(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR value);
//CSC_STATUS CSCMETHOD CSC_AStringLazyFillRange(CSC_AString* CONST pThis, CONST CSC_SIZE_T firstIndex, CONST CSC_SIZE_T lastIndex, CONST CSC_CHAR value);

CSC_STATUS CSCMETHOD CSC_AStringReplaceSequence(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T replaceIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray);
//CSC_STATUS CSCMETHOD CSC_AStringLazyReplaceSequence(CSC_AString* CONST pThis, CONST CSC_SIZE_T replaceIndex, CONST CSC_SIZE_T numOfElements, CONST CSC_CHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_AStringReplaceCString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T replaceIndex, _In_ CONST CSC_CHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_AStringReplaceString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T replaceIndex, _In_ CONST CSC_AString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_AStringInsertSequence(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray);
//CSC_STATUS CSCMETHOD CSC_AStringLazyInsertSequence(CSC_AString* CONST pThis, CONST CSC_SIZE_T insertIndex, CONST CSC_SIZE_T numOfElements, CONST CSC_CHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_AStringInsertCString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_ CONST CSC_CHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_AStringInsertString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T insertIndex, _In_ CONST CSC_AString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_AStringAppendSequence(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray);
//CSC_STATUS CSCMETHOD CSC_AStringLazyAppendSequence(CSC_AString* CONST pThis, CONST CSC_SIZE_T numOfElements, CONST CSC_CHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_AStringAppendCString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_AStringAppendString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_AStringPrependSequence(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray);
//CSC_STATUS CSCMETHOD CSC_AStringLazyPrependSequence(CSC_AString* CONST pThis, CONST CSC_SIZE_T numOfElements, CONST CSC_CHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_AStringPrependCString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_AStringPrependString(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_AStringReduceToSubstring(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T lastIndex);
CSC_STATUS CSCMETHOD CSC_AStringReduceToSubstringRange(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T numOfElements);
CSC_STATUS CSCMETHOD CSC_AStringRemove(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T firstIndex, _In_ CONST CSC_SIZE_T lastIndex);
CSC_STATUS CSCMETHOD CSC_AStringRemoveRange(_Inout_ CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T removeIndex, _In_ CONST CSC_SIZE_T numOfElements);

CSC_STATUS CSCMETHOD CSC_AStringReverse(_Inout_ CSC_AString* CONST pThis);

CSC_SIZE_T CSCMETHOD CSC_AStringFindSequence(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray);
CSC_SIZE_T CSCMETHOD CSC_AStringFindCString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr);
CSC_SIZE_T CSCMETHOD CSC_AStringFindString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther);

CSC_SIZE_T CSCMETHOD CSC_AStringRFindSequence(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray);
CSC_SIZE_T CSCMETHOD CSC_AStringRFindCString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr);
CSC_SIZE_T CSCMETHOD CSC_AStringRFindString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_AStringContainsSequence(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_AStringContainsCString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_AStringContainsString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_AStringStartsWithSequence(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_AStringStartsWithCString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_AStringStartsWithString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_AStringEndsWithSequence(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_AStringEndsWithCString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_CHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_AStringEndsWithString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_AString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_AStringCompareWithSequence(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_CHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_AStringCompareWithCString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_CHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_AStringCompareWithString(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_AString* CONST pOther);

CSC_CHAR* CSCMETHOD CSC_AStringGetData(_In_ CONST CSC_AString* CONST pThis);

CSC_STATUS CSCMETHOD CSC_AStringIsEmpty(_In_ CONST CSC_AString* CONST pThis);
CSC_STATUS CSCMETHOD CSC_AStringIsValid(_In_ CONST CSC_AString* CONST pThis, _In_ CONST CSC_BOOLEAN checkLength);

CSC_SIZE_T CSCMETHOD CSC_AStringGetLength(_In_ CONST CSC_AString* CONST pThis);
CSC_SIZE_T CSCMETHOD CSC_AStringGetCapacity(_In_ CONST CSC_AString* CONST pThis);
CSC_SIZE_T CSCMETHOD CSC_AStringGetMaxLength(_In_ CONST CSC_AString* CONST pThis);
CSC_PVOID CSCMETHOD CSC_AStringGetAllocator(_In_ CONST CSC_AString* CONST pThis);


CSC_STATUS CSCMETHOD CSC_WStringInitialize(_Out_ CSC_WString* CONST pThis, _In_ CONST CSC_PVOID pAllocator);
CSC_STATUS CSCMETHOD CSC_WStringInitializeSequence(_Out_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_PVOID pAllocator);
CSC_STATUS CSCMETHOD CSC_WStringInitializeCString(_Out_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr, _In_ CONST CSC_PVOID pAllocator);
CSC_STATUS CSCMETHOD CSC_WStringInitializeCopy(_Out_ CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther, _In_ CONST CSC_PVOID pAllocator);

CSC_STATUS CSCMETHOD CSC_WStringReserveChars(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements);
CSC_STATUS CSCMETHOD CSC_WStringShrinkToFit(_Inout_ CSC_WString* CONST pThis);
CSC_STATUS CSCMETHOD CSC_WStringRepair(_Inout_ CSC_WString* CONST pThis);

CSC_STATUS CSCMETHOD CSC_WStringDestroy(_Inout_ CSC_WString* CONST pThis);
CSC_STATUS CSCMETHOD CSC_WStringErase(_Inout_ CSC_WString* CONST pThis);
CSC_STATUS CSCMETHOD CSC_WStringZeroMemory(_Out_ CSC_WString* CONST pThis);

CSC_STATUS CSCMETHOD CSC_WStringPrepareForIndexing(_Inout_ CSC_WString* CONST pThis);
CSC_WCHAR* CSCMETHOD CSC_WStringAccessCodePoint(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T codePointIndex);
CSC_WCHAR* CSCMETHOD CSC_WStringLazyAccessCodePoint(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T codePointIndex);

CSC_STATUS CSCMETHOD CSC_WStringAssignSequence(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pArray, _In_ CONST CSC_SIZE_T numOfElements);
//CSC_STATUS CSCMETHOD CSC_WStringLazyAssignSequence(CSC_WString* CONST pThis, CONST WCSC_CHAR* CONST pArray, CONST CSC_SIZE_T numOfElements);
CSC_STATUS CSCMETHOD CSC_WStringAssignCString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_WStringCopy(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pSrc);
CSC_STATUS CSCMETHOD CSC_WStringMove(_Inout_ CSC_WString* CONST pThis, _Inout_ CSC_WString* CONST pSrc);

CSC_STATUS CSCMETHOD CSC_WStringFill(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfCodePoints, _In_ CONST CSC_DWORD codePointValue);
CSC_STATUS CSCMETHOD CSC_WStringFillRange(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T firstCodePointIndex, _In_ CONST CSC_SIZE_T numOfCodePoints, _In_ CONST CSC_DWORD codePointValue);
//CSC_STATUS CSCMETHOD CSC_WStringLazyFillRange(CSC_WString* CONST pThis, CONST CSC_SIZE_T firstCodePointIndex, CONST CSC_SIZE_T lastCodePointIndex, CONST CSC_DWORD codePointValue);

CSC_STATUS CSCMETHOD CSC_WStringReplaceSequence(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T replaceCodePointIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray);
//CSC_STATUS CSCMETHOD CSC_WStringLazyReplaceSequence(CSC_WString* CONST pThis, CONST CSC_SIZE_T replaceCodePointIndex, CONST CSC_SIZE_T numOfElements, CONST WCSC_CHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_WStringReplaceCString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T replaceCodePointIndex, _In_ CONST CSC_WCHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_WStringReplaceString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T replaceCodePointIndex, _In_ CONST CSC_WString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_WStringInsertSequence(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T insertCodePointIndex, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray);
//CSC_STATUS CSCMETHOD CSC_WStringLazyInsertSequence(CSC_WString* CONST pThis, CONST CSC_SIZE_T insertCodePointIndex, CONST CSC_SIZE_T numOfElements, CONST WCSC_CHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_WStringInsertCString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T insertCodePointIndex, _In_ CONST CSC_WCHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_WStringInsertString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T insertCodePointIndex, _In_ CONST CSC_WString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_WStringAppendSequence(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray);
//CSC_STATUS CSCMETHOD CSC_WStringLazyAppendSequence(CSC_WString* CONST pThis, CONST CSC_SIZE_T numOfElements, CONST WCSC_CHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_WStringAppendCString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_WStringAppendString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_WStringPrependSequence(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray);
//CSC_STATUS CSCMETHOD CSC_WStringLazyPrependSequence(CSC_WString* CONST pThis, CONST CSC_SIZE_T numOfElements, CONST WCSC_CHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_WStringPrependCString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_WStringPrependString(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_WStringReduceToSubstring(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T firstCodePointIndex, _In_ CONST CSC_SIZE_T lastCodePointIndex);
CSC_STATUS CSCMETHOD CSC_WStringReduceToSubstringRange(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T firstCodePointIndex, _In_ CONST CSC_SIZE_T numOfCodePoints);
CSC_STATUS CSCMETHOD CSC_WStringRemove(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T firstCodePointIndex, _In_ CONST CSC_SIZE_T lastCodePointIndex);
CSC_STATUS CSCMETHOD CSC_WStringRemoveRange(_Inout_ CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T removeCodePointIndex, _In_ CONST CSC_SIZE_T numOfCodePoints);

CSC_STATUS CSCMETHOD CSC_WStringReverse(_Inout_ CSC_WString* CONST pThis);

CSC_SIZE_T CSCMETHOD CSC_WStringFindSequence(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray);
CSC_SIZE_T CSCMETHOD CSC_WStringFindCString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr);
CSC_SIZE_T CSCMETHOD CSC_WStringFindString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther);

CSC_SIZE_T CSCMETHOD CSC_WStringRFindSequence(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray);
CSC_SIZE_T CSCMETHOD CSC_WStringRFindCString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr);
CSC_SIZE_T CSCMETHOD CSC_WStringRFindString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_WStringContainsSequence(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_WStringContainsCString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_WStringContainsString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_WStringStartsWithSequence(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_WStringStartsWithCString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_WStringStartsWithString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_WStringEndsWithSequence(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_WStringEndsWithCString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WCHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_WStringEndsWithString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_WString* CONST pOther);

CSC_STATUS CSCMETHOD CSC_WStringCompareWithSequence(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_SIZE_T numOfElements, _In_ CONST CSC_WCHAR* CONST pArray);
CSC_STATUS CSCMETHOD CSC_WStringCompareWithCString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_WCHAR* CONST pCStr);
CSC_STATUS CSCMETHOD CSC_WStringCompareWithString(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_BOOLEAN strict, _In_ CONST CSC_WString* CONST pOther);

CSC_WCHAR* CSCMETHOD CSC_WStringGetData(_In_ CONST CSC_WString* CONST pThis);

CSC_STATUS CSCMETHOD CSC_WStringIsEmpty(_In_ CONST CSC_WString* CONST pThis);
CSC_STATUS CSCMETHOD CSC_WStringIsValid(_In_ CONST CSC_WString* CONST pThis, _In_ CONST CSC_BOOLEAN checkLength);

CSC_SIZE_T CSCMETHOD CSC_WStringGetLength(_In_ CONST CSC_WString* CONST pThis);
CSC_SIZE_T CSCMETHOD CSC_WStringGetCapacityChars(_In_ CONST CSC_WString* CONST pThis);
CSC_SIZE_T CSCMETHOD CSC_WStringGetMaxLength(_In_ CONST CSC_WString* CONST pThis);
CSC_PVOID CSCMETHOD CSC_WStringGetAllocator(_In_ CONST CSC_WString* CONST pThis);

CSC_STATUS CSCMETHOD CSC_AStringConvertToWString(_In_ CONST CSC_AString* CONST pAString, _When_(return == STATUS_SUCCESS, _Out_) CSC_WString* CONST pWString);
CSC_STATUS CSCMETHOD CSC_WStringConvertToAString(_In_ CONST CSC_WString* CONST pWString, _When_(return == STATUS_SUCCESS, _Out_) CSC_AString* CONST pAString);

#endif CSC_STRING

/*
For my library I am mostly forwarding *CString and *String logic to the *Sequence equivalents, by before calculating the string length O(n). Now the *Sequence methods again also do a *ValidSequence check with complexity O(n). I don't want to add another parameter to the *Sequence methods, but I could introduce some internal API like *SequenceImpl, that has a parameter for an optional check... This way the *Sequence methods could just forward to this function and require a validity check, while *CString and *String methods could just invoke *SequenceImpl directly...

Further realization:
Implement length caching on CSC_AString, invalidation is easy by setting it to g_invalidStringLength...
A more complex principle can be applied to CSC_WString, which should hold an additional CSC_DynamicArray containing the code point inidces -> An empty array indicates either an empty string or an invalid index map, auxillary CSC_BOOLEAN can be used to check the validaty...
Both implementations can feature a function called "PrepareForIndexing", which will setup the container for indexing. Any successful mutational operations will invalidate this constant indexing again.
Also keep in mind that the code point checking currently accesses "out of bounds" for surrogate pairs. Changing this will affect string length and other algorithms for WString, because consecutive checks for surrogate pairs will need to be done manually.
*/
