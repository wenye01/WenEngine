#pragma once

#include "D3D12Utils.h"
#include "D3D12DescriptorCache.h"

namespace Gloria
{
    class GloriaD3D12CommandContext
    {
    public:
        GloriaD3D12CommandContext(GloriaD3D12Device* device);

        ~GloriaD3D12CommandContext();

        void CreateCommandContext();

        void ResetCommandAllocator();

        void ResetCommandGraphicsList();

        void ExecuteCommandList();

        void FlushCommandQueue();

        void EndFrame();

        ID3D12CommandQueue* GetCommandQueue()
        {
            return this->CommandQueue.Get();
        }

        ID3D12GraphicsCommandList* GetCommandGraphicsList()
        {
            return this->CommandGraphiceList.Get();
        }

        GloriaD3D12DescriptorCache* GetDescriptorCache()
        {
            return this->DescriptorCache.get();
        }
    private:
        GloriaD3D12Device* pDevice;

        Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue = nullptr;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandListAllocator = nullptr;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandGraphiceList = nullptr;

        std::unique_ptr<GloriaD3D12DescriptorCache> DescriptorCache = nullptr;

        Microsoft::WRL::ComPtr<ID3D12Fence> Fence = nullptr;

        UINT64 CurrentFenceValue = 0;
    };
}
