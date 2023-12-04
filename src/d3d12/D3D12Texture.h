#pragma once

#include "D3D12Resource.h"
#include "D3D12View.h"
#include <DirectXMath.h>
#include "../windowsCommons/d3dx12.h"

namespace Gloria
{
    using namespace DirectX;
    class GloriaD3D12Texture
    {
    public:
        GloriaD3D12Resource* GetResource()
        {
            return this->ResourceLocation.UnderlyingResource;
        }

        ID3D12Resource* GetD3DResource()
        {
            return this->ResourceLocation.UnderlyingResource->pD3D12Resource.Get();
        }

        void SetRTVClearValue(XMFLOAT4 rtvClearValue)
        {
            this->RTVClearValue = rtvClearValue;
        }

        XMFLOAT4 GetRTVClearValue()
        {
            return this->RTVClearValue;
        }

        float* GetRTVClearValuePtr()
        {
            return  (float*)&this->RTVClearValue;
        }

        GloriaShaderReosurceView* GetSRV(UINT index = 0)
        {
            assert(this->SRVs.size() > index);
            return this->SRVs[index].get();
        }
        
        GloriaRenderTargetView* GetRTV(UINT index = 0)
        {
            assert(this->RTVs.size() > index);
            return this->RTVs[index].get();
        }

        GloriaDepthStencilView* GetDSV(UINT index = 0)
        {
            assert(this->DSVs.size() > index);
            return this->DSVs[index].get();
        }

        GloriaUnorderedAccessView* GetUAV(UINT index = 0)
        {
            assert(this->UAVs.size() > index);
            return this->UAVs[index].get();
        }

        void AddSRV(std::unique_ptr<GloriaShaderReosurceView> srv)
        {
            this->SRVs.emplace_back(std::move(srv));
        }

        void AddRTV(std::unique_ptr<GloriaRenderTargetView> rtv)
        {
            this->RTVs.emplace_back(std::move(rtv));
        }

        void AddDSV(std::unique_ptr<GloriaDepthStencilView> dsv)
        {
            this->DSVs.emplace_back(std::move(dsv));
        }

        void AddUAV(std::unique_ptr<GloriaUnorderedAccessView> uav)
        {
            this->UAVs.emplace_back(std::move(uav));
        }
    public:
        GloriaD3D12ResourceLocation ResourceLocation;

    private:
        std::vector <std::unique_ptr<GloriaShaderReosurceView>> SRVs;
        std::vector <std::unique_ptr<GloriaRenderTargetView>> RTVs;
        std::vector <std::unique_ptr<GloriaDepthStencilView>> DSVs;
        std::vector <std::unique_ptr<GloriaUnorderedAccessView>> UAVs;

        XMFLOAT4 RTVClearValue;
    };
    typedef std::shared_ptr <GloriaD3D12Texture> GloriaD3D12TextureRef;

    enum class TextureType
    {
        Texture_2D,
        Texture_CUBE,
        Texture_3D,
    };

    struct GloriaTextureInfo
    {
        TextureType type;
        D3D12_RESOURCE_DIMENSION dimension;
        size_t width;
        size_t height;
        size_t depth;
        size_t arraySize;
        size_t mipCount;
        
        DXGI_FORMAT format;

        D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_GENERIC_READ;

        DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT rtvFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT dsvFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT uavFormat = DXGI_FORMAT_UNKNOWN;
    };

    enum TextureCreateFlags
    {
        TEXTURE_CREATE_NONE         = 0,
        TEXTURE_CREATE_RTV          = 1 << 0,
        TEXTURE_CREATE_RTV_CUBE     = 1 << 1,
        TEXTURE_CREATE_DSV          = 1 << 2,
        TEXTURE_CREATE_DSV_CUBE     = 1 << 3,
        TEXTURE_CREATE_SRV          = 1 << 4,
        TEXTURE_CREATE_UAV          = 1 << 5,
    };
}
