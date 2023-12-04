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

        ID3DBlob* GetVertexShader();
        ID3DBlob* GetPixelShader();
    private:
        Microsoft::WRL::ComPtr<ID3DBlob> CompileShader()

    };
}
