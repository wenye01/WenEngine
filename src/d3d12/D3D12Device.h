#pragma once

#include "D3D12MemoryAllocator.h"
#include "D3D12HeapAllocator.h"

namespace Gloria
{
    class GloriaD3D12Device
    {
    public:
        GloriaD3D12Device();

        ~GloriaD3D12Device();

    public:
        ID3D12Device* GetDevice() { return this->pDevice.Get(); }

        GloriaD3D12UploadBufferAllocator* GetUploadBufferAllocator() { return this->UploadBufferAllocator.get(); }

        GloriaD3D12DefaultBufferAllocator* GetDefaultBufferAllocator() { return this->DefaultBufferAllocator.get(); }

        GloriaD3D12TextureResourceAllocator* GetTextureBufferAllocator() { return this->TextureBufferAllocator.get(); }

        GloriaD3D12HeapAllocator* GetHeapAllocator(D3D12_DESCRIPTOR_HEAP_TYPE heaptype);
    private:
        void Initialize();

    private:
        Microsoft::WRL::ComPtr<ID3D12Device> pDevice = nullptr;

        std::unique_ptr<GloriaD3D12UploadBufferAllocator> UploadBufferAllocator = nullptr;
        std::unique_ptr<GloriaD3D12DefaultBufferAllocator> DefaultBufferAllocator = nullptr;
        std::unique_ptr<GloriaD3D12TextureResourceAllocator> TextureBufferAllocator = nullptr;

        std::unique_ptr<GloriaD3D12HeapAllocator> RTVHeapAllocator = nullptr;
        std::unique_ptr<GloriaD3D12HeapAllocator> DSVHeapAllocator = nullptr;
        std::unique_ptr<GloriaD3D12HeapAllocator> SRVHeapAllocator = nullptr;
    };
}
