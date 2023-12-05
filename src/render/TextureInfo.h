#pragma once

#include "../d3d12/D3D12Texture.h"

namespace Gloria
{
    struct TextureResource
    {
        GloriaTextureInfo TextureInfo;

        std::unique_ptr<uint8_t[]> TextureData;

        std::vector<D3D12_SUBRESOURCE_DATA> Data;
    };
}