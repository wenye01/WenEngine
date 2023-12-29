#include "RenderTarget.h"

namespace Gloria
{
    RenderTarget::RenderTarget(D3D12Common* pcommon, bool bdepth, UINT width, UINT height, DXGI_FORMAT format, DirectX::XMFLOAT4 clearValue)
        :pCommon(pcommon), bRenderDepth(bdepth), Width(width), Height(height), Format(format), ClearValue(clearValue)
    {

    }

    RenderTarget::~RenderTarget()
    {

    }

    RenderTarget2D::RenderTarget2D(D3D12Common* pcommon, bool bdepth, UINT width, UINT height, DXGI_FORMAT format, DirectX::XMFLOAT4 clearValue)
        :RenderTarget(pcommon, bdepth, width, height, format, clearValue)
    {
        this->CreateTexture();
    }

    void RenderTarget2D::CreateTexture()
    {
        GloriaTextureInfo info;
        {
            info.width = this->Width;
            info.height = this->Height;
            info.type = TextureType::Texture_2D;
            info.dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            info.depth = 1;
            info.mipCount = 1;
            info.arraySize = 1;
            info.format = Format;
        }

        if (this->bRenderDepth)
        {
            info.dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
            info.rtvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

            this->pTexture = this->pCommon->CreateTexture(info, TEXTURE_CREATE_DSV | TEXTURE_CREATE_SRV);
        }

        else
        {
            this->pTexture = this->pCommon->CreateTexture(info, TEXTURE_CREATE_DSV | TEXTURE_CREATE_SRV, ClearValue);
        }
    }

    GloriaRenderTargetView* RenderTarget2D::GetRTV() const
    {
        if ( !this->bRenderDepth )
        {
            return this->pTexture->GetRTV();
        }
        else
        {
            return nullptr;
        }
    }

    GloriaDepthStencilView* RenderTarget2D::GetDSV() const
    {
        if (this->bRenderDepth)
        {
            return this->pTexture->GetDSV();
        }
        else
        {
            return nullptr;
        }
    }

    GloriaShaderReosurceView* RenderTarget2D::GetSRV() const
    {
        this->pTexture->GetSRV();
    }

    RenderTargetCube::RenderTargetCube(D3D12Common* pcommon, bool bdepth, UINT sideLength, DXGI_FORMAT format, DirectX::XMFLOAT4 clearValue)
        :RenderTarget(pcommon, bdepth, sideLength, sideLength, format, clearValue)
    {
        this->CreateTexture();
    }

    void RenderTargetCube::CreateTexture()
    {
        GloriaTextureInfo info;
        {
            info.width = this->Width;
            info.height = this->Height;
            info.type = TextureType::Texture_CUBE;
            info.dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            info.depth = 1;
            info.mipCount = 1;
            info.arraySize = 6;
            info.format = Format;
        }

        if (this->bRenderDepth)
        {
            info.dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
            info.rtvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

            this->pTexture = this->pCommon->CreateTexture(info, TEXTURE_CREATE_DSV | TEXTURE_CREATE_SRV);
        }

        else
        {
            this->pTexture = this->pCommon->CreateTexture(info, TEXTURE_CREATE_DSV | TEXTURE_CREATE_SRV, ClearValue);
        }
    }

    GloriaRenderTargetView* RenderTargetCube::GetRTV(int index) const
    {
        if (!this->bRenderDepth)
        {
            return this->pTexture->GetRTV(index);
        }
        else
        {
            return nullptr;
        }
    }

    GloriaDepthStencilView* RenderTargetCube::GetDSV(int index) const
    {
        if (this->bRenderDepth)
        {
            return this->pTexture->GetDSV(index);
        }
        else
        {
            return nullptr;
        }
    }

    GloriaShaderReosurceView* RenderTargetCube::GetSRV() const
    {
        this->pTexture->GetSRV();
    }
}