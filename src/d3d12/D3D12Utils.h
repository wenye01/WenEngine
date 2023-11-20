#pragma once

#include "../windowsCommons/d3dx12.h"
#include <dxgi1_4.h>
#include <dxgidebug.h>
#include <comdef.h>
#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>


class GloriaException
{
public:
    GloriaException() = default;
    GloriaException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

    std::wstring ToString()const;

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                                \
{                                                                       \
    HRESULT hr__ = (x);                                                 \
    std::wstring wfn = string2wstring(__FILE__);                        \
    if(FAILED(hr__)) { throw GloriaException(hr__, L#x, wfn, __LINE__); }   \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif

template<UINT TNameLength>
inline void SetDebugName(_In_ ID3D12DeviceChild* resource, _In_z_ const wchar_t(&name)[TNameLength]) noexcept
{
#if !defined(NO_D3D12_DEBUG_NAME) && (defined(_DEBUG) || defined(PROFILE))
    resource->SetName(name);
#else
    UNREFERENCED_PARAMETER(resource);
    UNREFERENCED_PARAMETER(name);
#endif
}


// Aligns a value to the nearest higher multiple of 'Alignment'.
inline uint32_t AlignArbitrary(uint32_t Val, uint32_t Alignment)
{
    return ((Val + Alignment - 1) / Alignment) * Alignment;
}

//inline UIntPoint GetTextureSize(ID3D12Resource* Texture)
//{
//    const auto Desc = Texture->GetDesc();
//    return UIntPoint(static_cast<uint32_t>(Desc.Width), static_cast<uint32_t>(Desc.Height));
//}

namespace
{
    //将string转换成wstring  
    std::wstring string2wstring(std::string str)
    {
        std::wstring result;
        //获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
        int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
        TCHAR* buffer = new TCHAR[len + 1];
        //多字节编码转换成宽字节编码  
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
        buffer[len] = '\0';             //添加字符串结尾  
        //删除缓冲区并返回值  
        result.append(buffer);
        delete[] buffer;
        return result;
    }

    //将wstring转换成string  
    std::string wstring2string(std::wstring wstr)
    {
        std::string result;
        //获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
        int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
        char* buffer = new char[len + 1];
        //宽字节编码转换成多字节编码  
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
        buffer[len] = '\0';
        //删除缓冲区并返回值  
        result.append(buffer);
        delete[] buffer;
        return result;
    }
}