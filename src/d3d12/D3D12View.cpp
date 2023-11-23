#include "D3D12View.h"
#include "D3D12Device.h"

namespace Gloria
{
    GloriaD3D12View::GloriaD3D12View(GloriaD3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Resource* resource)
        :pDevice(device), Type(type), Resource(resource)
    {
        this->HeapAllocator = this->pDevice->GetHeapAllocator(this->Type);

        if (this->HeapAllocator)
        {
            this->HeapSlot = this->HeapAllocator->AllocateHeapSlot();
        }
    }

    GloriaD3D12View::~GloriaD3D12View()
    {
        this->Destory();
    }

    void GloriaD3D12View::Destory()
    {
        if (this->HeapAllocator)
        {
            this->HeapAllocator->FreeHeapSlot(this->HeapSlot);
        }
    }

    // shader
    GloriaShaderReosurceView::GloriaShaderReosurceView(GloriaD3D12Device* device, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc, ID3D12Resource* resource)
        :GloriaD3D12View(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, resource)
    {
        this->CreateShaderResourceView(desc);
    }

    GloriaShaderReosurceView::~GloriaShaderReosurceView() { }

    void GloriaShaderReosurceView::CreateShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC& desc)
    {
        this->pDevice->GetDevice()->CreateShaderResourceView(this->Resource, &desc, this->HeapSlot.Handle);
    }

    // render target
    GloriaRenderTargetReosurceView::GloriaRenderTargetReosurceView(GloriaD3D12Device* device, const D3D12_RENDER_TARGET_VIEW_DESC& desc, ID3D12Resource* resource)
        :GloriaD3D12View(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, resource)
    {
        this->CreateRenderTargetResourceView(desc);
    }

    GloriaRenderTargetReosurceView::~GloriaRenderTargetReosurceView() { }

    void GloriaRenderTargetReosurceView::CreateRenderTargetResourceView(const D3D12_RENDER_TARGET_VIEW_DESC& desc)
    {
        this->pDevice->GetDevice()->CreateRenderTargetView(this->Resource, &desc, this->HeapSlot.Handle);
    }

    // depth
    GloriaDepthStencilReosurceView::GloriaDepthStencilReosurceView(GloriaD3D12Device* device, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc, ID3D12Resource* resource)
        :GloriaD3D12View(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, resource)
    {
        this->CreateDepthStencilResourceView(desc);
    }

    GloriaDepthStencilReosurceView::~GloriaDepthStencilReosurceView() { }

    void GloriaDepthStencilReosurceView::CreateDepthStencilResourceView(const D3D12_DEPTH_STENCIL_VIEW_DESC& desc)
    {
        this->pDevice->GetDevice()->CreateDepthStencilView(this->Resource, &desc, this->HeapSlot.Handle);
    }

    // unorder
    GloriaUnorderedAccessReosurceView::GloriaUnorderedAccessReosurceView(GloriaD3D12Device* device, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc, ID3D12Resource* resource)
        :GloriaD3D12View(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, resource)
    {
        this->CreateUnorderedAccessResourceView(desc);
    }

    GloriaUnorderedAccessReosurceView::~GloriaUnorderedAccessReosurceView() { }

    void GloriaUnorderedAccessReosurceView::CreateUnorderedAccessResourceView(const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc)
    {
        this->pDevice->GetDevice()->CreateUnorderedAccessView(this->Resource, nullptr, &desc, this->HeapSlot.Handle);
    }
}