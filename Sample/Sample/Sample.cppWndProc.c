WORD MainWindow_OnCommand(MainWindow* p);
WORD MainWindow_OnDestroy(MainWindow* p);
WORD MainWindow_OnPaint(MainWindow* p, HDC hdc);

/*
    Generated windows procedure for MainWindow
*/
LRESULT CALLBACK MainWindow_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            MainWindow_OnCommand(p);
        }
    case WM_DESTROY:
        {
            MainWindow_OnDestroy(p);
        }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            MainWindow_OnPaint(p, hdc);
            EndPaint(hWnd, &ps);
        }
    }
  }
