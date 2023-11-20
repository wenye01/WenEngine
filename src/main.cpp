#include <windows.h>
#include "Engine/Engine.h"


#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define WND_CLASS_NAME  _T("Window Class")
#define WND_TITLE	    _T("DirectX12 ")

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    try
    {
        Gloria::GloriaEngine GEngine(hInstance);

        GEngine.Initialize();

        GEngine.Run();

        GEngine.Detory();

        return 0;
    }

    catch ()
    {
        
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, message, wParam, lParam); // 默认回调函数
}