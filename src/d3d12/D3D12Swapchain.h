#pragma once

#include "D3D12Utils.h"
#include "D3D12Texture.h"

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
        GloriaD3D12Swapchain(D3D12Common* pCommon);

        ~GloriaD3D12Swapchain();

    public:
        void OnResize(int width, int height);

        void GetViewport(D3D12_VIEWPORT& OutViewPort, D3D12_RECT& OutRect);

        void Present();

        GloriaD3D12Resource* GetCurrentBackBuffer() const
        {
            return this->RenderTargetTextures[this->CurentBackBuffer]->GetResource();
        }

        GloriaRenderTargetView* GetRenderTargerView() const
        {
            return this->RenderTargetTextures[this->CurentBackBuffer]->GetRTV();
        }

        float* GetCurrentBackBufferClearColor() const
        {
            return this->RenderTargetTextures[this->CurentBackBuffer]->GetRTVClearValuePtr();
        }

        GloriaDepthStencilView* GetDepthStencilView() const
        {
            return this->RenderTargetTextures[this->CurentBackBuffer]->GetDSV();
        }

        GloriaShaderReosurceView* GetShaderResourceView() const
        {
            return this->RenderTargetTextures[this->CurentBackBuffer]->GetSRV();
        }

        GloriaD3D12SwapchainInfoData GetSwapchainInfo() const
        {
            return this->SwapchainInfoData;
        }

    private:
        void Initialize();

    private:
        D3D12Common* pCommon;

        Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain = nullptr;
        int ViewportWidth = 0;
        int ViewportHeight = 0;

        static const int SwapchainBufferCount = 2;
        int CurentBackBuffer = 0;

        GloriaD3D12SwapchainInfoData SwapchainInfoData;

        GloriaD3D12TextureRef RenderTargetTextures[SwapchainBufferCount];
        GloriaD3D12TextureRef DepthStencilTexture = nullptr;
    };
}
