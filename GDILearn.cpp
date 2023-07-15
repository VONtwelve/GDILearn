// GDILearn.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "GDILearn.h"
#include<time.h>

#define MAX_LOADSTRING 100
#define ST_x 200
#define ST_y 20
#define HLEN 1000
#define WLEN 1200

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//自定义的全局函数
VOID Game_PaintPen(HWND);
VOID Game_PaintBit(HWND);
BOOL Game_Init(HWND);
BOOL Clean_UP(HWND);

//定义全局变量
BOOL is_down = FALSE;
POINT point,last_point;
HDC hdc,hdc1;
HBITMAP bitmap;
HPEN pen[7] = { 0 };//全局变量 画笔
HBRUSH brush[7] = { 0 };//全局变量画刷
int PenStyle[7] = { PS_DASH,PS_SOLID,PS_DOT,PS_DASHDOT,PS_DASHDOTDOT,PS_NULL,PS_INSIDEFRAME };//定义笔的类型方便我们CreatePen
int BrushStyle[6] = { HS_VERTICAL,HS_HORIZONTAL,HS_CROSS,HS_DIAGCROSS,HS_FDIAGONAL,HS_BDIAGONAL };//方便我们定义CreateBrush

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,//主函数入口
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。
    PlaySound(L"鸟之诗.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    //需要的头文件和LIB在GDI的头文件中，声音通过文件名搜索，以异步的方式循环播放。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GDILEARN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);//给WNDCLASSEXW类中的成员赋值（注册窗口类）

    if (!InitInstance (hInstance, nCmdShow)) // 执行应用程序初始化:
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GDILEARN));

    MSG msg = {0};

    // 主消息循环:
    while (msg.message!=WM_QUIT)
    {
        /*
        * 使用PeekMessage而不是用GetMessage，当我们没有消息时不必等待消息。
        */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            //其他的工作
        }
    }

    UnregisterClass(szWindowClass, hInstance);
    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
        
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = 0;//这里我们改变宽或高后不重绘
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));//加载Sai的图片
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);//背景灰色画刷填充
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GDILEARN);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
     hInst = hInstance; // 将实例句柄存储在全局变量中

   /*
   * 创建一个窗口，这里窗口的大小为0，我们会在后面的MoveWindow中改变窗口大小，WS_OVERLAPPEDWINDOW是为我们
   * 写好的一种默认窗口模式
   */
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       0, 0, WLEN, HLEN, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

     MoveWindow(hWnd, ST_x, ST_y, WLEN, HLEN, TRUE);//移动窗口，并且重置窗口大小和位置
     ShowWindow(hWnd, nCmdShow);//控制显式窗口的方式
     UpdateWindow(hWnd);//将WM_PAINT消息发送到hWnd，这里即是运行Game_Paint函数

   if (!Game_Init(hWnd))
   {
       return FALSE;
   }

    

   Clean_UP(hWnd);
   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_OP_B:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_OP_B), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
           
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            Game_PaintBit(hWnd);
            Game_PaintPen(hWnd);
            

            EndPaint(hWnd, &ps);
            ValidateRect(hWnd, NULL);//更新客户区显示
        }
        break;
    case WM_DESTROY:
        Clean_UP(hWnd);
        PostQuitMessage(0);
        break;

    case WM_LBUTTONDOWN://捕获鼠标左键点下的消息
        {
        /*
        * 获取鼠标点击的位置
        */
        last_point.x = LOWORD(lParam);
        last_point.y = HIWORD(lParam);

        //GetCursorPos(&last_point);//函数获得的是相对于屏幕的位置而不是相对于用户区的位置，所以不使用

        if(last_point.y >= 490+25)is_down = TRUE;//判断鼠标是否点下且在区域内
        }
        break;
    case WM_MOUSEMOVE://捕获鼠标移动的消息
        {
        
        if (is_down == TRUE)//如果已经点下
        {   
            srand((unsigned)time(NULL));//随机的种子

            HDC hdc = GetDC(hWnd);//创建一个DC

            /*
            *创建一个画笔，是一个SOLID型的画笔，颜色是随机的
            */
            HPEN pen = CreatePen(PS_SOLID, 1, RGB(rand() * 2 % 256, rand() * 3 % 256, rand()));
            //HPEN pen = CreatePen(PS_DOT, 1, RGB(255, 0, 0));//test

            /*
            * 获取鼠标当前的位置传给point
            */
            point.x = LOWORD(lParam);
            point.y = HIWORD(lParam);
            if (point.y <= 490 + 25)
            {
                break;
            }
            //GetCursorPos(&point);//（同上）函数获得的是相对于屏幕的位置而不是相对于用户区的位置
            SelectObject(hdc, pen);//将当前的画笔给到DC
            MoveToEx(hdc, last_point.x, last_point.y, NULL);//将点移动到上一个位置
            LineTo(hdc, point.x, point.y);//向现在的位置画线
            last_point = point;//将当前位置交给下一个位置
            ReleaseDC(hWnd,hdc);//释放当前DC
        }
        }
        break;
    case WM_LBUTTONUP://捕获左键放松的消息
        /*
        * 重新初始化所有的变量
        */
        is_down = FALSE;
        last_point = { 0 };
        point = { 0 };
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

