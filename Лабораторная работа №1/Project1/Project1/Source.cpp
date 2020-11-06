#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#define OEMRESOURCE
#define RECT_ID 1
#define IMG_ID 2
#define IDB_BITMAP1 101

#include <Windows.h>
#include <string>

void DrawFunction(HDC hdc, int x, int y, HBITMAP hBitmap) {
	HBITMAP hNewBitmap;

	HDC BhDc;
	BITMAP bitmap;
	int bitmapWidth, bitmapHeight;

	BhDc = CreateCompatibleDC(hdc);

	hNewBitmap = (HBITMAP)SelectObject(BhDc, hBitmap);

	if (hNewBitmap) {
		SetMapMode(BhDc, GetMapMode(hdc));
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap);

		bitmapWidth = bitmap.bmWidth;
		bitmapHeight = bitmap.bmHeight;

		POINT bmSize;
		bmSize.x = bitmapWidth;
		bmSize.y = bitmapHeight;

		DPtoLP(hdc, &bmSize, 1);

		POINT point;
		point.x = 0;
		point.y = 0;

		DPtoLP(BhDc, &point, 1);
		BitBlt(hdc, x, y, bitmapWidth, bitmapHeight, BhDc, point.x, point.y, SRCCOPY);
		SelectObject(BhDc, hNewBitmap);
	}

	DeleteDC(BhDc);
}

int xView{}, yView{};
int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	MSG msg{};
	HWND hwnd{}, hChildWin{};
	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = [](HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
	{
		static int id = RECT_ID;
		PAINTSTRUCT ps;
		HDC hdc, SdC;
		RECT ClientRect;
		static HBRUSH hBrush;
		static HBITMAP MyBmp;
		static int x = 200;
		static int y = 200;
		static int size = 100;
		static int ViewX, ViewY, alpha;
		static int moveStat = 0;
		int mouseOffs;
		switch (uMsg)
		{

			case WM_CREATE: {
				hBrush = CreateSolidBrush(RGB(0, 0, 0));
				MyBmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));

			} return 0;

			case WM_RBUTTONDOWN: {
				if (id == RECT_ID)
					id = IMG_ID;
				else
					id = RECT_ID;

			} return 0;

			case WM_LBUTTONDOWN: {
				if (!moveStat) {
					x = LOWORD(lParam);
					y = HIWORD(lParam);
					alpha = round((rand() % 360 - 180) * 3.14 / 180.0);
					ViewX = 10.0 * cos(alpha);
					ViewY = 10.0 * sin(alpha);
					SetTimer(hwnd, 1, 25, NULL);
					moveStat = 1;
				}
				else {
					KillTimer(hwnd, 1);
					moveStat = 0;
				}

			} return 0;
			case WM_TIMER: {
				x += ViewX;
				y += ViewY;
				GetClientRect(hwnd, &ClientRect);
				if ((x + size) >= ClientRect.right)
					ViewX = -abs(ViewX);
				if (x < ClientRect.left)
					ViewX = abs(ViewX);
				if ((y + size) > ClientRect.bottom)
					ViewY = -abs(ViewY);
				if (y < ClientRect.top)
					ViewY = abs(ViewY);
				x--;
				y--;
				InvalidateRect(hwnd, NULL, TRUE);
			} return 0;

			case WM_PAINT: {
				if (id == RECT_ID) {
					hdc = BeginPaint(hwnd, &ps);
					SelectObject(hdc, hBrush);
					Rectangle(hdc, x, y, x + size, y + size);
					EndPaint(hwnd, &ps);
				}
				else {
					hdc = BeginPaint(hwnd, &ps);
					DrawFunction(hdc, x, y, MyBmp);
					EndPaint(hwnd, &ps);
				} 
			} return 0;

			case WM_KEYDOWN:
			{
				if (!moveStat) {
					GetClientRect(hwnd, &ClientRect);
					switch (wParam) {
					case VK_LEFT: {
						x -= 10;
						if (x < ClientRect.left)
							x += 20;
						break;
					}
					case VK_RIGHT: {
						x += 10;
						if ((x + size) > ClientRect.right)
							x -= 20;
						break;
					}
					case VK_UP: {
						y -= 10;
						if (y < ClientRect.top)
							y += 20;
						break;
					}
					case VK_DOWN: {
						y += 10;
						if ((y + size) > ClientRect.bottom)
							y -= 20;
						break;
					}
					}
					InvalidateRect(hwnd, NULL, TRUE);
				} return 0;
			}
			case WM_MOUSEWHEEL: {
				if (!moveStat) {
					GetClientRect(hwnd, &ClientRect);
					mouseOffs = GET_WHEEL_DELTA_WPARAM(wParam);
					if (LOWORD(wParam) == MK_SHIFT) {
						if (mouseOffs > 0) {
							x += 10;
							if ((x + size) > ClientRect.right)
								x -= 20;
						}
						else {
							x -= 10;
							if (x < ClientRect.left)
								x += 20;
						}
					}
					else {
						if (mouseOffs > 0) {
							y += 10;
							if ((y + size) > ClientRect.bottom)
								y -= 20;
						}
						else {
							y -= 10;
							if (y < ClientRect.top)
								y += 20;
						}
					}
					InvalidateRect(hwnd, NULL, TRUE);
				} return 0;
			}
			case WM_MOUSEMOVE: {
				if (!moveStat) {
					int oldX = x;
					int oldY = y;
					GetClientRect(hwnd, &ClientRect);
					x = LOWORD(lParam);
					y = HIWORD(lParam);
					if (x < ClientRect.left)
						x = oldX;
					if ((x + size) > ClientRect.right)
						x = oldX;
					if (y < ClientRect.top)
						y = oldY;
					if ((y + size) > ClientRect.bottom)
						y = oldY;
					InvalidateRect(hwnd, NULL, TRUE);
				}

			} return 0;

			case WM_DESTROY:
			{
				if (moveStat) {
					KillTimer(hwnd, 1);
				}
				DeleteObject(hBrush);
				DeleteObject(MyBmp);
				PostQuitMessage(EXIT_SUCCESS);
			} return 0;

		} return DefWindowProc(hwnd, uMsg, wParam, lParam);
	};
	wc.lpszClassName = L"MyAppClass";
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc))
		return EXIT_FAILURE;

	if (hwnd = CreateWindow(wc.lpszClassName, L"Moving object!", WS_OVERLAPPEDWINDOW, 100, 100, 600, 600, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return static_cast<int>(msg.wParam);
}