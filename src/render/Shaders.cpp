#include "Shaders.h"



namespace Gloria
{
    void Shader::Initialize()
    {

    }

    Microsoft::WRL::ComPtr<ID3DBlob> Shader::CompileShader(const D3D_SHADER_MACRO* macro, const std::string& Entrypoint, const std::string& Target)
    {
        UINT compileFlags = 0;

#if defined(_DEBUG)
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
        
        Microsoft::WRL::ComPtr<ID3DBlob> byte;
        Microsoft::WRL::ComPtr<ID3DBlob> error;

        D3DCompileFromFile(this->filepath.c_str(), macro, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            Entrypoint.c_str(), Target.c_str(), compileFlags, 0, &byte, &error);

        return byte;
    }
}