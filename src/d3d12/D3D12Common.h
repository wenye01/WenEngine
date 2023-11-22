#pragma once

#include "D3D12Device.h"
#include "D3D12MemoryAllocator.h"

// 外部调用
namespace Gloria
{
    class D3D12Common
    {
    public:
        D3D12Common();

        ~D3D12Common();

    public:
        GloriaD3D12Device* GetDevice() { return this->pDevice.get(); }
    private:
        std::unique_ptr<GloriaD3D12Device> pDevice = nullptr;
    };
}

