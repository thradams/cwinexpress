
// Copyright (C) 2010, Thiago Adams (thiago.adams@gmail.com)
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

//This is a experimental code.

#include "stdafx.h"
//#pragma once

#include <windows.h>
#include <commctrl.h>
#include <Commdlg.h>

//Gradiente
#pragma comment( lib, "msimg32.lib" )
#pragma comment( lib, "comctl32.lib" )
//create image
//#pragma comment( lib, "comctl32.lib" )



#ifndef ASSERT
#define ASSERT(x)
#endif

#include <string.h>

#define WIN32_EXPRESS_USE_MEMORY_DC


#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif



//extern CDPI g_metrics;
extern HWND hDlgCurrent;


 bool IsControlKeyPressed()
{
    return (GetKeyState(VK_CONTROL) & 0xf000) == 0xf000;
}

 bool IsShiftKeyPressed()
{
    return (GetKeyState(VK_SHIFT) & 0xf000) == 0xf000;
}

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)    ((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)    ((int)(short)HIWORD(lParam))
#endif



 DWORD GetStyle(HWND m_hWnd)
{
    ASSERT(IsWindow(m_hWnd));
    return (DWORD)GetWindowLong(m_hWnd, GWL_STYLE);
}

 BOOL CenterWindow(HWND m_hWnd, HWND hWndCenter = NULL)
{
    ASSERT(IsWindow(m_hWnd));

    // determine owner window to center against
    DWORD dwStyle = GetStyle(m_hWnd);
    if (hWndCenter == NULL)
    {
        if (dwStyle & WS_CHILD)
            hWndCenter = GetParent(m_hWnd);
        else
            hWndCenter = GetWindow(m_hWnd, GW_OWNER);
    }

    // get coordinates of the window relative to its parent
    RECT rcDlg;
    GetWindowRect(m_hWnd, &rcDlg);
    RECT rcArea;
    RECT rcCenter;
    HWND hWndParent;
    if (!(dwStyle & WS_CHILD))
    {
        // don't center against invisible or minimized windows
        if (hWndCenter != NULL)
        {
            DWORD dwStyleCenter = GetWindowLong(hWndCenter, GWL_STYLE);
            if (!(dwStyleCenter & WS_VISIBLE) || (dwStyleCenter & WS_MINIMIZE))
                hWndCenter = NULL;
        }

        // center within screen coordinates
#if WINVER < 0x0500
        SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
#else
        HMONITOR hMonitor = NULL;
        if (hWndCenter != NULL)
        {
            hMonitor = MonitorFromWindow(hWndCenter,
                MONITOR_DEFAULTTONEAREST);
        }
        else
        {
            hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
        }

        //ATLENSURE_RETURN_VAL(hMonitor != NULL, FALSE);

        MONITORINFO minfo;
        minfo.cbSize = sizeof(MONITORINFO);
        BOOL bResult = GetMonitorInfo(hMonitor, &minfo);
        //ATLENSURE_RETURN_VAL(bResult, FALSE);

        rcArea = minfo.rcWork;
#endif
        if (hWndCenter == NULL)
            rcCenter = rcArea;
        else
            GetWindowRect(hWndCenter, &rcCenter);
    }
    else
    {
        // center within parent client coordinates
        hWndParent = GetParent(m_hWnd);
        ASSERT(IsWindow(hWndParent));

        GetClientRect(hWndParent, &rcArea);
        ASSERT(IsWindow(hWndCenter));
        GetClientRect(hWndCenter, &rcCenter);
        MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
    }

    int DlgWidth = rcDlg.right - rcDlg.left;
    int DlgHeight = rcDlg.bottom - rcDlg.top;

    // find dialog's upper left based on rcCenter
    int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
    int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

    // if the dialog is outside the screen, move it inside
    if (xLeft + DlgWidth > rcArea.right)
        xLeft = rcArea.right - DlgWidth;
    if (xLeft < rcArea.left)
        xLeft = rcArea.left;

    if (yTop + DlgHeight > rcArea.bottom)
        yTop = rcArea.bottom - DlgHeight;
    if (yTop < rcArea.top)
        yTop = rcArea.top;

    // map screen coordinates to child coordinates
    return SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}





/////////////////////////////////////////////////////////////////////////////
// IsDialogEx
#pragma pack(push, 1)


struct DLGTEMPLATEEX
{
    WORD dlgVer;
    WORD signature;
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    WORD cDlgItems;
    short x;
    short y;
    short cx;
    short cy;

    // Everything else in this structure is variable length,
    // and therefore must be determined dynamically

    // sz_Or_Ord menu;   // name or ordinal of a menu resource
    // sz_Or_Ord windowClass; // name or ordinal of a window class
    // WCHAR title[titleLen]; // title string of the dialog box
    // short pointsize;   // only if DS_SETFONT is set 
    // short weight;   // only if DS_SETFONT is set
    // short bItalic;   // only if DS_SETFONT is set
    // WCHAR font[fontLen];  // typeface name, if DS_SETFONT is set
};
struct DLGITEMTEMPLATEEX
{
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    short x;
    short y;
    short cx;
    short cy;
    DWORD id;

    // Everything else in this structure is variable length,
    // and therefore must be determined dynamically

    // sz_Or_Ord windowClass; // name or ordinal of a window class
    // sz_Or_Ord title;   // title string or ordinal of a resource
    // WORD extraCount;   // bytes following creation data
};
#pragma pack(pop)

 BOOL IsDialogEx(const DLGTEMPLATE* pTemplate)
{
    return ((DLGTEMPLATEEX*)pTemplate)->signature == 0xFFFF;
}

/////////////////////////////////////////////////////////////////////////////
// HasFont

 BOOL HasFont(const DLGTEMPLATE* pTemplate)
{
    return (DS_SETFONT &
        (IsDialogEx(pTemplate) ? ((DLGTEMPLATEEX*)pTemplate)->style :
            pTemplate->style));
}

/////////////////////////////////////////////////////////////////////////////
// FontAttrSize

 int FontAttrSize(BOOL bDialogEx)
{
    return (int)sizeof(WORD) * (bDialogEx ? 3 : 1);
}

 WCHAR* _SkipString(__in_z WCHAR* p)
{
    while (*p++);
    return p;
}

 BYTE*  GetFontSizeField(const DLGTEMPLATE* pTemplate)
{
    BOOL bDialogEx = IsDialogEx(pTemplate);
    WORD* pw;

    if (bDialogEx)
        pw = (WORD*)((DLGTEMPLATEEX*)pTemplate + 1);
    else
        pw = (WORD*)(pTemplate + 1);

    if (*pw == (WORD)-1)        // Skip menu name string or ordinal
        pw += 2; // WORDs
    else
        while (*pw++);

    if (*pw == (WORD)-1)        // Skip class name string or ordinal
        pw += 2; // WORDs
    else
        while (*pw++);

    while (*pw++);          // Skip caption string

    return (BYTE*)pw;
}

 UINT GetTemplateSize(const DLGTEMPLATE* pTemplate)
{
    BOOL bDialogEx = IsDialogEx(pTemplate);
    BYTE* pb = GetFontSizeField(pTemplate);

    if (HasFont(pTemplate))
    {
        // Skip font size and name
        pb += FontAttrSize(bDialogEx);  // Skip font size, weight, (italic, charset)
        pb += 2 * (wcslen((WCHAR*)pb) + 1);
    }

    WORD nCtrl = bDialogEx ? (WORD)((DLGTEMPLATEEX*)pTemplate)->cDlgItems :
        (WORD)pTemplate->cdit;

    while (nCtrl > 0)
    {
        pb = (BYTE*)(((DWORD_PTR)pb + 3) & ~DWORD_PTR(3)); // DWORD align

        pb += (bDialogEx ? sizeof(DLGITEMTEMPLATEEX) : sizeof(DLGITEMTEMPLATE));

        if (*(WORD*)pb == (WORD)-1)     // Skip class name string or ordinal
            pb += 2 * sizeof(WORD);
        else
            pb = (BYTE*)_SkipString((WCHAR*)pb);

        if (*(WORD*)pb == (WORD)-1)     // Skip text string or ordinal
            pb += 2 * sizeof(WORD);
        else
            pb = (BYTE*)_SkipString((WCHAR*)pb);

        WORD cbExtra = *(WORD*)pb;      // Skip extra data
        if (cbExtra != 0 && !bDialogEx)
            cbExtra -= 2;
        pb += sizeof(WORD) + cbExtra;
        --nCtrl;
    }

    //IA64: Max dialog template size of 4GB should be fine
    return UINT(pb - (BYTE*)pTemplate);
}

 void GetSystemIconFont(int& nPointSize)
{
   // strFontName = L"MS Shell Dlg"; // out
    nPointSize = 8; //out

   // OSVERSIONINFO osvi;
  //  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
  //  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   // GetVersionEx(&osvi);

    NONCLIENTMETRICS metrics;
   // if (osvi.dwMajorVersion <= 5)
   // {
        // If an application that is compiled for Windows Server 2008 or Windows 
        // Vista must also run on Windows Server 2003 or Windows XP/2000, 
        // use the GetVersionEx function to check the operating system version at 
        // run time and, if the application is running on Windows Server 2003 or 
        // Windows XP/2000, subtract the size of the iPaddedBorderWidth member 
        // from the cbSize member of the NONCLIENTMETRICS structure before 
        // calling the SystemParametersInfo function.

    //    metrics.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(metrics.iPaddedBorderWidth);
    //}
    //else
   // {
        metrics.cbSize = sizeof(NONCLIENTMETRICS);
   // }

    if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
        sizeof(NONCLIENTMETRICS), &metrics, 0) != 0)
    {
        HDC hDC = GetDC(NULL);
        int nLPixY = GetDeviceCaps(hDC, LOGPIXELSY);
        ReleaseDC(NULL, hDC);

        // copy font parameters ///nao sei bem pq 72 mas funciona?
        nPointSize = -MulDiv(metrics.lfMessageFont.lfHeight, 72, nLPixY);
       // strFontName = metrics.lfMessageFont.lfFaceName;
    }
    else
    {
        //ASSERT(false);//
    }
}


 BOOL SetFont(DLGTEMPLATE* pTemplate,
     LPCTSTR lpFaceName, WORD nFontSize)
{
    DWORD dwTemplateSize = GetTemplateSize(pTemplate);
    BOOL bDialogEx = IsDialogEx(pTemplate);
    BOOL bHasFont = HasFont(pTemplate);
    int cbFontAttr = FontAttrSize(bDialogEx);

    if (bDialogEx)
        ((DLGTEMPLATEEX*)pTemplate)->style |= DS_SETFONT;
    else
        pTemplate->style |= DS_SETFONT;

    int nFaceNameLen = lstrlen(lpFaceName);
    if (nFaceNameLen >= LF_FACESIZE)
    {
        // Name too long
        return FALSE;
    }

#ifdef _UNICODE
    int cbNew = cbFontAttr + ((nFaceNameLen + 1) * sizeof(TCHAR));
    BYTE* pbNew = (BYTE*)lpFaceName;
#else
    WCHAR wszFaceName[LF_FACESIZE];
    int cbNew = cbFontAttr + 2 * MultiByteToWideChar(CP_ACP, 0, lpFaceName, -1, wszFaceName, LF_FACESIZE);
    BYTE* pbNew = (BYTE*)wszFaceName;
#endif
    if (cbNew < cbFontAttr)
    {
        return FALSE;
    }
    BYTE* pb = GetFontSizeField(pTemplate);
    int cbOld = (int)(bHasFont ? cbFontAttr + 2 * (wcslen((WCHAR*)(pb + cbFontAttr)) + 1) : 0);

    BYTE* pOldControls = (BYTE*)(((DWORD_PTR)pb + cbOld + 3) & ~DWORD_PTR(3));
    BYTE* pNewControls = (BYTE*)(((DWORD_PTR)pb + cbNew + 3) & ~DWORD_PTR(3));

    WORD nCtrl = bDialogEx ? (WORD)((DLGTEMPLATEEX*)pTemplate)->cDlgItems :
        (WORD)pTemplate->cdit;

    if (cbNew != cbOld && nCtrl > 0)
    {
        size_t nBuffLeftSize = (size_t)(dwTemplateSize - (pOldControls - (BYTE*)pTemplate));
        if (nBuffLeftSize > dwTemplateSize)
        {
            return FALSE;
        }
        memmove_s(pNewControls, nBuffLeftSize, pOldControls, nBuffLeftSize);
    }

    *(WORD*)pb = nFontSize;
    memmove_s(pb + cbFontAttr, cbNew - cbFontAttr, pbNew, cbNew - cbFontAttr);

    //IA64: Max dialog template size of 4GB should be fine
    dwTemplateSize += ULONG(pNewControls - pOldControls);

    return TRUE;
}

