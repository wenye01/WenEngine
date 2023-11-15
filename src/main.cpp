#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN // 从 Windows 头中排除极少使用的资料
#include <windows.h>
#include <tchar.h>
#include <fstream>			//for ifstream
#include <wrl.h>			//添加WTL支持 方便使用COM
#include <atlcoll.h>		//for atl array
#include <d3d12.h>			//for d3d12
#include <d3dcompiler.h>

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif
#include <DirectXMath.h>

#include "./WindowsCommons/DDSTextureLoader12.h"
#include "common/structures.h"
#include "error.hpp"
#include "Engine.h"

// using namespace std; byte与c++17中的std::byte冲突
using namespace Microsoft;
using namespace Microsoft::WRL;
using namespace DirectX;

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define GRS_WND_CLASS_NAME  _T("GRS Game Window Class")
#define GRS_WND_TITLE	    _T("GRS DirectX12 Render To Texture Sample")

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    ::CoInitialize(nullptr); // COM Init

    Gloria::wndStruct window;
    {
        window.wndWidth  = 1024;
        window.wndHeight = 768;
        window.hwnd = nullptr;
    }

    try
    {
        // 窗口
        {
            WNDCLASSEX wcex = { };
            {
                wcex.cbSize         = sizeof(WNDCLASSEX); // 固定参数
                wcex.style          = CS_GLOBALCLASS; // https://learn.microsoft.com/zh-cn/windows/win32/winmsg/window-class-styles
                wcex.lpfnWndProc    = WndProc; // 回调函数，不可为nullptr，函数也不能直接返回0，使用默认函数
                wcex.cbClsExtra     = 0;
                wcex.cbWndExtra     = 0;
                wcex.hInstance      = hInstance;
                wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW); // 鼠标句柄
                wcex.hbrBackground  = (HBRUSH)GetStockObject(NULL_BRUSH);
                wcex.lpszClassName = GRS_WND_CLASS_NAME;
            }
            RegisterClassEx(&wcex);

            DWORD dwWndStyle = WS_OVERLAPPED | WS_SYSMENU;
            RECT rtWnd = { 0, 0, window.wndWidth, window.wndHeight };
            AdjustWindowRect(&rtWnd, dwWndStyle, FALSE);

            // 计算窗口居中的屏幕坐标
            INT posX = (GetSystemMetrics(SM_CXSCREEN) - rtWnd.right - rtWnd.left) / 2;
            INT posY = (GetSystemMetrics(SM_CYSCREEN) - rtWnd.bottom - rtWnd.top) / 2;

            window.hwnd = CreateWindowW(GRS_WND_CLASS_NAME, GRS_WND_TITLE, dwWndStyle
                , posX, posY, rtWnd.right - rtWnd.left, rtWnd.bottom - rtWnd.top
                , nullptr, nullptr, hInstance, nullptr);
            auto er = GetLastError();
            if (!window.hwnd)
            {
                throw CGRSCOMException(HRESULT_FROM_WIN32(GetLastError()));
            }
        }
        
        ShowWindow(window.hwnd, nCmdShow);
        UpdateWindow(window.hwnd);

        while (true)
        {

        }

        ::CoUninitialize();
        return 0;
    }

    catch (CGRSCOMException& e)
    {
        e;
    }

    ::CoUninitialize();
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, message, wParam, lParam); // 默认回调函数
}