# Windows Express in C

This project contains a WindowsProcedure generator and a C library.

The generator implements the switch cases for each windows event utilized.
The generator include the switch case if the source code has one function with the same name of the event. Theses names are defined in a template file.
For instance, if you source code has a function classed MainWindow_OnPaint then the gerator includes the  WM_PAINT.

See sample 1.

https://github.com/thradams/cwinexpress/blob/master/Sample/Sample/MainWindow.cpp

and the generated file

https://github.com/thradams/cwinexpress/blob/master/Sample/Sample/MainWindowProc.c
