#pragma once

#include "D3D12Utils.h"
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

        void ResizeSwapChain(int width, int height);

        void TransitionResource(GloriaD3D12Resource*resource,D3D12_RESOURCE_STATES state);

        void CopyResource(GloriaD3D12Resource* DstResource,GloriaD3D12Resource* SrcResource);

        void CopyBufferRegion(GloriaD3D12Resource* DstResource,UINT64 DstOffset,GloriaD3D12Resource* SrcResource,UINT64 srcOffset,UINT64 Size);

        void CopyTextureRegion(D3D12_TEXTURE_COPY_LOCATION* Dst, UINT DstX,UINT DstY,UINT DstZ,const D3D12_TEXTURE_COPY_LOCATION *Src,const D3D12_BOX* SrcBox);

        GloriaConstantBufferRef CreateConstantnBuffer(const void* Contents,uint32_t Size);

        GloriaStructedBufferRef CreateStructedBuffer(const void* Contents, uint32_t ElementSize, uint32_t ElementCount);

        GloriaRWStructedResourceviewRef CreteRWStructedBuffer(uint32_t ElementSize, uint32_t ElementCount);

        GloriaVertexBufferRef CreateVertexBuffer(const void* Contents, uint32_t Size);

        GloriaIndexBufferRef CreateIndexBuffer(const void* Contents, uint32_t Size);

        GloriaReadBackBufferRef CreateReadBackBuffer(uint32_t Size);

        GloriaD3D12TextureRef CreateTexture(const GloriaTextureInfo& textureInfo, uint32_t createFlag, XMFLOAT4 rtvClratValue = XMFLOAT4(0.f, 0.f, 0.f, 0.f));

        GloriaD3D12TextureRef CreateTexture(Microsoft::WRL::ComPtr<ID3D12Resource> resource, GloriaTextureInfo info, uint32_t createFlag);

        void UploadTextureData(GloriaD3D12TextureRef texture, const std::vector<D3D12_SUBRESOURCE_DATA>& initData);

        void SetVertexBuffer(const GloriaVertexBufferRef& vertexBuffer, UINT offset, UINT stride, UINT size);

        void SetIndexBuffer(const GloriaIndexBufferRef& indexBuffer, UINT offset, DXGI_FORMAT format, UINT size);

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
            return this->SwapChainInfo;
        }

        IDXGIFactory4* GetDxgiFactory()
        {
            return this->DxgiFactory.Get();
        }
    private:
        void LogAdapters(); //TODO

        UINT GetSupportMSAAQuality(DXGI_FORMAT BackBufferFormat);

        void CreateDefaultBuffer(uint32_t Size, uint32_t Alignment, D3D12_RESOURCE_FLAGS Flags, GloriaD3D12ResourceLocation ResourceLocation);

        void CreateAndInitDefaultBuffer(const void* Contents, uint32_t Size, uint32_t Aligment, GloriaD3D12ResourceLocation& ResourceLocation);

        GloriaD3D12TextureRef CreateTextureResource(const GloriaTextureInfo& TextureInfo, uint32_t CreateFlags, XMFLOAT4 RTVClearValue);

        void CreateTextureViews(GloriaD3D12TextureRef TextureRef, const GloriaTextureInfo& TextureInfo, uint32_t CreateFlags);
    private:
        std::unique_ptr<GloriaD3D12Device> pDevice = nullptr;

        std::unique_ptr<GloriaD3D12Swapchain> pSwapChain = nullptr;

        GloriaD3D12SwapchainInfoData SwapChainInfo;

        Microsoft::WRL::ComPtr<IDXGIFactory4> DxgiFactory = nullptr;
    };
}

