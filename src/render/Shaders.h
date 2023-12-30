#pragma once

#include <unordered_map>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include "../util/util.h"
#include "../d3d12/D3D12Common.h"
#include "../d3d12/D3D12Resource.h"

namespace Gloria
{
    enum class ShaderType
    {
        VERTEX_SHADER,
        PIXEL_SHADER,
        COMPUTE_SHADER,
    };

    struct ShaderParameter
    {
        std::string Name;
        ShaderType eShaderType;
        UINT BindPoint;
        UINT RegisterSpace;
    };

    struct ShaderCBVParameter :ShaderParameter
    {
        GloriaConstantBufferRef ConstantBufferRef;
    };

    struct ShaderSRVParameter :ShaderParameter
    {
        UINT BindCount;

        std::vector<GloriaShaderReosurceView*> SRVLists;
    };

    struct ShaderUAVParameter :ShaderParameter
    {
        UINT BindCount;

        std::vector<GloriaUnorderedAccessView*> UAVLists;
    };

    struct ShaderSamplerParameter :ShaderParameter
    {

    };

    struct ShaderMacro
    {
    public:
        void GetShaderMacro(_Out_ std::vector<D3D_SHADER_MACRO>& macro) const;

        void SetShaderMacro(const std::string& Name, const std::string& Definition);

        bool operator ==(const ShaderMacro& macro) const;

    public:
        std::unordered_map<std::string, std::string> DefinesMap;
    };

    struct ShaderInfomation
    {
        std::wstring FilePath = L"./";
        std::wstring FileName;

        ShaderMacro Macro;

        bool bCreateVS = false;
        std::string VSEntryPoint = "VS";

        bool bCreatePS = false;
        std::string PSEntryPoint = "PS";

        //
        bool bCreateCS = false;
        std::string CSEntryPoint = "CS";
    };

    class Shader
    {
    public:
        Shader(const ShaderInfomation& shaderinfo, D3D12Common* pcommon);

        ~Shader();

        void Initialize();
    private:
        Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring filepath, const D3D_SHADER_MACRO* macro, const std::string& Entrypoint, const std::string& Target);

        void GetShaderParameters(Microsoft::WRL::ComPtr<ID3DBlob> blob, ShaderType type);

        D3D12_SHADER_VISIBILITY GetShaderVisibility(ShaderType type);

        void CreateRootSignature();

        std::vector<CD3DX12_STATIC_SAMPLER_DESC> CreateStaticSampler();

    public:
        std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> Shaders;

        std::vector<ShaderCBVParameter> CBVParams;

        std::vector<ShaderSRVParameter> SRVParams;

        std::vector<ShaderUAVParameter> UAVParams;

        std::vector<ShaderSamplerParameter> SamplerParams;
        
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    private:
        ShaderInfomation ShaderInfo;
             

        int CBVSignatureBaseBindSlot = -1;

        UINT SRVCount = 0;
        int SRVSignatureBindSlot = -1;

        UINT UAVCount = 0;
        int UAVSignatureBindSlot = -1;

        D3D12Common* pCommon = nullptr;
    };
}
