#include "D3D12CommandContext.h"
#include "D3D12Device.h"

namespace Gloria
{
    GloriaD3D12CommandContext::GloriaD3D12CommandContext(GloriaD3D12Device* device)
        :pDevice(device)
    {
        this->CreateCommandContext();
    }

    GloriaD3D12CommandContext::~GloriaD3D12CommandContext() { }

    void GloriaD3D12CommandContext::CreateCommandContext()
    {
        ThrowIfFailed(this->pDevice->GetD3DDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&this->Fence)));

        D3D12_COMMAND_QUEUE_DESC desc;
        {
            desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        }
        ThrowIfFailed(this->pDevice->GetD3DDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&this->CommandQueue)));

        ThrowIfFailed(this->pDevice->GetD3DDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(this->CommandListAllocator.GetAddressOf())));

        ThrowIfFailed(this->pDevice->GetD3DDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
            this->CommandListAllocator.Get(),
            nullptr, IID_PPV_ARGS(this->CommandGraphiceList.GetAddressOf())));

        ThrowIfFailed(this->CommandGraphiceList->Close());
    }

    void GloriaD3D12CommandContext::ResetCommandAllocator()
    {
        ThrowIfFailed(this->CommandListAllocator->Reset());
    }

    void GloriaD3D12CommandContext::ResetCommandGraphicsList()
    {
        ThrowIfFailed(this->CommandGraphiceList->Reset(this->CommandListAllocator.Get(), nullptr));
    }

    void GloriaD3D12CommandContext::ExecuteCommandList()
    {
        ThrowIfFailed(this->CommandGraphiceList->Close());

        ID3D12CommandList* cmdsLists[] = { this->CommandGraphiceList.Get() };
        this->CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
    }

    void GloriaD3D12CommandContext::FlushCommandQueue()
    {// TODO ¶àÏß³Ì
        this->CurrentFenceValue++;

        ThrowIfFailed(this->CommandQueue->Signal(this->Fence.Get(), this->CurrentFenceValue));

        if (this->Fence->GetCompletedValue() < this->CurrentFenceValue)
        {
            HANDLE event = CreateEvent(nullptr, false, false, nullptr);

            ThrowIfFailed(Fence->SetEventOnCompletion(this->CurrentFenceValue, event));

            WaitForSingleObject(event, INFINITE);
            CloseHandle(event);
        }
    }

    void GloriaD3D12CommandContext::EndFrame()
    {
        this->DescriptorCache->Reset();
    }
}
