#include "Shaders.h"

#include <d3dcompiler.h>

namespace Gloria
{
    void ForwardShaders::AddInputLayout(std::function<D3D12_INPUT_ELEMENT_DESC()> layout)
    {
        this->aInputLayout.push_back(layout());
    }
    void ForwardShaders::CompileVertexShader(LPCWSTR filename, LPCSTR entrypoint)
    {
        UINT compileFlags = 0;
#if defined(_DEBUG)
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
        //编译为行矩阵形式	   
        compileFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
        GRS_THROW_IF_FAILED(D3DCompileFromFile(filename, nullptr, nullptr,
            entrypoint, "vs_5_0", compileFlags, 0, &this->pIVertexShader, nullptr));
    }

    void ForwardShaders::CompilePixelShader(LPCWSTR filename, LPCSTR entrypoint)
    {
        UINT compileFlags = 0;
#if defined(_DEBUG)
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif   
        compileFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
        GRS_THROW_IF_FAILED(D3DCompileFromFile(filename, nullptr, nullptr,
            entrypoint, "ps_5_0", compileFlags, 0, &this->pIVertexShader, nullptr));
    }

    ID3DBlob* ForwardShaders::GetVertexShader()
    {
        return this->pIVertexShader.Get();
    }

    ID3DBlob* ForwardShaders::GetPixelShader()
    {
        return this->pIPixelShader.Get();
    }
}