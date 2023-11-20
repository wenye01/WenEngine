#pragma once

#include <stdint.h>
#include <set>

#include "D3D12Resource.h"

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
    private:
        void Initialize();
    };


    class D3D12MemoryAllocator
    {
    };
}

