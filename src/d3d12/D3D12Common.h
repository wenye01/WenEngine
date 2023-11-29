#pragma once

#include "D3D12Device.h"
#include "D3D12MemoryAllocator.h"
#include "D3D12Swapchain.h"
#include "D3D12Buffer.h"
#include "D3D12Texture.h"

namespace Gloria
{
    // 外部调用
    class D3D12Common
    {
    public:
        D3D12Common();

        ~D3D12Common();

        void Initialize(HWND hwnd, int wndWidth, int wndHeight);

        void Destory();

    public:
        void FlushCommandQueue();

        void ExecuteCommandList();

        void ResetCommandList();

        void ResetCommandAlocator();

        void Present();

        void ResizeSwapChain();

        void TransitionResource();

        void CopyResource();

        void CopyBufferRegion();

        void CopyTextureRegion();

        GloriaConstantBufferRef CreateConstantnBuffer();

        GloriaStructedBufferRef CreateStructedBuffer();

        GloriaRWStructedResourceviewRef CreteRWStructedBuffer();

        GloriaVertexBufferRef CreateVertexBuffer();

        GloriaIndexBufferRef CreateIndexBuffer();

        GloriaReadBackBufferRef CreateReadBackBuffer();

        GloriaD3DTextureRef CreateTexture();

        void UploadTextureData();

        void SetVertexBuffer();

        void SetIndexBuffer();

        void EndFrame();

    public:
        GloriaD3D12Device* GetDevice()
        {
            return this->pDevice.get();
        }

        GloriaD3D12Swapchain* GetSwapChain()
        {
            return this->pSwapChain.get();
        }

        const GloriaD3D12SwapchainInfoData& GetSwapChainInfo()
        {

        }

        IDXGIFactory4* GetDxgiFactory()
        {

        }
    private:
        void LogAdapters();

        UINT GetSupportMSAAQuality(DXGI_FORMAT BackBufferFormat);

    private:
        std::unique_ptr<GloriaD3D12Device> pDevice = nullptr;

        std::unique_ptr<GloriaD3D12Swapchain> pSwapChain = nullptr;

        GloriaD3D12SwapchainInfoData SwapChainInfo;

        Microsoft::WRL::ComPtr<IDXGIFactory> DxgiFactory = nullptr;
    };
}

