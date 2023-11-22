#pragma once

#include <stdint.h>
#include <set>

#include "D3D12Resource.h"

#define DEFAULT_POOL_SIZE (512 * 1024 * 512)

#define DEFAULT_RESOURCE_ALIGNMENT 4
#define UPLOAD_RESOURCE_ALIGNMENT 256

namespace Gloria
{
    class GloriaD3D12BuddyAllocator
    {
    public:
        enum class AllocationStrategy
        {
            PlacedResource,
            ManualSubAllocation
        };

        struct AllocatorInitData
        {
            AllocationStrategy eAllocationStrategy;
            D3D12_HEAP_TYPE HeapType;
            D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAG_NONE;
            D3D12_RESOURCE_FLAGS ResourceFlags = D3D12_RESOURCE_FLAG_NONE;
        };
    public:
        GloriaD3D12BuddyAllocator(ID3D12Device* device, const AllocatorInitData& initDate);

        ~GloriaD3D12BuddyAllocator();

        bool AllocateResource(uint32_t size,uint32_t aligment,GloriaD3D12ResourceLocation& resourceLocation);

        void Deallocate(GloriaD3D12ResourceLocation& resourceLocation);

        void CleanUpAllocations();

    public:
        ID3D12Heap* GetBackHeap()
        {
            return this->pBackHeap;
        }

        AllocationStrategy GetAllocationStrategy()
        {
            return this->InitData.eAllocationStrategy;
        }

    private:
        void Initialize();

        uint32_t GetSizeToAllocate(uint32_t size, uint32_t aligment);

        bool CanAllocate(uint32_t size2allocate);

        uint32_t AllocateBlock(uint32_t order);

        void DeallocateInternal(const GloriaD3D12BuddyBlockData& Block);

        void DeallocateBlock(uint32_t Offset, uint32_t Order);

    private:
        AllocatorInitData InitData;

        const uint32_t MinBlockSize = 256;
        uint32_t MaxOrder;
        uint32_t AllocatedSize = 0;

        std::vector<std::set<uint32_t>> FreeBlocks;
        std::vector<GloriaD3D12BuddyBlockData> DeferredDelectionQueue;

        ID3D12Device* pDevice;
        GloriaD3D12Resource* pBackResource = nullptr;
        ID3D12Heap* pBackHeap = nullptr;

    private:
        uint32_t Size2UnitSize(uint32_t size) const
        {// ÏòÉÏ¶ÔÆë
            return (uint32_t)(size + this->MinBlockSize - 1) / this->MinBlockSize;
        }

        uint32_t Order2UnitSize(uint32_t order) const
        {
            return ((uint32_t)1) << order;
        }

        uint32_t UnitSize2Order(uint32_t size) const
        {
            //unsigned long result;
            //_BitScanReverse(&result, size + size - 1);
            //return result;
            return (uint32_t)(ceil(log2(size)));
        }

        uint32_t GetBuddyOffset(const uint32_t& offset, const uint32_t& size)
        {
            return offset ^ size;
        }

        uint32_t GetAllocateOffsetInByte(uint32_t offset) const
        {
            return offset * this->MinBlockSize;
        }
    };

    class GloriaD3D12MultiBuddyAllocator
    {
    public:
        GloriaD3D12MultiBuddyAllocator(ID3D12Device* device, const GloriaD3D12BuddyAllocator::AllocatorInitData initdata);

        ~GloriaD3D12MultiBuddyAllocator();

        bool AllocateResource(uint32_t size, uint32_t aligment, GloriaD3D12ResourceLocation& resourceLocation);

        void CleanUpAllocations();
    private:
        std::vector<std::shared_ptr<GloriaD3D12BuddyAllocator>> Allocators;

        ID3D12Device* pDevice;

        GloriaD3D12BuddyAllocator::AllocatorInitData InitData;
    };

    class GloriaD3D12UploadBufferAllocator
    {
    public:
        GloriaD3D12UploadBufferAllocator(ID3D12Device* InDevice);

        void* AllocUploadResource(uint32_t Size, uint32_t Alignment, GloriaD3D12ResourceLocation& ResourceLocation);

        void CleanUpAllocations();

    private:
        std::unique_ptr<GloriaD3D12MultiBuddyAllocator> Allocator = nullptr;

        ID3D12Device* D3DDevice = nullptr;
    };

    class GloriaD3D12DefaultBufferAllocator
    {
    public:
        GloriaD3D12DefaultBufferAllocator(ID3D12Device* InDevice);

        void AllocDefaultResource(const D3D12_RESOURCE_DESC& ResourceDesc, uint32_t Alignment, GloriaD3D12ResourceLocation& ResourceLocation);

        void CleanUpAllocations();

    private:
        std::unique_ptr<GloriaD3D12MultiBuddyAllocator> Allocator = nullptr;

        std::unique_ptr<GloriaD3D12MultiBuddyAllocator> UavAllocator = nullptr;

        ID3D12Device* D3DDevice = nullptr;
    };

    class GloriaD3D12TextureResourceAllocator
    {
    public:
        GloriaD3D12TextureResourceAllocator(ID3D12Device* InDevice);

        void AllocTextureResource(const D3D12_RESOURCE_STATES& ResourceState, const D3D12_RESOURCE_DESC& ResourceDesc, GloriaD3D12ResourceLocation& ResourceLocation);

        void CleanUpAllocations();

    private:
        std::unique_ptr<GloriaD3D12MultiBuddyAllocator> Allocator = nullptr;

        ID3D12Device* D3DDevice = nullptr;
    };
}

