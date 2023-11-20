#pragma once

#include "D3D12Utils.h"

namespace Gloria
{
    class GloriaD3D12Resource
    {
    public:
        GloriaD3D12Resource(Microsoft::WRL::ComPtr<ID3D12Resource> InD3DResource,
            D3D12_RESOURCE_STATES InitState = D3D12_RESOURCE_STATE_COMMON);

        void Map();
    private:
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

        void SetType(ResourceLocationType type);
    private:
        ResourceLocationType eResourceLocationType = ResourceLocationType::Undefined;


    };
}
