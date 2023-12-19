#pragma once

#include <wrl/client.h>
#include <d3dcompiler.h>
#include "../d3d12/D3D12Common.h"

namespace Gloria
{

    class Shader
    {
    public:
        Shader();

        ~Shader();

        void Initialize();
    private:
        Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const D3D_SHADER_MACRO* macro, const std::string& Entrypoint, const std::string& Target);

    private:
        std::wstring filepath;
        
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    };
}
