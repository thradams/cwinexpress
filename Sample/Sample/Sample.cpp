// Sample.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Sample.h"
#include "cwinexpress.h"

typedef struct
{
    HWND hwnd;
} MainWindow;

#include "mainwindowproc.c"

void MainWindow_OnCommand(MainWindow* p, int wmId)
{
}

void MainWindow_OnPaint(MainWindow* p, HDC hdc)
{
}

void MainWindow_OnDestroy(MainWindow* p)
{

}
void MainWindow_Create(MainWindow* p)
{
    p->hwnd = Create(&MainWindow_WndProc,
        L"MainWindow", L"MainWindow", p,
        WS_OVERLAPPEDWINDOW | WS_MAXIMIZE, 0, NULL, IDC_SAMPLE, IDI_SAMPLE);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    
    MainWindow mainWindow;

    MainWindow_Create(&mainWindow);

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
