#include "WinUtils.h"

CSC_STATUS CSCAPI CSC_WinUtilsValidHandle(_In_ CONST HANDLE handle)
{
    return (handle && handle != INVALID_HANDLE_VALUE) ? CSC_STATUS_SUCCESS : STATUS_INVALID_HANDLE;
}