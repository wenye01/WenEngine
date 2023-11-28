#pragma once

#include "D3D12Resource.h"
#include "D3D12View.h"
#include <DirectXMath.h>

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

        void SetREVClearValue(XMFLOAT4 rtvClearValue)
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
    typedef std::shared_ptr <GloriaD3D12Texture> GloriaD3DTextureRef;
}
