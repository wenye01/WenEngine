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
}