#pragma once

#include "D3D12Utils.h"

namespace Gloria
{
    class D3D12Common;

    struct GloriaD3D12SwapchainInfoData
    {
        HWND hwnd;

        DXGI_FORMAT BackBufferFormat;
        DXGI_FORMAT DepthStencilFormat;

        bool Enalble4xMSAA = false;
        UINT QualityOf4xMSAA = 0;
    };

    class GloriaD3D12Swapchain
    {
    public:
        GloriaD3D12Swapchain();

        ~GloriaD3D12Swapchain();

    private:
        void Initialize();
    private:
        D3D12Common* pCommon;

        Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain = nullptr;
        int ViewportWidth = 0;
        int ViewportHeight = 0;

        static const int SwapchainBufferCount = 2;

        GloriaD3D12SwapchainInfoData SwapchainInfoData;

    };
}
