#include <stdio.h>
#include <stdlib.h>
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <malloc.h>

int xWidth, yHeight;
int CountColls = 5;
RECT rct;
int len = 0;
char **str_array = NULL;

void ReadFromFile()
{
    FILE *file;
    file = fopen("MyText.txt", "r");
    if (file == NULL)
        printf("Не удалось открыть файл!");
    else
    {
        char line[256];
        while(fgets(line, 256, file))
        {

            str_array = (char**)realloc(str_array, (len+1)* sizeof(char*));
            str_array[len++] = strdup(line);
        }
        fclose(file);
    }


}


void WinShow(HDC dc)
{
    int MediumSize = xWidth / CountColls;
    int beginX = 0, beginY = 15;
    int MaxTextHeight = 50;
    int ColNumber = 0;
    RECT TextRct;
    HDC memDC = CreateCompatibleDC(dc);
    HBITMAP memBM = CreateCompatibleBitmap(dc, rct.right - rct.left, rct.bottom - rct.top);
    SelectObject(memDC, memBM);
    SelectObject(memDC, GetStockObject(DC_PEN));
    SetDCPenColor(memDC, RGB(0,0,0));
    SelectObject(memDC, GetStockObject(DC_BRUSH));
    SetDCBrushColor(memDC, RGB(255,255,255));
    Rectangle(memDC,0, 0, rct.right, rct.bottom);
    SetTextColor(memDC, RGB(0,0,0));
    for (int i = 0; i < CountColls; i++)
    {
        MoveToEx(memDC, beginX, 0, NULL);
        LineTo(memDC, beginX, yHeight);
        beginX += MediumSize;
    }
    beginX = 10;
    for (int i = 0; i <  len; i++)
    {
        if (ColNumber == CountColls)
        {
            beginY += MaxTextHeight + 20;
            ColNumber = 0;
            MoveToEx(memDC, 0, beginY, NULL);
            LineTo(memDC, xWidth, beginY);
            beginX = 10;
            beginY += 15;
        }

        SetRect(&TextRct, beginX, beginY, beginX + MediumSize - 15, NULL);
        int TextHeight = DrawText(memDC, (LPCSTR)str_array[i],strlen(str_array[i]), &TextRct, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK | DT_NOCLIP);
        if (TextHeight > MaxTextHeight)
            MaxTextHeight = TextHeight;
        ColNumber++;
        beginX +=MediumSize;

    }

    BitBlt(dc, 0, 0,rct.right - rct.left, rct.bottom - rct.top, memDC, 0, 0, SRCCOPY);
    DeleteDC(memDC);
    DeleteObject(memBM);
}

LRESULT  WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC dc = GetDC(hwnd);
    HFONT hfont;
if (message == WM_DESTROY)
{
    for(int i = 0; i < len; i++)
        free(str_array[i]);
    free(str_array);
    PostQuitMessage(0);
}

else if (message == WM_CREATE)
{
       ReadFromFile();
    hfont = CreateFont(0,0,0,0,FW_REGULAR, FALSE,FALSE,FALSE, RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, NULL);
}
else if (message == WM_SIZE)
{
    GetClientRect(hwnd, &rct);
    xWidth = rct.right;
    yHeight = rct.bottom;
    printf("%d x %d \n", xWidth,yHeight);
    WinShow(dc);
}
else return DefWindowProcA(hwnd, message, wParam, lParam);
}

int main()
{
WNDCLASSA wcl;
        memset(&wcl, 0, sizeof(WNDCLASSA));
        wcl.lpszClassName = "my Window";
        wcl.lpfnWndProc = WndProc;
        wcl.hCursor = LoadCursorA(NULL, IDC_ARROW);
    RegisterClassA(&wcl);

    HWND hwnd;

    hwnd = CreateWindow("my Window", "TextOut", WS_OVERLAPPEDWINDOW, 0, 50, 1280, 1024, NULL, NULL, NULL, NULL);
    ShowWindow(hwnd, SW_SHOWNORMAL);

    MSG msg;

    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
