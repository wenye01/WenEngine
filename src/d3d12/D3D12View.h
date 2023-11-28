#pragma once

#include "D3D12HeapAllocator.h"

namespace Gloria
{
    class GloriaD3D12Device;

    class GloriaD3D12View
    {
    public:
        GloriaD3D12View(GloriaD3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Resource* resource);

        ~GloriaD3D12View();

        D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const { return this->HeapSlot.Handle; }

    private:
        void Destory();

    protected:
        GloriaD3D12Device* pDevice = nullptr;
        
        GloriaD3D12HeapAllocator* HeapAllocator = nullptr;

        ID3D12Resource* Resource = nullptr;

        GloriaD3D12HeapAllocator::HeapSlot HeapSlot;

        D3D12_DESCRIPTOR_HEAP_TYPE Type;
    };
    //
    class GloriaShaderReosurceView :public GloriaD3D12View
    {
    public:
        GloriaShaderReosurceView(GloriaD3D12Device* device, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc, ID3D12Resource* resource);

        virtual ~GloriaShaderReosurceView();
    private:
        void CreateShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);
    };
    //
    class GloriaRenderTargetView :public GloriaD3D12View
    {
    public:
        GloriaRenderTargetView(GloriaD3D12Device* device, const D3D12_RENDER_TARGET_VIEW_DESC& desc, ID3D12Resource* resource);

        virtual ~GloriaRenderTargetView();
    private:
        void CreateRenderTargetResourceView(const D3D12_RENDER_TARGET_VIEW_DESC& desc);
    };
    //
    class GloriaDepthStencilView :public GloriaD3D12View
    {
    public:
        GloriaDepthStencilView(GloriaD3D12Device* device, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc, ID3D12Resource* resource);

        virtual ~GloriaDepthStencilView();
    private:
        void CreateDepthStencilResourceView(const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);
    };
    //
    class GloriaUnorderedAccessView :public GloriaD3D12View
    {
    public:
        GloriaUnorderedAccessView(GloriaD3D12Device* device, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc, ID3D12Resource* resource);

        virtual ~GloriaUnorderedAccessView();
    private:
        void CreateUnorderedAccessResourceView(const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);
    };
}
