#pragma once
#include <Windows.h>

HWND Create(WNDPROC winproc,
    PCWSTR lpWindowName,
    PCWSTR className,
    void* pPointer,
    DWORD dwStyle,
    DWORD dwExStyle = 0,
    HWND hWndParent = 0,
    UINT  MENUIDD = 0,
    UINT  ICON = 0,
    int x = CW_USEDEFAULT,
    int y = CW_USEDEFAULT,
    int nWidth = CW_USEDEFAULT,
    int nHeight = CW_USEDEFAULT);