#include "D3D12Resource.h"

namespace Gloria
{
    using namespace Microsoft::WRL;

    GloriaD3D12Resource::GloriaD3D12Resource(
        Microsoft::WRL::ComPtr<ID3D12Resource> inD3D12Resource,
        D3D12_RESOURCE_STATES InitState = D3D12_RESOURCE_STATE_COMMON)
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


}                  