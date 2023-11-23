#include "D3D12Device.h"

namespace Gloria
{
    GloriaD3D12Device::GloriaD3D12Device()
    {
        this->Initialize();
    }

    GloriaD3D12Device::~GloriaD3D12Device() { }

    void GloriaD3D12Device::Initialize()
    {
        // device
        D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(&this->pDevice));

        this->UploadBufferAllocator = std::make_unique<GloriaD3D12UploadBufferAllocator>(this->pDevice.Get());
        this->DefaultBufferAllocator = std::make_unique<GloriaD3D12DefaultBufferAllocator>(this->pDevice.Get());
        this->TextureBufferAllocator = std::make_unique<GloriaD3D12TextureResourceAllocator>(this->pDevice.Get());


    }

    GloriaD3D12HeapAllocator* GloriaD3D12Device::GetHeapAllocator(D3D12_DESCRIPTOR_HEAP_TYPE heaptype)
    {
        switch (heaptype)
        {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
            return this->SRVHeapAllocator.get();
            break;
        //case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
            //break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
            return this->RTVHeapAllocator.get();
            break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
            return this->DSVHeapAllocator.get();
            break;
        //case D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES:
            //break;
        default:
            return nullptr;
        }
    }
}