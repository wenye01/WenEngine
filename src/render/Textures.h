#pragma once

#include "../d3d12/D3D12Texture.h"
#include "../d3d12/D3D12Common.h"
#include "TextureInfo.h"

namespace Gloria
{

    class Texture
    {
    public:
        Texture();

        ~Texture();

        void LoadTextureFromFile(std::wstring filepath);

    private:
        void LoadDDSTexture(GloriaD3D12Device* device);

        void LoadWICTexture(GloriaD3D12Device* device);

    private:
        std::string name;

        std::wstring filepath;

        TextureResource texture;

        bool SRBG;
    };
}
