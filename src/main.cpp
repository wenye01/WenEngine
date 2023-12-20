#include <windows.h>
#include "Engine/Engine.h"


#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    try
    {
        Gloria::GloriaEngine GEngine(hInstance);

        GEngine.Initialize();

        GEngine.Run();

        GEngine.Detory();

        return 0;
    }

    catch (GloriaException& e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"ERROR", MB_OK);
        return 0;
    }
}