void GetSystemIconFont(LOGFONT& lf);




HWND Create(WNDPROC winproc,
    PCWSTR lpWindowName,
    PCWSTR className,
    void* pPointer,
        DWORD dwStyle,
        DWORD dwExStyle ,
        HWND hWndParent ,
        UINT  MENUIDD ,
        UINT  ICON ,
        int x ,
        int y ,
        int nWidth ,
        int nHeight )

    {
       // wchar_t ws[1000];
       // mbstowcs(ws, typeid(T).name(), 1000);

        HINSTANCE hInstance = GetModuleHandle(NULL);

        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcex.lpfnWndProc = winproc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;

        wcex.hIcon = 0;// LoadIcon(hInstance, MAKEINTRESOURCE(IconId));

        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        if (MENUIDD != NULL)
            wcex.lpszMenuName = MAKEINTRESOURCE(MENUIDD);
        else
            wcex.lpszMenuName = NULL;

        wcex.lpszClassName = className;//TGetClassName();

        if (ICON)
        {
            wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(ICON));
        }
        else
            wcex.hIconSm = NULL;


        RegisterClassEx(&wcex);


        HWND hWnd = CreateWindowEx(dwExStyle, className/* TGetClassName()*/, lpWindowName, dwStyle, x, y,
            nWidth, nHeight, hWndParent, 0, GetModuleHandle(NULL), pPointer);
	

        ShowWindow(hWnd, TRUE);
        UpdateWindow(hWnd);
        return hWnd;
    }


 void GetSystemIconFont(LOGFONT& lf)
{
    // get LOGFONT structure for the icon font
    SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0);
}




 void FillSolidRect(HDC hDC, LPCRECT lpRect, COLORREF clr)
{
    COLORREF clrOld = SetBkColor(hDC, clr);

    if (clrOld != CLR_INVALID)
    {
        ExtTextOut(hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
        SetBkColor(hDC, clrOld);
    }
}

 void FillSolidRect(HDC hdc, int left, int top, int right, int bottom, COLORREF clr)
{
    RECT rc = { left, top, right, bottom };
    FillSolidRect(hdc, &rc, clr);
}

 void Combo_DropDown(HWND hCombo)
{
    SendMessage(hCombo, (UINT)CB_SHOWDROPDOWN, TRUE, 0);
}

 void Combo_AddString(HWND hCombo, LPCTSTR psz)
{
    SendMessage(hCombo, (UINT)CB_ADDSTRING, 0, (LPARAM)psz);
}

 void Combo_ResetContent(HWND hWndControl)
{
    SendMessage(hWndControl, (UINT)CB_RESETCONTENT, 0, 0);
}

 int Combo_FindStringExact(HWND hWndControl, int nIndexStart, LPCTSTR lpszFind)
{
    return (int)SendMessage(hWndControl, CB_FINDSTRINGEXACT, nIndexStart, (LPARAM)lpszFind);
}

 int Combo_SetCurSel(HWND hWnd, int nSelect)
{
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, CB_SETCURSEL, nSelect, 0L);
}

 int Combo_GetCurSel(HWND hWndControl)
{
    return (int)SendMessage(hWndControl, CB_GETCURSEL, 0, 0L);
}


 int Combo_GetLBTextLen(HWND hWnd, int nIndex)
{
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, CB_GETLBTEXTLEN, nIndex, 0L);
}

 int Combo_GetLBText(HWND hWnd, int nIndex, LPTSTR lpszText)
{
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, CB_GETLBTEXT, nIndex, (LPARAM)lpszText);
}

 int Combo_GetLBText(HWND hWnd, int nIndex)
{
    ASSERT(IsWindow(hWnd));
    int cchLen = Combo_GetLBTextLen(hWnd, nIndex);
    if (cchLen == CB_ERR)
        return CB_ERR;
    int nRet = CB_ERR;
    //strText.resize(cchLen);
    LPTSTR lpstr = NULL;// &strText[0];
    if (lpstr != NULL)
    {
        nRet = Combo_GetLBText(hWnd, nIndex, lpstr);
    }
    return nRet;
}



 void DrawRectangle(HDC hdc, int x0, int y0, int x1, int y1, COLORREF cr)
{
    LOGBRUSH lb;
    lb.lbColor = 0;
    lb.lbHatch = HS_CROSS;
    lb.lbStyle = BS_NULL;

    HBRUSH hBrush = CreateBrushIndirect(&lb);

    HPEN hPen = CreatePen(PS_SOLID, 0, cr);

    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);


    Rectangle(hdc, x0, y0, x1, y1);


    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

 void DrawRectangle(HDC hdc, LPRECT prc, COLORREF cr)
{
    DrawRectangle(hdc, prc->left, prc->top, prc->right, prc->bottom, cr);
}

 void DrawSolidRectangle(HDC hdc, LPRECT prc, COLORREF fill, COLORREF cr)
{
    FillSolidRect(hdc, prc, fill);
    DrawRectangle(hdc, prc->left, prc->top, prc->right, prc->bottom, cr);
}

 int ShowPopupMenu(HWND hWnd,
    int x,
    int y)
{
    HMENU hMenu = CreatePopupMenu();

   // for (int i = 0; i < (int)items.size(); i++)
    //{
      //  if (items[i] == L"-")
        //    AppendMenu(hMenu, MF_SEPARATOR, i + 1, _T(""));
       // else
         //   AppendMenu(hMenu, MF_STRING, i + 1, items[i].c_str());
    //}

    POINT pt = { x, y };
    ClientToScreen(hWnd, &pt);
    BOOL b = TrackPopupMenu(hMenu,
        TPM_LEFTALIGN | TPM_RETURNCMD,
        pt.x,
        pt.y,
        0,
        hWnd,
        0);

    DestroyMenu(hMenu);
    return b;
}


