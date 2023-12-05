//--------------------------------------------------------------------------------------
// File: WICTextureLoader12.h
//
// Function for loading a WIC image and creating a Direct3D runtime texture for it
// (auto-generating mipmaps if possible)
//
// Note: Assumes application has already called CoInitializeEx
//
// Note these functions are useful for images created as simple 2D textures. For
// more complex resources, DDSTextureLoader is an excellent light-weight runtime loader.
// For a full-featured DDS file reader, writer, and texture processing pipeline see
// the 'Texconv' sample and the 'DirectXTex' library.
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248926
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#ifndef _WIN32
#error This module only supports Windows
#endif

#ifdef __MINGW32__
#include <unknwn.h>
#endif

#ifdef USING_DIRECTX_HEADERS
#include <directx/d3d12.h>
#include <dxguids/dxguids.h>
#else
#include <d3d12.h>
#pragma comment(lib,"dxguid.lib")
#endif

#pragma comment(lib,"windowscodecs.lib")

#include <cstddef>
#include <cstdint>
#include <memory>

#include "../d3d12/D3D12Texture.h"

namespace DirectX
{
#ifndef WIC_LOADER_FLAGS_DEFINED
#define WIC_LOADER_FLAGS_DEFINED
    enum WIC_LOADER_FLAGS : uint32_t
    {
        WIC_LOADER_DEFAULT = 0,
        WIC_LOADER_FORCE_SRGB = 0x1,
        WIC_LOADER_IGNORE_SRGB = 0x2,
        WIC_LOADER_SRGB_DEFAULT = 0x4,
        WIC_LOADER_MIP_AUTOGEN = 0x8,
        WIC_LOADER_MIP_RESERVE = 0x10,
        WIC_LOADER_FIT_POW2 = 0x20,
        WIC_LOADER_MAKE_SQUARE = 0x40,
        WIC_LOADER_FORCE_RGBA32 = 0x80,
    };

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-dynamic-exception-spec"
#endif

    DEFINE_ENUM_FLAG_OPERATORS(WIC_LOADER_FLAGS);

#ifdef __clang__
#pragma clang diagnostic pop
#endif
#endif

    HRESULT __cdecl LoadWICTextureFromFile(
        _In_z_ const wchar_t* szFileName,
        D3D12_RESOURCE_FLAGS resFlags,
        WIC_LOADER_FLAGS loadFlags,
        std::unique_ptr<uint8_t[]>& decodedData,
        D3D12_SUBRESOURCE_DATA& subresource,
        Gloria::GloriaTextureInfo& info,
        size_t maxsize = 0) noexcept;



    HRESULT __cdecl LoadWICTextureFromFileEx(
        _In_z_ const wchar_t* szFileName,
        size_t maxsize,
        D3D12_RESOURCE_FLAGS resFlags,
        WIC_LOADER_FLAGS loadFlags,
        std::unique_ptr<uint8_t[]>& decodedData,
        D3D12_SUBRESOURCE_DATA& subresource,
        Gloria::GloriaTextureInfo& info) noexcept;
}