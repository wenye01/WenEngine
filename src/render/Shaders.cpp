#include "Shaders.h"


namespace Gloria
{
    void ShaderMacro::GetShaderMacro(_Out_ std::vector<D3D_SHADER_MACRO>& macro) const
    {
        for (const auto& pair : this->DefinesMap)
        {
            D3D_SHADER_MACRO Macro;
            {
                Macro.Name = pair.first.c_str();
                Macro.Definition = pair.second.c_str();
            }
            macro.push_back(Macro);
        }

        D3D_SHADER_MACRO Macro;
        {
            Macro.Name = NULL;
            Macro.Definition = NULL;
        }
        macro.push_back(Macro);
    }

    void ShaderMacro::SetShaderMacro(const std::string& Name, const std::string& Definition)
    {
        this->DefinesMap.insert_or_assign(Name, Definition);
    }

    bool ShaderMacro::operator==(const ShaderMacro& macro) const
    {
        if (this->DefinesMap.size() != macro.DefinesMap.size())
        {
            return false;
        }

        for (const auto& pair : this->DefinesMap)
        {
            const std::string key = pair.first;
            const std::string value = pair.second;

            auto iter = macro.DefinesMap.find(key);
            if (iter == macro.DefinesMap.end() || iter->second != value)
            {
                return false;
            }
        }

        return true;
    }

    Shader::Shader(const ShaderInfomation& shaderinfo, D3D12Common* pcommon)
        :ShaderInfo(shaderinfo), pCommon(pcommon)
    {
        this->Initialize();

        assert((this->ShaderInfo.bCreateCS | this->ShaderInfo.bCreatePS) ^ this->ShaderInfo.bCreateVS);
    }

    void Shader::Initialize()
    {
        std::wstring ShaderDir = Util::GetShaderDir() + this->ShaderInfo.FilePath + this->ShaderInfo.FileName;
        
        std::vector<D3D_SHADER_MACRO> ShaderMacros;
        this->ShaderInfo.Macro.GetShaderMacro(ShaderMacros);

        if (this->ShaderInfo.bCreateVS)
        {
            auto vsBlob = this->CompileShader(ShaderDir, ShaderMacros.data(), this->ShaderInfo.VSEntryPoint, "vs_5_1");
            this->Shaders["VS"] = vsBlob;

            this->GetShaderParameters(vsBlob, ShaderType::VERTEX_SHADER);
        }
        else if (this->ShaderInfo.bCreatePS)
        {
            auto psBlob = this->CompileShader(ShaderDir, ShaderMacros.data(), this->ShaderInfo.PSEntryPoint, "ps_5_1");
            this->Shaders["PS"] = psBlob;

            this->GetShaderParameters(psBlob, ShaderType::PIXEL_SHADER);
        }
        else if (this->ShaderInfo.bCreateCS)
        {
            auto csBlob = this->CompileShader(ShaderDir, ShaderMacros.data(), this->ShaderInfo.CSEntryPoint, "cs_5_1");
            this->Shaders["CS"] = csBlob;

            this->GetShaderParameters(csBlob, ShaderType::COMPUTE_SHADER);
        }

    }

    Microsoft::WRL::ComPtr<ID3DBlob> Shader::CompileShader(const std::wstring filepath, const D3D_SHADER_MACRO* macro, const std::string& Entrypoint, const std::string& Target)
    {
        UINT compileFlags = 0;

#if defined(_DEBUG)
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
        
        Microsoft::WRL::ComPtr<ID3DBlob> byte;
        Microsoft::WRL::ComPtr<ID3DBlob> error;

       auto hr = D3DCompileFromFile(filepath.c_str(), macro, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            Entrypoint.c_str(), Target.c_str(), compileFlags, 0, &byte, &error);

       ThrowIfFailed(hr);

        return byte;
    }

    void Shader::GetShaderParameters(Microsoft::WRL::ComPtr<ID3DBlob> blob, ShaderType type)
    {
        ID3D12ShaderReflection* Reflection = nullptr;

        D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&Reflection);

        D3D12_SHADER_DESC ShderDesc;
        Reflection->GetDesc(&ShderDesc);

