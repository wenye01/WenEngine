#pragma once

#include "../d3d12/D3D12Common.h"
#include "../d3d12/D3D12Texture.h"

namespace Gloria
{
    class RenderTarget
    {
    public:
        RenderTarget();
        
        virtual ~RenderTarget();

    public:
        GloriaD3D12TextureRef GetTexture() const { return this->pTexture; }

        GloriaD3D12Resource* GetResource() const { return this->pTexture->GetResource(); }

        DXGI_FORMAT GetFormat() const { return this->Format; }

        DirectX::XMFLOAT4 GetClearValue() const { return this->ClearValue; }

        float* GetClearValurPointer() const { return (float*)&this->ClearValue; }

    protected:
        D3D12Common* pCommon = nullptr;

        GloriaD3D12TextureRef pTexture = nullptr;

        bool bRenderDepth;

        UINT Width;
        UINT Height;

        DXGI_FORMAT Format;

        DirectX::XMFLOAT4 ClearValue;
    };
}
