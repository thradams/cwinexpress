#pragma once
#include <Windows.h>

typedef struct
{
    HWND hwnd;
} MainWindow;

void MainWindow_Create(MainWindow* p);