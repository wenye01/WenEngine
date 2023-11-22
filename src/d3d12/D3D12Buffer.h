#pragma once

#include "D3D12MemoryAllocator.h"

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

    class GloriaConstBuffer :public GloriaD3D12Buffer
    {

    };

    
}

