#pragma once

#include <d3d12.h>	
#include <wrl/client.h>
#include <tchar.h>
#include <vector>
#include <functional>
#include "../error.hpp"

namespace Gloria
{
    using namespace Microsoft;
    using namespace Microsoft::WRL;

    class ForwardShaders
    {
    public:
        void AddInputLayout(std::function<D3D12_INPUT_ELEMENT_DESC()> layout);
        void CompileVertexShader(LPCWSTR filename, LPCSTR entrypoint);
        void CompilePixelShader (LPCWSTR filename, LPCSTR entrypoint);

        ID3DBlob* GetVertexShader();
        ID3DBlob* GetPixelShader();
    private:
        std::vector<D3D12_INPUT_ELEMENT_DESC> aInputLayout;
        ComPtr<ID3DBlob> pIVertexShader;
        ComPtr<ID3DBlob> pIPixelShader;
    };
}