        for (UINT i = 0; i < ShderDesc.BoundResources; i++)
        {
            D3D12_SHADER_INPUT_BIND_DESC desc;
            Reflection->GetResourceBindingDesc(i, &desc);

            auto ShaderVarName = desc.Name;
            auto ResourceType = desc.Type;
            auto RegisterSpace = desc.Space;
            auto BindPoint = desc.BindPoint;
            auto BindCount = desc.BindCount;

            if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER)
            {
                ShaderCBVParameter param;
                param.Name = ShaderVarName;
                param.eShaderType = type;
                param.BindPoint = BindPoint;
                param.RegisterSpace = RegisterSpace;

                this->CBVParams.push_back(param);
            }
            else if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED
                    || ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE)
            {
                ShaderSRVParameter param;
                param.Name = ShaderVarName;
                param.eShaderType = type;
                param.BindPoint = BindPoint;
                param.BindCount = BindCount;
                param.RegisterSpace = RegisterSpace;

                this->SRVParams.push_back(param);
            }
            else if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED
                || ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED)
            {
                ShaderUAVParameter param;
                param.Name = ShaderVarName;
                param.eShaderType = type;
                param.BindPoint = BindPoint;
                param.BindCount = BindCount;
                param.RegisterSpace = RegisterSpace;

                this->UAVParams.push_back(param);
            }
            else if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER)
            {
                assert(type == ShaderType::PIXEL_SHADER);

                ShaderSamplerParameter param;
                param.Name = ShaderVarName;
                param.eShaderType = type;
                param.BindPoint = BindPoint;
                param.RegisterSpace = RegisterSpace;

                this->SamplerParams.push_back(param);
            }
        }
    }

    void Shader::CreateRootSignature()
    {
        std::vector<CD3DX12_ROOT_PARAMETER> SlotRootParameter;

        {
            for (const ShaderCBVParameter& param : this->CBVParams)
            {
                if (this->CBVSignatureBaseBindSlot == -1)
                {
                    this->CBVSignatureBaseBindSlot = (UINT)SlotRootParameter.size();
                }
                CD3DX12_ROOT_PARAMETER RootParam;

                RootParam.InitAsConstantBufferView(param.BindPoint, param.RegisterSpace, this->GetShaderVisibility(param.eShaderType));
                SlotRootParameter.push_back(RootParam);
            }
        }
        {
            for (const ShaderSRVParameter& param : this->SRVParams)
            {
                this->SRVCount += param.BindCount;
            }
            
            if (SRVCount > 0)
            {
                this->SRVSignatureBindSlot = (UINT)SlotRootParameter.size();
                CD3DX12_DESCRIPTOR_RANGE SRVTable;
                SRVTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRVCount, 0, 0);

                CD3DX12_ROOT_PARAMETER RootParam;
                D3D12_SHADER_VISIBILITY ShaderVisibility = this->ShaderInfo.bCreateCS ? D3D12_SHADER_VISIBILITY_ALL : D3D12_SHADER_VISIBILITY_PIXEL;//
                RootParam.InitAsDescriptorTable(1, &SRVTable, ShaderVisibility);
                SlotRootParameter.push_back(RootParam);
            }
        }
        {
            for (const ShaderUAVParameter& param : UAVParams)
            {
                this->UAVCount += param.BindCount;
            }
            
            if (this->UAVCount > 0)
            {
                this->UAVSignatureBindSlot = (UINT)SlotRootParameter.size();

                CD3DX12_DESCRIPTOR_RANGE UAVTable;
                UAVTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UAVCount, 0, 0);

                CD3DX12_ROOT_PARAMETER RootParam;
                D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//TODO
                RootParam.InitAsDescriptorTable(1, &UAVTable, ShaderVisibility);
                SlotRootParameter.push_back(RootParam);
            }
        }

        // TODO : Sampler
        std::vector<CD3DX12_STATIC_SAMPLER_DESC> Samplers;

        CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(
            (UINT)SlotRootParameter.size(),
            SlotRootParameter.data(),
            (UINT)Samplers.size(),
            Samplers.data(),
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSignature = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

        auto hr = D3D12SerializeRootSignature(
            &RootSignatureDesc,
            D3D_ROOT_SIGNATURE_VERSION_1,
            serializedRootSignature.GetAddressOf(),
            errorBlob.GetAddressOf()
        );
        
        if (errorBlob != nullptr);
        {
            ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }

        ThrowIfFailed(hr);

        this->pCommon->GetDevice()->GetD3DDevice()->CreateRootSignature(
            0,
            serializedRootSignature->GetBufferPointer(),
            serializedRootSignature->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature)
        );
    }

}