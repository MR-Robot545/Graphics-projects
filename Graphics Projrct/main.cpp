#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <cmath>
#include <windows.h>
#include <windowsx.h>
#include <sstream>
#include <tchar.h>
#include <iostream>
#include <list>
#include <stack>
#include <vector>
#include <fstream>
#include "Fill.h"
#include "Clip.h"
#include "Draw.h"
#include "State.h"

using namespace std;
double const PI = 3.14159265358979323846;
RECT rect;
bool isSaved = false;


void swap(int &x, int &y) {
    int temp = x;
    x = y;
    y = temp;
}

void ClearScreen(HWND hWnd) {
    // Get the device context for drawing
    HDC hdc = GetDC(hWnd);

    // Get the dimensions of the client area
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    // Set the background color
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); // White color

    // Fill the client area with the background color
    FillRect(hdc, &clientRect, hBrush);

    // Clean up resources
    DeleteObject(hBrush);
    ReleaseDC(hWnd, hdc);

}

void solidBrush(HWND hWnd, COLORREF color) {
    HDC hdc = GetDC(hWnd);

    // Create a solid brush with the specified color
    HBRUSH hBrush = CreateSolidBrush(color);

    // Select the brush into the device context
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    // Draw a rectangle filled with the stock brush
    FillRect(hdc, &clientRect, hBrush);
    // Restore the previous brush and clean up
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
    ReleaseDC(hWnd, hdc);

}

void stockBrush(HWND hWnd, COLORREF color) {
    HDC hdc = GetDC(hWnd);
    HBRUSH hBrush = (HBRUSH)GetStockBrush(GRAY_BRUSH);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    // Draw a rectangle filled with the stock brush
    FillRect(hdc, &clientRect, hBrush);

    // Restore the previous brush and clean up
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
    ReleaseDC(hWnd, hdc);
}

