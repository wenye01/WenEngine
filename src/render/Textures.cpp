#include "Textures.h"

#include "../util/util.h"
#include "../windowsCommons/DDSTextureLoader12.h"
#include "../windowsCommons/WICTextureLoader12.h"

namespace Gloria
{
    void Texture::LoadTextureFromFile(std::wstring filepath)
    {
        auto extension = Util::GetFileExrension(filepath);

        if (extension == L"dds")
        {
            this->LoadDDSTexture()
        }
        else if (extension == L"png" || extension == L"jpg")
        {

        }
    }

    void Texture::LoadDDSTexture(GloriaD3D12Device* device)
    {
        ThrowIfFailed(DirectX::LoadDDSTextureFromFile(
            device->GetD3DDevice(), this->filepath.c_str(),
            texture.TextureData, texture.Data, texture.TextureInfo));
    }

    void Texture::LoadWICTexture(GloriaD3D12Device* device)
    {
        D3D12_SUBRESOURCE_DATA initData;
        
        DirectX::WIC_LOADER_FLAGS flags;
        if (this->SRBG)
        {
            flags = DirectX::WIC_LOADER_FORCE_SRGB;
        }
        else
        {
            flags = DirectX::WIC_LOADER_IGNORE_SRGB;
        }

        DirectX::LoadWICTextureFromFile(this->filepath.c_str(),
            D3D12_RESOURCE_FLAG_NONE, flags,
            texture.TextureData, initData, texture.TextureInfo, 0u);

        texture.Data.push_back(initData);
    }
}