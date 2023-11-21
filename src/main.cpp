#include <windows.h>
#include "Engine/Engine.h"


#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

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