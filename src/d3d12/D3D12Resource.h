#pragma once

#include "D3D12Utils.h"
//#include "D3D12MemoryAllocator.h"


namespace Gloria
{
    class GloriaD3D12BuddyAllocator;

    class GloriaD3D12Resource
    {
    public:
        GloriaD3D12Resource(Microsoft::WRL::ComPtr<ID3D12Resource> InD3DResource,
            D3D12_RESOURCE_STATES InitState = D3D12_RESOURCE_STATE_COMMON);

        void Map();
    public:
        Microsoft::WRL::ComPtr<ID3D12Resource> pD3D12Resource = nullptr;

        D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = 0;

        D3D12_RESOURCE_STATES CurrentState;

        void* MappedBaseAddress = nullptr;
    };

    struct GloriaD3D12BuddyBlockData
    {
        uint32_t offset = 0;
        uint32_t order  = 0;
        uint32_t actualUsedSize = 0;

        GloriaD3D12Resource* PlacedResource = nullptr;
    };
    
    class GloriaD3D12ResourceLocation
    {
    public:
        enum class ResourceLocationType
        {
            Undefined,
            StandAlone,
            SubAllocation,
        };

        GloriaD3D12ResourceLocation();

        ~GloriaD3D12ResourceLocation();

        void ReleaseResource();

        void SetType(ResourceLocationType Type)
        {
            eResourceLocationType = Type;
        }
    public:
        ResourceLocationType eResourceLocationType = ResourceLocationType::Undefined;

        GloriaD3D12BuddyAllocator* Allocator = nullptr;

        GloriaD3D12BuddyBlockData BlockData;

        GloriaD3D12Resource* UnderlyingResource = nullptr;

        union
        {
            uint64_t OffsetFromBaseOfResource;
            uint64_t OffsetFromBaseOfHeap;
        };
        D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = 0;

        void* MappedAddress = nullptr;
    };

    template<typename T>
    class GloriaD3D12ScopeMap
    {
    public:
        GloriaD3D12ScopeMap(GloriaD3D12Resource* resource)
        {
            this->D3DResource = resource->pD3D12Resource.Get();
            this->D3DResource->Map(0, nullptr, reinterpret_cast<void**>(&MappedData));
        }

        ~GloriaD3D12ScopeMap()
        {
            this->D3DResource->Unmap(0, nullptr);
        }

        T* GetMappedData()
        {
            return this->MappedData;
        }
    private:
        ID3D12Resource* D3DResource = nullptr;

        T* MappedData = nullptr;
    };
}
