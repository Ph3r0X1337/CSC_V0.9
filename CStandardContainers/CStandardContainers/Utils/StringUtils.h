#ifndef CSC_STRING_UTILS
#define CSC_STRING_UTILS

#include "../Configuration/Configuration.h"

#define CSC_STRING_INVALID_LENGTH (CSC_SIZE_T)-1
#define CSC_STRING_INVALID_INDEX CSC_STRING_INVALID_LENGTH

#define CSC_MAXIMUM_STRING_LENGTH_ANSI_STRING (CSC_SIZE_T)0x1FFFFFFF
#define CSC_MAXIMUM_STRING_LENGTH_WIDE_STRING (CSC_SIZE_T)0x07FFFFFF
#define CSC_MAXIMUM_STRING_CHARS_WIDE_STRING (CSC_SIZE_T)0x0FFFFFFE

typedef enum _CSC_CodePointType
{
	csc_cpt_invalid = 0,
	csc_cpt_bmp,
	csc_cpt_surrogate_high,
	csc_cpt_surrogate_low
} CSC_CodePointType;

CSC_SIZE_T CSCAPI CSC_StringUtilsStrLenAnsiString(_In_ CONST CSC_CHAR* CONST pCStr, _In_ CONST CSC_SIZE_T maxChars, _In_ CONST CSC_BOOLEAN requireNullTerminator);
CSC_SIZE_T CSCAPI CSC_StringUtilsStrLenWideString(_In_ CONST CSC_WCHAR* CONST pCStr, _In_ CONST CSC_SIZE_T maxCodePoints, _In_ CONST CSC_BOOLEAN requireNullTerminator);
CSC_SIZE_T CSCAPI CSC_StringUtilsStrWideCharLenWideString(_In_ CONST CSC_WCHAR* CONST pCStr, _In_ CONST CSC_SIZE_T maxChars, _In_ CONST CSC_BOOLEAN requireNullTerminator, _Out_opt_ CSC_SIZE_T* CONST pCodePointLength);

CSC_STATUS CSCAPI CSC_StringUtilsCompareAnsiString(_In_ CONST CSC_CHAR* CONST pFirst, _In_ CONST CSC_CHAR* CONST pSecond, _In_ CONST CSC_SIZE_T maxChars);
CSC_STATUS CSCAPI CSC_StringUtilsCompareWideString(_In_ CONST CSC_WCHAR* CONST pFirst, _In_ CONST CSC_WCHAR* CONST pSecond, _In_ CONST CSC_SIZE_T maxCodePoints);

CSC_BOOLEAN CSCAPI CSC_StringUtilsIsBMPCharacter(_In_ CONST CSC_WCHAR wChar);
CSC_BOOLEAN CSCAPI CSC_StringUtilsIsSurrogatePair(_In_ CONST CSC_WCHAR leading, _In_ CONST CSC_WCHAR trailing);
CSC_CodePointType CSCAPI CSC_StringUtilsGetCodePointType(_In_ CONST CSC_WCHAR wChar);

#endif CSC_STRING_UTILS