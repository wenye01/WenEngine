#include "RootSignatures.h"


namespace Gloria
{
    void SingleRootSignature::Init(
        ID3D12Device4* pDevice, 
        D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)
    {
        D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDesc = { };
        {
            RootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1; // 最新版本
            RootSignatureDesc.Desc_1_1.Flags = flags;
            RootSignatureDesc.Desc_1_1.NumParameters = static_cast<UINT>(this->ParameterVector.size());
            RootSignatureDesc.Desc_1_1.pParameters = this->ParameterVector.data();
            RootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
            RootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;
        }

        ComPtr<ID3DBlob> pISignatureBlob;
        ComPtr<ID3DBlob> pIErrorBlob;

        GRS_THROW_IF_FAILED(D3D12SerializeVersionedRootSignature(
            &RootSignatureDesc,
            &pISignatureBlob,
            &pIErrorBlob));
        GRS_THROW_IF_FAILED(pDevice->CreateRootSignature(
            0,
            pISignatureBlob->GetBufferPointer(),
            pISignatureBlob->GetBufferSize(),
            IID_PPV_ARGS(&this->pRootSignature)));

        GRS_SET_D3D12_DEBUGNAME_COMPTR(this->pRootSignature);
    }

    void SingleRootSignature::AddDesriptorRange(
        D3D12_DESCRIPTOR_RANGE_TYPE rangetype,
        UINT numdescriptors = 1,
        UINT baseshaderregister = 0,
        UINT registerspace = 0,
        D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
        UINT offsetindescriptorfromtablestart = 0)
    {
        if (this->DescriptorRangeVector.size() != this->ParameterVector.size())
        {
            throw "D3D12_DESCRIPTOR_RANGE1与D3D12_ROOT_PARAMETER1不对应";
        }

        D3D12_DESCRIPTOR_RANGE1 tmpDescriptorRange = { };
        {
            tmpDescriptorRange.RangeType = rangetype;
            tmpDescriptorRange.NumDescriptors = numdescriptors;
            tmpDescriptorRange.BaseShaderRegister = baseshaderregister;
            tmpDescriptorRange.RegisterSpace = registerspace;
            tmpDescriptorRange.Flags = flags;
            tmpDescriptorRange.OffsetInDescriptorsFromTableStart = offsetindescriptorfromtablestart;
        }
        this->DescriptorRangeVector.push_back(tmpDescriptorRange);
    }

    void SingleRootSignature::AddParameter(
        D3D12_SHADER_VISIBILITY shadervisibility,
        D3D12_ROOT_PARAMETER_TYPE parametertype = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        UINT numdescriptorranges = 1)
    {
        if (1 != (this->DescriptorRangeVector.size() - this->ParameterVector.size()))
        {
            throw "D3D12_DESCRIPTOR_RANGE1与D3D12_ROOT_PARAMETER1不对应";
        }

        D3D12_ROOT_PARAMETER1 tmpRootParameter = { };
        {
            tmpRootParameter.ParameterType = parametertype;
            tmpRootParameter.ShaderVisibility = shadervisibility;
            tmpRootParameter.DescriptorTable.NumDescriptorRanges = numdescriptorranges;
            tmpRootParameter.DescriptorTable.pDescriptorRanges = &this->DescriptorRangeVector.back();
        }
        this->ParameterVector.push_back(tmpRootParameter);
    }
}