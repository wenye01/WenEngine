#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <tchar.h>

#include "error.hpp"
#include "common/structures.h"

namespace Gloria
{
    using namespace Microsoft;
    using namespace Microsoft::WRL;

    class Engine
    {
    public:
        Engine() { }
    private:
        wndStruct window;

        ComPtr<IDXGIFactory5> pIDXGIFactory5;
        ComPtr<ID3D12Device4> pID3D12Device4;

        ComPtr<ID3D12CommandAllocator> pICmdAlloc;
        ComPtr<ID3D12CommandQueue> pIMainCmdQueue;
        ComPtr<ID3D12GraphicsCommandList>	pICmdList;

        void FactoryInit();
        void DeviceInit();
        void CmdListInit();
        void SwapChainInit();
    };
}

