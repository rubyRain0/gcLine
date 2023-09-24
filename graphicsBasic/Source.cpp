#include <Windows.h>

WNDCLASS createWindowClass(HBRUSH bgColor, HCURSOR cursor, HINSTANCE hInstance, HICON icon, LPCWSTR windowName, WNDPROC windowProcedure);
LRESULT CALLBACK windowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

POINT startPoint = { -1, -1 }; // Initialize with invalid values
POINT endPoint = { -1, -1 };   // Initialize with invalid values
COLORREF lineColor = RGB(0, 0, 0);
bool isDrawing = false;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    WNDCLASS mainWindow = createWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInstance, LoadIcon(NULL, IDI_QUESTION), L"MainWndClass", windowProcedure);

    if (!RegisterClassW(&mainWindow))
        return -1;

    CreateWindow(L"MainWndClass", L"MainWindow", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 800, 800, NULL, NULL, NULL, NULL);

    MSG message = { 0 };
    while (GetMessage(&message, NULL, NULL, NULL)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return 0;
}

WNDCLASS createWindowClass(HBRUSH bgColor, HCURSOR cursor, HINSTANCE hInstance, HICON icon, LPCWSTR windowName, WNDPROC windowProcedure)
{
    WNDCLASS wc = { 0 };

    wc.hCursor = cursor;
    wc.hIcon = icon;
    wc.hInstance = hInstance;
    wc.lpszClassName = windowName;
    wc.hbrBackground = bgColor;
    wc.lpfnWndProc = windowProcedure;

    return wc;
}

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    HDC hdc = NULL;
    PAINTSTRUCT ps;

    static RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    switch (msg)
    {
    case WM_RBUTTONDOWN:
        if (!isDrawing)
        {
            isDrawing = true;
            startPoint.x = LOWORD(lp);
            startPoint.y = HIWORD(lp);
            endPoint = startPoint; // Set the end point to the start point initially
            InvalidateRect(hWnd, NULL, true);
        }
        else
        {
            isDrawing = false;
            endPoint.x = LOWORD(lp);
            endPoint.y = HIWORD(lp);
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    case WM_KEYDOWN:
        if (wp == 0x43) // 'C' key to change the color
        {
            lineColor = RGB(rand() % 256, rand() % 256, rand() % 256);
            InvalidateRect(hWnd, NULL, true);
        }
        else if (wp == 0x44) //'D' key to clear the line
        {
            startPoint = { -1, -1 };
            endPoint = { -1, -1 };
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    case WM_PAINT:
        if (!isDrawing)
        {
            hdc = BeginPaint(hWnd, &ps);

            {
                HPEN hPen = CreatePen(PS_SOLID, 2, lineColor); // random color pen, 2-pixel width
                HGDIOBJ hOldPen = SelectObject(hdc, hPen);

                // Draw the line
                MoveToEx(hdc, startPoint.x, startPoint.y, NULL);
                LineTo(hdc, endPoint.x, endPoint.y);

                // Clean up
                SelectObject(hdc, hOldPen);
                DeleteObject(hPen);
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wp, lp);
    }
    return 0;
}
