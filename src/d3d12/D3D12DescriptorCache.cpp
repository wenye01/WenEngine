#include "D3D12DescriptorCache.h"
#include "D3D12Device.h"

namespace Gloria 
{
    GloriaD3D12DescriptorCache::GloriaD3D12DescriptorCache(GloriaD3D12Device* device)
        :pDevice(device)
    {
        this->CreatCacheViewDescriptorHeap();

        this->CreatCacheRTVDescriptor();
    }

    GloriaD3D12DescriptorCache::~GloriaD3D12DescriptorCache() { }

    void GloriaD3D12DescriptorCache::CreatCacheViewDescriptorHeap()
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        {
            desc.NumDescriptors = this->MaxViewDescripotrCount;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        }

        ThrowIfFailed(this->pDevice->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&this->CacheViewDescriptorHeap)));
        SetDebugName(this->CacheViewDescriptorHeap.Get(), L"CacheViewDescriptorHeap");

        this->ViewDescriptorSize = this->pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE GloriaD3D12DescriptorCache::AppendViewDescriptor(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& srcDescriptor)
    {
        uint32_t SlotSize = (uint32_t)srcDescriptor.size();

        auto CpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), ViewDescriptorOffset, ViewDescriptorSize);
        this->pDevice->GetDevice()->CopyDescriptors(1, &CpuDescriptorHandle, &SlotSize, SlotSize, srcDescriptor.data(), nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        auto GpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CacheViewDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), ViewDescriptorOffset, ViewDescriptorSize);

        this->ViewDescriptorOffset += SlotSize;

        return GpuDescriptorHandle;
    }

    void GloriaD3D12DescriptorCache::ResetCacheViewDescriptorHeap()
    {
        ViewDescriptorOffset = 0;
    }

    void GloriaD3D12DescriptorCache::CreatCacheRTVDescriptor()
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        {
            desc.NumDescriptors = this->MaxRTVDescriptor;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        }

        ThrowIfFailed(this->pDevice->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&this->CacheRTVDescriptorHeap)));
        SetDebugName(this->CacheRTVDescriptorHeap.Get(), L"CacheRTVDescriptorHeap");

        this->RTVDescriptorSize = this->pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    }

    void GloriaD3D12DescriptorCache::AppendRTVDescriptor(
        const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& RtvDescriptors,
        CD3DX12_GPU_DESCRIPTOR_HANDLE& OutGpuHandle,
        CD3DX12_CPU_DESCRIPTOR_HANDLE& OutCpuHandle)
    {
        uint32_t SlotSize = (uint32_t)RtvDescriptors.size();

        auto CpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), RTVDescriptorOffset, RTVDescriptorSize);
        this->pDevice->GetDevice()->CopyDescriptors(1, &CpuDescriptorHandle, &SlotSize, SlotSize, RtvDescriptors.data(), nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


        OutGpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CacheRTVDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), RTVDescriptorOffset, RTVDescriptorSize);

        OutCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), RTVDescriptorOffset, RTVDescriptorSize);

        this->ViewDescriptorOffset += SlotSize;
    }

    void GloriaD3D12DescriptorCache::ResetCacheRTVDescriptor()
    {
        this->RTVDescriptorOffset = 0;
    }

    void GloriaD3D12DescriptorCache::Reset()
    {
        this->ResetCacheViewDescriptorHeap();

        this->ResetCacheRTVDescriptor();
    }
}