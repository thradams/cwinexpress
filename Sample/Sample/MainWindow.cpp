#include "stdafx.h"
#include "MainWindow.h"
#include "Resource.h"
#include "cwinexpress.h"

typedef struct
{
    HWND hDlg;
} AboutDlg;

//includes the code for the main window here
//call WinProcGen.exe MainWindow.cpp MainWindowProc.c
#include "mainwindowproc.c"


//This function identifies the dialog procedure instead of winproc
void AboutDlg_OnInitDialog(AboutDlg* p)
{
}

void AboutDlg_OnCommand(AboutDlg* p, int id)
{
    if (id == IDOK || id == IDCANCEL)
    {
        EndDialog(p->hDlg, id);
        //return (INT_PTR)TRUE;
    }
}

void MainWindow_OnMenuCommand_Exit(MainWindow* p)
{
    PostQuitMessage(0);
}

void MainWindow_OnMenuCommand_About(MainWindow* p)
{
    AboutDlg aboutDlg;

    INT_PTR r = DialogBoxParam(
        NULL,
        MAKEINTRESOURCE(IDD_ABOUTBOX),
        p->hwnd,
        &AboutDlg_DlgProc,
        (LPARAM)&aboutDlg
        );
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
