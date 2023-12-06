#include "Textures.h"

#include "../util/util.h"
#include "../windowsCommons/DDSTextureLoader12.h"
#include "../windowsCommons/WICTextureLoader12.h"

namespace Gloria
{
    Texture::Texture(std::wstring filepath, bool isSRGB)
        :filepath(filepath), SRGB(isSRGB)
    {}

    void Texture::LoadTextureFromFile(D3D12Common* common)
    {
        auto extension = Util::GetFileExrension(this->filepath);

        if (extension == L"dds")
        {
            this->LoadDDSTexture(common->GetDevice());
        }
        else if (extension == L"png" || extension == L"jpg")
        {
            this->LoadWICTexture(common->GetDevice());
        }
    }

    void Texture::CreateTexture(D3D12Common* common)
    {
        auto cmdlist = common->GetDevice()->GetCommandList();

        texture = common->CreateTexture(this->textureResource.TextureInfo, TEXTURE_CREATE_SRV);

        common->UploadTextureData(texture, this->textureResource.Data);
    }

    void Texture::LoadDDSTexture(GloriaD3D12Device* device)
    {
        ThrowIfFailed(DirectX::LoadDDSTextureFromFile(
            device->GetD3DDevice(), this->filepath.c_str(),
            textureResource.TextureData, textureResource.Data, textureResource.TextureInfo));
    }

    void Texture::LoadWICTexture(GloriaD3D12Device* device)
    {
        D3D12_SUBRESOURCE_DATA initData;
        
        DirectX::WIC_LOADER_FLAGS flags;
        if (this->SRGB)
        {
            flags = DirectX::WIC_LOADER_FORCE_SRGB;
        }
        else
        {
            flags = DirectX::WIC_LOADER_IGNORE_SRGB;
        }

        DirectX::LoadWICTextureFromFile(this->filepath.c_str(),
            D3D12_RESOURCE_FLAG_NONE, flags,
            textureResource.TextureData, initData, textureResource.TextureInfo, 0u);

        textureResource.Data.push_back(initData);
    }
}