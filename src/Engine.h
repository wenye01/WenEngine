#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <tchar.h>

#include "error.hpp"

namespace Gloria
{
    using namespace Microsoft;
    using namespace Microsoft::WRL;

    class Engine
    {
    public:
        Engine() { }
    private:
        ComPtr<IDXGIFactory5> pIDXGIFactory5;
        ComPtr<ID3D12Device4> pID3D12Device4;

        void FactoryInit();
        void DeviceInit();
    };
}

