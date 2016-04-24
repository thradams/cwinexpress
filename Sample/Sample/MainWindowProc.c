void MainWindow_OnCommand(MainWindow* p, int wmId);
void MainWindow_OnDestroy(MainWindow* p);
void MainWindow_OnPaint(MainWindow* p, HDC hdc);

/*
    Generated windows procedure for MainWindow
*/
LRESULT CALLBACK MainWindow_WndProc(HWND hWnd,
                                 UINT message,
								 WPARAM wParam,
								 LPARAM lParam)
{
    MainWindow *p = NULL;
	//
    if (message == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        p = (MainWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)p);
    }
    else
    {
        p = (MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }
	//
    switch (message)
    {
    case WM_COMMAND:
        {
            MainWindow_OnCommand(p,  HIWORD(wParam));
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
   default:
    return DefWindowProc(hWnd, message, wParam, lParam);
    }
	return 0;
  }
