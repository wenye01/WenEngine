#pragma once

#include <vector>
#include <d3d12.h>
#include <wrl/client.h>

#include "../error.hpp"

namespace Gloria
{
    using namespace Microsoft::WRL;
    class RootSignatures
    {
    public:
    private:
    };

    class SingleRootSignature
    {
    public:
        SingleRootSignature() { }

        void Init(ID3D12Device4* pDevice, D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        void AddDesriptorRange(
            D3D12_DESCRIPTOR_RANGE_TYPE rangetype,
            UINT numdescriptors = 1,
            UINT baseshaderregister = 0,
            UINT registerspace = 0,
            D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
            UINT offsetindescriptorfromtablestart = 0);

        void AddParameter(
            D3D12_SHADER_VISIBILITY shadervisibility,
            D3D12_ROOT_PARAMETER_TYPE parametertype = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            UINT numdescriptorranges = 1);
            
        ID3D12RootSignature* Get()
        {
            return this->pRootSignature.Get();
        }
    private:
        ComPtr<ID3D12RootSignature> pRootSignature;

        std::vector<D3D12_DESCRIPTOR_RANGE1> DescriptorRangeVector;
        std::vector< D3D12_ROOT_PARAMETER1>  ParameterVector;
    };
}
