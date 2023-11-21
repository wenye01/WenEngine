#include "D3D12Resource.h"
#include "D3D12MemoryAllocator.h"

namespace Gloria
{
    using namespace Microsoft::WRL;

    GloriaD3D12Resource::GloriaD3D12Resource(
        Microsoft::WRL::ComPtr<ID3D12Resource> inD3D12Resource,
        D3D12_RESOURCE_STATES InitState )
        :pD3D12Resource(inD3D12Resource), CurrentState(InitState)
    {
        if (this->pD3D12Resource->GetDesc().Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
        {
            this->GPUVirtualAddress = this->pD3D12Resource->GetGPUVirtualAddress();
        }
    }

    void GloriaD3D12Resource::Map()
    {
        ThrowIfFailed(this->pD3D12Resource->Map(0, nullptr, &this->MappedBaseAddress));
    }

    GloriaD3D12ResourceLocation::GloriaD3D12ResourceLocation()
    {

    }

    GloriaD3D12ResourceLocation::~GloriaD3D12ResourceLocation()
    {
        ReleaseResource();
    }

    void GloriaD3D12ResourceLocation::ReleaseResource()
    {
        switch (eResourceLocationType)
        {
        case GloriaD3D12ResourceLocation::ResourceLocationType::StandAlone:
        {
            delete UnderlyingResource;

            break;
        }
        case GloriaD3D12ResourceLocation::ResourceLocationType::SubAllocation:
        {
            if (this->Allocator)
            {
                this->Allocator->Deallocate(*this);
            }

            break;
        }

        default:
            break;
        }
    }
}                  