///////////////////////////////////////////////////////////////

 void GradientRectH(HDC hDC, RECT& rect, COLORREF fore, COLORREF bk)
{
    TRIVERTEX v[2];
    GRADIENT_RECT r[1];

    v[0].x = (rect.left);
    v[0].y = (rect.top);
    v[0].Red = GetRValue(bk) << 8;
    v[0].Green = GetGValue(bk) << 8;
    v[0].Blue = GetBValue(bk) << 8;
    v[0].Alpha = 0x0000;


    v[1].x = (rect.right);
    v[1].y = (rect.bottom);
    v[1].Red = GetRValue(fore) << 8;
    v[1].Green = GetGValue(fore) << 8;
    v[1].Blue = GetBValue(fore) << 8;
    v[1].Alpha = 0x0000;

    r[0].UpperLeft = 0;
    r[0].LowerRight = 1;

    GradientFill(hDC, v, (ULONG)2, r, (ULONG)1, GRADIENT_FILL_RECT_H);
}

 void FillGradientRect(HDC hDC, RECT& rect, COLORREF fore, COLORREF bk, ULONG dwMode)
{
    TRIVERTEX v[2];
    GRADIENT_RECT r[1];

    v[0].x = (rect.left);
    v[0].y = (rect.top);
    v[0].Red = GetRValue(bk) << 8;
    v[0].Green = GetGValue(bk) << 8;
    v[0].Blue = GetBValue(bk) << 8;
    v[0].Alpha = 0x0000;


    v[1].x = (rect.right);
    v[1].y = (rect.bottom);
    v[1].Red = GetRValue(fore) << 8;
    v[1].Green = GetGValue(fore) << 8;
    v[1].Blue = GetBValue(fore) << 8;
    v[1].Alpha = 0x0000;

    r[0].UpperLeft = 0;
    r[0].LowerRight = 1;

    GradientFill(hDC, v, (ULONG)2, r, (ULONG)1, dwMode);
}

