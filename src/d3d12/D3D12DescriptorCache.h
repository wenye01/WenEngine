#pragma once

#include "D3D12Utils.h"

namespace Gloria
{
    class GloriaD3D12Device;

    class GloriaD3D12DescriptorCache
    {
    public:
        GloriaD3D12DescriptorCache(GloriaD3D12Device* device);

        ~GloriaD3D12DescriptorCache();

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetCacheViewDescriptor() { return this->CacheViewDescriptorHeap; }

        CD3DX12_GPU_DESCRIPTOR_HANDLE AppendViewDescriptor(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& srcDescriptor);
        
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetCacheRTVDescriptorHeap() { return this->CacheRTVDescriptorHeap; }

        void AppendRTVDescriptor(
            const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& RtvDescriptors,
            CD3DX12_GPU_DESCRIPTOR_HANDLE& OutGpuHandle,
            CD3DX12_CPU_DESCRIPTOR_HANDLE& OutCpuHandle);

        void Reset();
    private:
        void CreatCacheViewDescriptorHeap();

        void CreatCacheRTVDescriptor();
        
        void ResetCacheViewDescriptorHeap();

        void ResetCacheRTVDescriptor();
    private:
        GloriaD3D12Device* pDevice = nullptr;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CacheViewDescriptorHeap = nullptr;

        UINT ViewDescriptorSize;

        static const int MaxViewDescripotrCount = 2048;

        uint32_t ViewDescriptorOffset = 0;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CacheRTVDescriptorHeap = nullptr;
        
        UINT RTVDescriptorSize;

        static const int MaxRTVDescriptor = 1024;

        uint32_t RTVDescriptorOffset = 0;
    };
}

