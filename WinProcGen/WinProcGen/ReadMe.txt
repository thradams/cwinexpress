#include "stdafx.h"
#include "MainWindow.h"
#include "Resource.h"
#include "cwinexpress.h"

typedef struct
{
    HWND hwnd;
} AboutDlg;

void AboutDlg_OnInitDialog(AboutDlg* p)
{
}

//includes the code for the main window here
//call WinProcGen.exe MainWindow.cpp MainWindowProc.c
#include "mainwindowproc.c"

void MainWindow_OnMenuCommand_Exit(MainWindow* p)
{
    PostQuitMessage(0);
}

void MainWindow_OnMenuCommand_About(MainWindow* p)
{
    MessageBox(p->hwnd, L"Sample application!", L"About", MB_ICONINFORMATION);
}


void MainWindow_OnClose(MainWindow* p)
{
    PostQuitMessage(0);
}



void MainWindow_OnPaint(MainWindow* p, HDC hdc)
{
    TextOut(hdc, 10, 10, L"Hello World!", 12);
}

void MainWindow_OnDestroy(MainWindow* p)
{

}

void MainWindow_Create(MainWindow* p)
{
    p->hwnd = Create(&MainWindow_WndProc,
        L"MainWindow",
        L"MainWindow",
        p,
        WS_OVERLAPPEDWINDOW | WS_MAXIMIZE,
        0,
        NULL,
        IDC_SAMPLE,
        IDI_SAMPLE);
}