//, GRADIENT_FILL_RECT_H
 void GradientRectV(HDC hdc, RECT& rect, COLORREF fore, COLORREF bk, ULONG dwMode = GRADIENT_FILL_RECT_V)
{
    FillGradientRect(hdc, rect, fore, bk, GRADIENT_FILL_RECT_V);
}
 void GradientRectH(HDC hdc, RECT& rect, COLORREF fore, COLORREF bk, ULONG dwMode = GRADIENT_FILL_RECT_V)
{
    FillGradientRect(hdc, rect, fore, bk, GRADIENT_FILL_RECT_H);
}


 void Edit_SetSel(HWND hWnd, int nStartChar, int nEndChar, BOOL bNoScroll = TRUE)
{
    ASSERT(IsWindow(hWnd));
    SendMessage(hWnd, EM_SETSEL, nStartChar, nEndChar);
    if (!bNoScroll)
        SendMessage(hWnd, EM_SCROLLCARET, 0, 0);
}

 void Edit_SelectAll(HWND hWnd)
{
    Edit_SetSel(hWnd, 0, -1);
}


//rich edit

#include <Richedit.h>

 void RichEdit_ReplaceSel(HWND hWnd, LPCTSTR psz)
{
    SendMessage(hWnd, (UINT)EM_REPLACESEL, FALSE, (LPARAM)psz);
}


 void RichEdit_GetSelText(HWND hWnd)
{
    ASSERT(IsWindow(hWnd));
    CHARRANGE cr;
    cr.cpMin = cr.cpMax = 0;
    SendMessage(hWnd, EM_EXGETSEL, 0, (LPARAM)&cr);
    //stdvector<wchar_t> strText;
    //strText.resize((cr.cpMax - cr.cpMin + 1) * 2);
    //LPTSTR lpsz=strText.GetBufferSetLength((cr.cpMax - cr.cpMin + 1) * 2);
    //lpsz[0] = NULL;
//    SendMessage(hWnd, EM_GETSELTEXT, 0, (LPARAM)&strText[0]);
    //strText.ReleaseBuffer();
    //return stdwstring(&strText[0]);
}


 void RichEdit_GetSel(HWND hWnd, long& nStartChar, long& nEndChar)
{
    ASSERT(IsWindow(hWnd));
    CHARRANGE cr;
    SendMessage(hWnd, EM_EXGETSEL, 0, (LPARAM)&cr);
    nStartChar = cr.cpMin;
    nEndChar = cr.cpMax;
}

 void RichEdit_SetSel(HWND hWnd, long nStartChar, long nEndChar)
{
    ASSERT(IsWindow(hWnd));
    CHARRANGE cr;
    cr.cpMin = nStartChar;
    cr.cpMax = nEndChar;
    SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&cr);
}

 BOOL RichEdit_LineScroll(HWND hWnd, int nLines, int nChars = 0)
{
    ASSERT(IsWindow(hWnd));
    return (BOOL)SendMessage(hWnd, EM_LINESCROLL, nChars, nLines);
}

 void RichEdit_Clear(HWND hWnd)
{
    ASSERT(IsWindow(hWnd));
    SendMessage(hWnd, WM_CLEAR, 0, 0L);
}

 BOOL RichEditCtrl_SetReadOnly(HWND hWnd, BOOL bReadOnly)
{
    ASSERT(IsWindow(hWnd));
    return (BOOL)SendMessage(hWnd, EM_SETREADONLY, bReadOnly, 0L);
}


 int RichEdit_GetLineCount(HWND hWnd);

 void RichEdit_ClearAll(HWND hWnd)
{
    RichEditCtrl_SetReadOnly(hWnd, FALSE);
    RichEdit_SetSel(hWnd, 0, -1);
    RichEdit_Clear(hWnd);
}

 int RichEdit_GetLineCount(HWND hWnd)
{
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, EM_GETLINECOUNT, 0, 0);
}

 int RichEdit_LineIndex(HWND hWnd, int nLine = -1)
{
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, EM_LINEINDEX, nLine, 0L);
}

 int RichEdit_LineLength(HWND hWnd, int nLine = -1)
{
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, EM_LINELENGTH, nLine, 0L);
}

 int RichEdit_LineFromChar(HWND hWnd, int nIndex = -1)
{
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, EM_LINEFROMCHAR, nIndex, 0L);
}

 DWORD RichEdit_GetSelectionCharFormat(HWND hWnd, CHARFORMAT &cf)
{
    ASSERT(IsWindow(hWnd));
    cf.cbSize = sizeof(CHARFORMAT);
    return (DWORD)SendMessage(hWnd, EM_GETCHARFORMAT, 1, (LPARAM)&cf);
}

 DWORD RichEdit_GetSelectionCharFormat(HWND hWnd, CHARFORMAT2 &cf)
{
    ASSERT(IsWindow(hWnd));
    cf.cbSize = sizeof(CHARFORMAT2);
    return (DWORD)SendMessage(hWnd, EM_GETCHARFORMAT, 1, (LPARAM)&cf);
}

 BOOL RichEdit_SetSelectionCharFormat(HWND hWnd, CHARFORMAT &cf)
{
    ASSERT(IsWindow(hWnd));
    cf.cbSize = sizeof(CHARFORMAT);
    return (BOOL)SendMessage(hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

 BOOL RichEdit_SetSelectionCharFormat(HWND hWnd, CHARFORMAT2 &cf)
{
    ASSERT(IsWindow(hWnd));
    cf.cbSize = sizeof(CHARFORMAT2);
    return (BOOL)SendMessage(hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}
 int RichEdit_GetFirstVisibleLine(HWND hWnd)
{
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, EM_GETFIRSTVISIBLELINE, 0, 0L);
}

 int RichEdit_SetEventMask(HWND hWnd, DWORD dwMask)
{
    //ENM_MOUSEEVENTS
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, EM_SETEVENTMASK, 0, dwMask);
}

 void RichEdit_Copy(HWND hWnd)
{
    ASSERT(IsWindow(hWnd));
    SendMessage(hWnd, WM_COPY, 0, 0L);
}

 LONG RichEdit_FindText(HWND hWnd, DWORD dwFlags, FINDTEXT& ft)
{
    ASSERT(IsWindow(hWnd));
#if (_RICHEDIT_VER >= 0x0200) && defined(_UNICODE)
    return (LONG)SendMessage(hWnd, EM_FINDTEXTW, dwFlags, (LPARAM)&ft);
#else
    return (LONG)SendMessage(m_hWnd, EM_FINDTEXT, dwFlags, (LPARAM)&ft);
#endif
}

 COLORREF RichEdit_SetBackgroundColor(HWND hWnd, BOOL bSysColor, COLORREF cr)
{
    ASSERT(IsWindow(hWnd));
    return (COLORREF)SendMessage(hWnd, EM_SETBKGNDCOLOR, bSysColor, cr);
}


 void RichEdit_InsertText(HWND edit,
    LPCTSTR psz,
    COLORREF color,
    BOOL bold,
    BOOL strikeout)
{
    CHARFORMAT cf0;
    RichEdit_GetSelectionCharFormat(edit, cf0);

    RichEdit_ReplaceSel(edit, psz);

    LONG s, e;
    RichEdit_GetSel(edit, s, e);
    RichEdit_SetSel(edit, e - wcslen(psz), e + 1);



    CHARFORMAT cf;

    // Modify the selection format so that the selected text is 
    // displayed in bold and not striked out.
    cf.cbSize = sizeof(cf);
    cf.crTextColor = color;

    cf.dwMask = CFM_COLOR | CFM_BOLD;
    cf.dwEffects = 0;

    if (bold)
    {
        cf.dwMask |= CFM_BOLD;
        cf.dwEffects |= CFE_BOLD;
    }

    if (strikeout)
    {
        cf.dwEffects |= CFE_STRIKEOUT;
    }
    cf.dwMask |= CFM_STRIKEOUT;

    RichEdit_SetSelectionCharFormat(edit, cf);
    RichEdit_SetSel(edit, e + 1, e + 1);
    RichEdit_SetSelectionCharFormat(edit, cf0);
}

 UINT Button_GetState(HWND hWnd)
{
    ASSERT(IsWindow(hWnd));
    return (UINT)SendMessage(hWnd, BM_GETSTATE, 0, 0);
}


 int ComboBox_AddString(HWND hWnd, LPCTSTR lpszString)
{
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)lpszString);
}

 void Window_SetFont(HWND hWnd, HFONT hFont, BOOL bRedraw = TRUE)
{
    ASSERT(IsWindow(hWnd));
    SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, bRedraw);
}

 DWORD_PTR ComboBox_GetItemData(HWND hWnd, int nIndex)
{
    ASSERT(IsWindow(hWnd));
    return SendMessage(hWnd, CB_GETITEMDATA, nIndex, 0);
}

 int ComboBox_SetItemData(HWND hWnd, int nIndex, DWORD_PTR dwItemData)
{
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, CB_SETITEMDATA, nIndex, (LPARAM)dwItemData);
}


 int ComboBox_GetCount(HWND hWnd)
{
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, CB_GETCOUNT, 0, 0);
}

 int ComboBox_GetCurSel(HWND hWnd)
{
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, CB_GETCURSEL, 0, 0);
}

int ComboBox_SetCurSel(HWND hWnd, int nSelect)
{
    ASSERT(IsWindow(hWnd));
    return (int)SendMessage(hWnd, CB_SETCURSEL, nSelect, 0);
}