BOOL Game_Init(HWND hWnd)
{
    hdc = GetDC(hWnd);//给全局变量hdc赋予一个DC，以便我们GamePaint使用。
    hdc1 = GetDC(hWnd);

    bitmap = (HBITMAP)LoadImage(NULL, L"bg.bmp", IMAGE_BITMAP, WLEN, HLEN, LR_LOADFROMFILE);
    hdc1 = CreateCompatibleDC(hdc1);

    Game_PaintBit(hWnd);//我们先Paint背景

    srand((unsigned)time(NULL));
    for (int i = 0; i < 7; i++)
    {
        pen[i] = CreatePen(PenStyle[i], 1, RGB(rand() % 256, rand() % 256, rand() % 256));//CreatePen返回HPEN类型
        if (i > 4)brush[i] = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));//如果大于4用SOLID类型
        else brush[i] = CreateHatchBrush(BrushStyle[i], RGB(rand() % 256, rand() % 256, rand() % 256));
    }
    Game_PaintPen(hWnd);//后Paint我们的内容


    ReleaseDC(hWnd,hdc1);
    ReleaseDC(hWnd,hdc);
    return TRUE;
}

VOID Game_PaintPen(HWND hWnd)
{
    int y = 0;
    for (int i = 0; i < 7; i++)
    {
        y = (i + 1) * 70;
        SelectObject(hdc, pen[i]);//选择画笔
        MoveToEx(hdc, 30, y, NULL);
        LineTo(hdc, 100, y);
    }

    int x1 = 120, x2 = 190;
    for (int i = 0; i < 7; i++)
    {
        SelectObject(hdc, brush[i]);
        Rectangle(hdc, x1, 70, x2, y);
        x1 += 90, x2 += 90;
    }

    SetTextColor(hdc, RGB(255, 0, 0));
    SetBkMode(hdc, TRANSPARENT);
    HFONT font = CreateFont(45, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("微软雅黑"));
    TextOut(hdc, 0, y + 26, L"画图区:", wcslen(L"画图区:"));
    MoveToEx(hdc, 0, y + 25, NULL);
    LineTo(hdc, ST_x+HLEN, y + 25);
}
VOID Game_PaintBit(HWND hWnd)
{
    SelectObject(hdc1, bitmap);
    BitBlt(hdc, 0, 0, WLEN, HLEN, hdc1, 0, 0, SRCCOPY);
}

BOOL Clean_UP(HWND hWnd)
{
    for (int i = 0; i < 7; i++)
    {
        DeleteObject(brush[i]);
        DeleteObject(pen[i]);
    }
    DeleteObject(bitmap);
    return TRUE;
}