#pragma once

#include "D3D12MemoryAllocator.h"
#include "D3D12View.h"

namespace Gloria
{
    class GloriaD3D12Buffer
    {
    public:
        GloriaD3D12Buffer() { }

        ~GloriaD3D12Buffer() { }

        GloriaD3D12Resource* GetResource()
        {
            return this->ResourceLocation.UnderlyingResource;
        }

        GloriaD3D12ResourceLocation ResourceLocation;
    };

    class GloriaStructedBuffer :public GloriaD3D12Buffer
    {
    public:
        GloriaShaderReosurceView* GetSRV()
        {
            return this->SRV.get();
        }

        void SetSRV(std::unique_ptr<GloriaShaderReosurceView>& srv)
        {
            this->SRV = std::move(srv);
        }
    private:
        std::unique_ptr<GloriaShaderReosurceView> SRV = nullptr;
    };
    typedef std::shared_ptr<GloriaStructedBuffer> GloriaStructedBufferRef;

    class GloriaRWStructedResourceview :public GloriaD3D12Buffer
    {
    public:
        GloriaShaderReosurceView* GetSRV()
        {
            return this->SRV.get();
        }

        GloriaUnorderedAccessView* GetUAV()
        {
            return this->UAV.get();
        }

        void SetSRV(std::unique_ptr<GloriaShaderReosurceView> srv)
        {
            this->SRV = std::move(srv);
        }

        void SetUAV(std::unique_ptr<GloriaUnorderedAccessView> uav)
        {
            this->UAV = std::move(uav);
        }

    private:
        std::unique_ptr<GloriaShaderReosurceView> SRV = nullptr;
        std::unique_ptr<GloriaUnorderedAccessView> UAV = nullptr;
    };
    typedef std::shared_ptr<GloriaRWStructedResourceview> GloriaRWStructedResourceviewRef;

    class GloriaVertexBuffer :public GloriaD3D12Buffer
    { };
    typedef std::shared_ptr<GloriaVertexBuffer> GloriaVertexBufferRef;

    class GloriaIndexBuffer :public GloriaD3D12Buffer
    { };
    typedef std::shared_ptr<GloriaIndexBuffer> GloriaIndexBufferRef;

    class GloriaReadBackBuffer :public GloriaD3D12Buffer
    { };
    typedef std::shared_ptr<GloriaReadBackBuffer> GloriaReadBackBufferRef;
}

