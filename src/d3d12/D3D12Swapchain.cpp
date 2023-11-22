#include "D3D12Swapchain.h"
#include "D3D12Common.h"

namespace Gloria
{
    GloriaD3D12Swapchain::GloriaD3D12Swapchain()
    {
        this->Initialize();
    }

    GloriaD3D12Swapchain::~GloriaD3D12Swapchain() { }

    void GloriaD3D12Swapchain::Initialize()
    {
        this->SwapChain.Reset();

        DXGI_SWAP_CHAIN_DESC desc;
        {
            desc.BufferDesc.Width = ViewportWidth;
            desc.BufferDesc.Height = ViewportHeight;
            desc.BufferDesc.RefreshRate.Numerator = 60;
            desc.BufferDesc.RefreshRate.Denominator = 1;
            desc.BufferDesc.Format = SwapchainInfoData.BackBufferFormat;
            desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
            desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
            desc.SampleDesc.Count = SwapchainInfoData.Enalble4xMSAA ? 4 : 1;
            desc.SampleDesc.Quality = SwapchainInfoData.Enalble4xMSAA ? (SwapchainInfoData.QualityOf4xMSAA - 1) : 0;
            desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            desc.BufferCount = SwapchainBufferCount;
            desc.OutputWindow = SwapchainInfoData.hwnd;
            desc.Windowed = true;
            desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        }

        Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue = this->pCommon->GetDevice();
    }
}