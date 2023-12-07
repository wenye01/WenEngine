#pragma once

#include <wrl/client.h>

#include "../d3d12/D3D12Common.h"

namespace Gloria
{
    class Shader
    {
    public:
        Shader();

        ~Shader();

    private:
        Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::string& Entrypoint, const std::string& Target);

    private:
        std::wstring filepath;
    };
}
