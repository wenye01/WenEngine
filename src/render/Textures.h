#pragma once

#include "../d3d12/D3D12Texture.h"
#include "../d3d12/D3D12Common.h"
#include "TextureInfo.h"

namespace Gloria
{

    class Texture
    {
    public:
        Texture(std::wstring filepath, bool isSRGB);

        ~Texture() = default;

        void LoadTextureFromFile(D3D12Common* common);

        void CreateTexture(D3D12Common* common);
    private:
        void LoadDDSTexture(GloriaD3D12Device* device);

        void LoadWICTexture(GloriaD3D12Device* device);

    private:
        std::string name;

        std::wstring filepath;

        bool SRGB;

        TextureResource textureResource;

        GloriaD3D12TextureRef texture;
    };
}
