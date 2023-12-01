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
        this->pDevice->GetD3DDevice()->CreateShaderResourceView(this->Resource, &desc, this->HeapSlot.Handle);
    }

    // render target
    GloriaRenderTargetView::GloriaRenderTargetView(GloriaD3D12Device* device, const D3D12_RENDER_TARGET_VIEW_DESC& desc, ID3D12Resource* resource)
        :GloriaD3D12View(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, resource)
    {
        this->CreateRenderTargetResourceView(desc);
    }

    GloriaRenderTargetView::~GloriaRenderTargetView() { }

    void GloriaRenderTargetView::CreateRenderTargetResourceView(const D3D12_RENDER_TARGET_VIEW_DESC& desc)
    {
        this->pDevice->GetD3DDevice()->CreateRenderTargetView(this->Resource, &desc, this->HeapSlot.Handle);
    }

    // depth
    GloriaDepthStencilView::GloriaDepthStencilView(GloriaD3D12Device* device, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc, ID3D12Resource* resource)
        :GloriaD3D12View(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, resource)
    {
        this->CreateDepthStencilResourceView(desc);
    }

    GloriaDepthStencilView::~GloriaDepthStencilView() { }

    void GloriaDepthStencilView::CreateDepthStencilResourceView(const D3D12_DEPTH_STENCIL_VIEW_DESC& desc)
    {
        this->pDevice->GetD3DDevice()->CreateDepthStencilView(this->Resource, &desc, this->HeapSlot.Handle);
    }

    // unorder
    GloriaUnorderedAccessView::GloriaUnorderedAccessView(GloriaD3D12Device* device, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc, ID3D12Resource* resource)
        :GloriaD3D12View(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, resource)
    {
        this->CreateUnorderedAccessResourceView(desc);
    }

    GloriaUnorderedAccessView::~GloriaUnorderedAccessView() { }

    void GloriaUnorderedAccessView::CreateUnorderedAccessResourceView(const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc)
    {
        this->pDevice->GetD3DDevice()->CreateUnorderedAccessView(this->Resource, nullptr, &desc, this->HeapSlot.Handle);
    }
}