void drawLineDDABrush(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c, bool load = false) {

    if (!load)
    {
        std::string params = serializeParameters("x1", x1, "y1", y1, "x2", x2, "y2", y2, "Color", c);
        Drawing Line("Line", "DDA", params);
        operations.push_back(Line);
    }
    int count=0;bool ok=false;
    int dx = x2 - x1, dy = y2 - y1;
    if (abs(dx) > abs(dy)) {
        if (x1 > x2)swap(x1, x2), swap(y1, y2);
        int x = x1;
        double y = y1;
        double m = (double)dy / dx;
        while (x < x2) {
            x++,
                    y += m;
            if (count % 5 == 0) {
                ok = !ok;
            }
            if (!ok)
                SetPixel(hdc, x, (int) round(y), c);
            count++;

        }
    }
    else {
        if (y1 > y2)swap(x1, x2), swap(y1, y2);
        int y = y1;
        double x = x1;
        double mi = (double)dx / dy;
        while (y < y2)y++, x += mi, SetPixel(hdc, (int)round(x), y, c);
    }
}

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_CROSS);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx(&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx(
            0,                   /* Extended possibilites for variation */
            szClassName,         /* Classname */
            _T("Code::Blocks Template Windows App"),       /* Title Text */
            WS_OVERLAPPEDWINDOW, /* default window */
            CW_USEDEFAULT,       /* Windows decides the position */
            CW_USEDEFAULT,       /* where the window ends up on the screen */
            544,                 /* The programs width */
            375,                 /* and height in pixels */
            HWND_DESKTOP,        /* The window is a child-window to desktop */
            NULL,                /* No menu */
            hThisInstance,       /* Program Instance handler */
            NULL                 /* No Window Creation data */
    );

    /* Make the window visible on the screen */
    ShowWindow(hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage(&messages, NULL, 0, 0)) {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


int distance(int xc, int yc, int xr, int yr) {
    return (int) sqrt(pow(abs(xr - xc), 2) + pow(abs(yr - yc), 2));
}


LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    hdc = GetDC(hwnd);
    vector<POINT> points;
    static COLORREF color = RGB(0, 0, 0);
    static int xc, yc, xr, yr, c1 = 0, x1, y1, x2, y2, rad, c2 = 1, A = -1, B, length, width, height, side,r,g,b;
    static bool isMouseDown = false;
    static int QuarterNum;
    static vector<Vertex> p;
    static vector<Vertex> p2;
    static int count = 0;
    static int counter = 0;
//    static Vector2 P0;
//    static Vector2 P1;
    static int numpoints = 100;
    static int shape, kind;

    switch (message)                  /* handle the messages */
    {
        case WM_CREATE: {
            // Create a menu
            HMENU hMenu = CreateMenu();
            HMENU hSubMenu = CreatePopupMenu();
            HMENU hubMenu = CreatePopupMenu();
            HMENU brushMenu = CreatePopupMenu();
            AppendMenu(hubMenu, MF_STRING, 1, "Line DDA");
            AppendMenu(hubMenu, MF_STRING, 2, "Line MidPoint");
            AppendMenu(hubMenu, MF_STRING, 3, "Line Parametric");
            AppendMenu(hubMenu, MF_STRING, 4, "Circle Direct");
            AppendMenu(hubMenu, MF_STRING, 5, "Circle Polar");
            AppendMenu(hubMenu, MF_STRING, 6, "Circle Iterative Polar");
            AppendMenu(hubMenu, MF_STRING, 7, "Circle MidPoint");
            AppendMenu(hubMenu, MF_STRING, 8, "Circle Modified MidPoint");
            AppendMenu(hubMenu, MF_STRING, 9, "Filling Quarter By Lines");
            AppendMenu(hubMenu, MF_STRING, 10, "Filling Quarter By Circles");
            AppendMenu(hubMenu, MF_STRING, 11, "Filling Square With Hermit Curve");
            AppendMenu(hubMenu, MF_STRING, 12, "Filling Rectangle With Bezier Curve");
            AppendMenu(hubMenu, MF_STRING, 13, "Recursive Flood Filled");
            AppendMenu(hubMenu, MF_STRING, 14, "NonRecursive Flood Filled");
            AppendMenu(hubMenu, MF_STRING, 15, "Convex Filling");
            AppendMenu(hubMenu, MF_STRING, 16, "NonConvex Filling");
            AppendMenu(hubMenu, MF_STRING, 17, "Cardinal Spline");
            AppendMenu(hubMenu, MF_STRING, 18, "Ellipse Direct");
            AppendMenu(hubMenu, MF_STRING, 19, "Ellipse Polar");
            AppendMenu(hubMenu, MF_STRING, 20, "Ellipse MidPoint");
            AppendMenu(hubMenu, MF_STRING, 21, "Clipping Rectangle Point");
            AppendMenu(hubMenu, MF_STRING, 22, "Clipping Rectangle Line");
            AppendMenu(hubMenu, MF_STRING, 23, "Clipping Rectangle Polygon");
            AppendMenu(hubMenu, MF_STRING, 24, "Clipping Square Point");
            AppendMenu(hubMenu, MF_STRING, 25, "Clipping Square Line");
            AppendMenu(hMenu, MF_POPUP, (UINT_PTR) hubMenu, "Shapes");
            AppendMenu(hSubMenu, MF_STRING, 26, "Red");
            AppendMenu(hSubMenu, MF_STRING, 27, "Green");
            AppendMenu(hSubMenu, MF_STRING, 28, "Blue");
            AppendMenu(hMenu, MF_POPUP, (UINT_PTR) hSubMenu, "Select Color");
            AppendMenu(hMenu, MF_STRING, 29, "Clear");
            AppendMenu(hMenu, MF_STRING, 30, "Save");
            AppendMenu(hMenu, MF_STRING, 31, "Load");
            AppendMenu(hMenu, MF_POPUP, (UINT_PTR)brushMenu, "Select Brush");
            AppendMenu(brushMenu, MF_STRING, 32, "Solid");
            AppendMenu(brushMenu, MF_STRING, 33, "Stock");

            SetMenu(hwnd, hMenu);
            break;
        }
        case WM_LBUTTONDOWN:
            if (shape == 1) {
                if (count == 0) {
                    xc = LOWORD(lParam);
                    yc = HIWORD(lParam);
                    count++;
                } else if (count == 1) {
                    xr = LOWORD(lParam);
                    yr = HIWORD(lParam);
                    if (kind == 1) {
                        drawLineDDA(hdc, xc, yc, xr, yr, color);
                    } else if (kind == 2) {
                        drawLineMidpoint(hdc, xc, yc, xr, yr, color);
                    } else if (kind == 3) {
                        drawLineParametric(hdc, xc, yc, xr, yr, color);
                    }else if(kind==6){
                        drawLineDDABrush(hdc, xc,  yc,  xr,  yr,color,  false);
                    }
                    count = 0;
                    cout<<"Line ("<<xc<<","<<yc<<") , ("<<xr<<","<<yr<<")"<<endl;
                }
            } 
            else if (shape == 2) {
                if (count == 0) {
                    xc = LOWORD(lParam);
                    yc = HIWORD(lParam);
                    count++;
                } else if (count == 1) {
                    xr = LOWORD(lParam);
                    yr = HIWORD(lParam);
                    int rad = distance(xc, yc, xr, yr);
                    if (kind == 1) {
                       drawCircleCartesian(hdc, xc, yc, rad, color);
                    } else if (kind == 2) {
                        drawCirclePolar(hdc, xc, yc, rad, color);
                    } else if (kind == 3) {
                        drawCircleItrPolar(hdc, xc, yc, rad, color);
                    } else if (kind == 4) {
                        drawCircleMidpoint(hdc, xc, yc, rad, color);
                    } else if (kind == 5) {
                        drawCircleModifiedMidpoint(hdc, xc, yc, rad, color);
                    } else if (kind == 6) {
                        cout << "Enter Number Of Quarter" << endl;
                        cin >> QuarterNum;
                        fillCircleQuarter(hdc, xc, yc, rad, QuarterNum, color);
                    } else if (kind == 7) {
                        cout << "Enter Number Of Quarter" << endl;
                        cin >> QuarterNum;
                        FilledCircleQuarterWithCircles(hdc, xc, yc, rad, QuarterNum, color);
                    }
                    count = 0;
                    cout<<"Circle ("<<xc<<","<<yc<<") , ("<<xr<<","<<yr<<")"<<endl;
                }
            }
            else if (shape == 3) {
                if (kind == 2) {
                    if (counter == 0) {
                        rect.left = LOWORD(lParam);
                        rect.top = HIWORD(lParam);
                        counter++;
                    } else if (counter == 1) {
                        rect.right = LOWORD(lParam);
                        rect.bottom = HIWORD(lParam);
                        FillBezierRectangle(hdc, rect, color);
                        cout<<"FillBezierRectangle ("<<rect.left<<","<<rect.top<<") , ("<<rect.right<<","<<rect.bottom<<")"<<endl;
                        counter = 0;
                    }
                } else if (kind == 1) {
                    if (counter == 0) {
                        rect.left = LOWORD(lParam);
                        rect.top = HIWORD(lParam);
                        counter++;
                    } else if (counter == 1) {
                        int side = sqrt(((rect.left - LOWORD(lParam)) * (rect.left - LOWORD(lParam)))
                                        + ((rect.top - HIWORD(lParam)) * (rect.top - HIWORD(lParam))));
                        rect.right = rect.left + side;
                        rect.bottom = rect.top + side;
                        DrawHermitCurveSquare(hdc, rect, color);
                        cout<<"FillBHermitSquare ("<<rect.left<<","<<rect.top<<") , ("<<rect.right<<","<<rect.bottom<<")"<<endl;
                        counter = 0;
                    }
                }
            } 
            else if (shape == 4) {
                if (kind == 1) {
//                    cout<<"Enter Color to Fill (RGB values)"<<endl;
//                    cin>>r>>g>>b;
                    FloodFill(hdc, xc, yc, color, RGB(r, g, b));
//                    cout<<"FloodFill Start at Point("<<xc<<","<<yc<<")"<<endl;
                } else if (kind == 2) {
//                    cout<<"Enter Color to Fill (RGB values)"<<endl;
//                    cin>>r>>g>>b;

                    NRFloodFill(hdc, xc, yc, color, RGB(0, 0, 255));
//                    cout<<"FloodFill Start at Point("<<xc<<","<<yc<<")"<<endl;
                }
            } 
            else if (shape == 5) {
                if (kind == 1) {
                    if (count >= 0) {
                        x1 = LOWORD(lParam);
                        y1 = HIWORD(lParam);
                        p2.emplace_back(Vertex(x1, y1));
                    }
                    if (count == 4) {
                        convexFill(hdc, p2, p2.size(), color);
                        p2.clear();
                        cout<<"ConvexFill"<<endl;
                        count = 0;
                    } else {
                        count++;
                    }
                } else if (kind == 2) {
                    if (count >= 0) {
                        x1 = LOWORD(lParam);
                        y1 = HIWORD(lParam);
                        p2.emplace_back(Vertex(x1, y1));
                    }
                    if (count == 4) {
                        nonConvexFill(hdc, p2, p2.size(), color);
                        p2.clear();
                        cout<<"NonConvexFill"<<endl;
                        count = 0;
                    } else {
                        count++;
                    }
                }
            } 
            else if (shape == 6) {
                if (count <= 6) {
                    p.emplace_back(Vertex(LOWORD(lParam), HIWORD(lParam)));
                }
                if (count == 6) {
                    DrawCardinalSpline(hdc, p, p.size(), 0.5, 7);
                    cout<<"CardinalSpline"<<endl;
                    count = 0;
                } else {
                    count++;
                }
            } 
            else if (shape == 7) {
                if (count == 0) {
                    xc = LOWORD(lParam);
                    yc = HIWORD(lParam);
                    count++;
                } else if (count == 1) {
                    x2 = LOWORD(lParam);
                    y2 = HIWORD(lParam);
                    A = distance(xc, yc, x2, y2);
                    count++;
                } else if (count == 2) {
                    xr = LOWORD(lParam);
                    yr = HIWORD(lParam);
                    B = distance(xc, yc, xr, yr);
                    if (kind == 1) {
                        drawEllipseCartesian(hdc, xc, yc, max(A, B), min(A, B), color);
                    } else if (kind == 2) {
                        drawEllipsePolar(hdc, xc, yc, max(A, B), min(A, B), color);
                    } else if (kind == 3) {
                        drawEllipseMidpoint(hdc, xc, yc, max(A, B), min(A, B), color);
                    }
                    cout<<"Ellipse ("<<xc<<","<<yc<<") , ("<<A<<","<<B<<")"<<endl;
                    count = 0;
                }

            } 
            else if (shape == 8) {
                if (count == 0) {
                    xc = LOWORD(lParam);
                    yc = HIWORD(lParam);
                    count++;
                } else if (count == 1) {
                    xr = LOWORD(lParam);
                    yr = HIWORD(lParam);
                    DrawRectangle(hdc, xc, yc, xr, yr);
                    cout<<"Rectangle ("<<xc<<","<<yc<<") , ("<<xr<<","<<yr<<")"<<endl;
                    count = 0;

                }
            } 
            else if (shape == 9) {
                if (counter == 0) {
                    xc = LOWORD(lParam);
                    yc = HIWORD(lParam);
                    counter++;
                } else if (counter == 1) {
                    side = sqrt(((xc - LOWORD(lParam)) * (xc - LOWORD(lParam)))
                                + ((yc - HIWORD(lParam)) * (yc - HIWORD(lParam))));
                    Square(hdc, xc, yc, side, color);
                    cout<<"Square ("<<xc<<","<<yc<<") , ("<<xr<<","<<yr<<")"<<endl;
                    counter = 0;
                }
            }
            break;
        case WM_LBUTTONUP:
            break;
        case WM_RBUTTONDOWN:
            if (shape == 8) {
                if (kind == 1) {
                    x1 = LOWORD(lParam);
                    y1 = HIWORD(lParam);
                    rectanglePointClipping(hdc, x1, y1, xc, yc, xr, yr, color);
                } else if (kind == 2) {
                    if (c1 == 0) {
                        x1 = LOWORD(lParam);
                        y1 = HIWORD(lParam);
                        c1++;
                    } else if (c1 == 1) {
                        x2 = LOWORD(lParam);
                        y2 = HIWORD(lParam);
                        rectangleLineClipping(hdc, x1, y1, x2, y2, xc, yc, xr, yr, color);
                        c1 = 0;
                    }
                } else if (kind == 3) {
                    if (count <= 4) {
                        x1 = LOWORD(lParam);
                        y1 = HIWORD(lParam);
                        p2.emplace_back(Vertex(x1, y1));
                    }
                    if (count == 4) {
                        rectanglePolygonClip(hdc, p2, p2.size(), xc, yc, xr, yr);
                        p2.clear();
                        count = 0;
                    } else {
                        count++;
                    }
                }
            } 
            else if (shape == 9) {
                if (kind == 1) {
                    x1 = LOWORD(lParam);
                    y1 = HIWORD(lParam);
                    squarePointClipping(hdc, x1, y1, xc, yc, side, color);
                } else if (kind == 2) {
                    if (c1 == 0) {
                        x1 = LOWORD(lParam);
                        y1 = HIWORD(lParam);
                        c1++;
                    } else if (c1 == 1) {
                        x2 = LOWORD(lParam);
                        y2 = HIWORD(lParam);
                        squareLineClipping(hdc, x1, y1, x2, y2, xc, yc, side, color);
                        c1 = 0;
                    }
                }
            }
            break;
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            switch (id) {
                case 1: // Red color menu item ID
                    // Set the drawing color to red
                    shape = 1;
                    kind = 1;
                    break;

                case 2: // Green color menu item ID
                    // Set the drawing color to green
                    shape = 1;
                    kind = 2;
                    break;

                case 3: // Blue color menu item ID
                    // Set the drawing color to blue
                    shape = 1;
                    kind = 3;
                    break;
                case 4:
                    shape = 2;
                    kind = 1;
                    break;
                case 5:
                    shape = 2;
                    kind = 2;
                    break;
                case 6:
                    shape = 2;
                    kind = 3;
                    break;
                case 7:
                    shape = 2;
                    kind = 4;
                    break;
                case 8:
                    shape = 2;
                    kind = 5;
                    break;
                case 9:
                    shape = 2;
                    kind = 6;
                    break;
                case 10:
                    shape = 2;
                    kind = 7;
                    break;
                case 11:
                    shape = 3;
                    kind = 1;
                    break;
                case 12:
                    shape = 3;
                    kind = 2;
                    break;
                case 13:
                    shape = 4;
                    kind = 1;
                    break;
                case 14:
                    shape = 4;
                    kind = 2;
                    break;
                case 15:
                    shape = 5;
                    kind = 1;
                    break;
                case 16:
                    shape = 5;
                    kind = 2;
                    break;
                case 17:
                    shape = 6;
                    kind = 1;
                    break;
                case 18:
                    shape = 7;
                    kind = 1;
                    break;
                case 19:
                    shape = 7;
                    kind = 2;
                    break;
                case 20:
                    shape = 7;
                    kind = 3;
                    break;
                case 21:
                    shape = 8;
                    kind = 1;
                    break;
                case 22:
                    shape = 8;
                    kind = 2;
                    break;
                case 23:
                    shape = 8;
                    kind = 3;
                    break;
                case 24:
                    shape = 9;
                    kind = 1;
                    break;
                case 25:
                    shape = 9;
                    kind = 2;
                    break;
                case 26:
                    color = (RGB(255, 0, 0));
                    break;
                case 27:
                    color = (RGB(0, 255, 0));
                    break;
                case 28:
                    color = (RGB(0, 0, 255));
                    break;
                case 29:
                    ClearScreen(hwnd);
                    break;
                case 30:
                    saveState(operations);
                    break;
                case 31:
                    hdc = GetDC(hwnd);
                    loadState(hdc);
                    ReleaseDC(hwnd, hdc);
                    break;
                case 32:
                    solidBrush(hwnd, color);
                    break;
                case 33:
                    shape=1;
                    kind=6;
                    break;
            }
        }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

