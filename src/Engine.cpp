#include "Engine.h"

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif

namespace Gloria
{
    void Engine::FactoryInit()
    {
        UINT DXGIFactoryFlags = 0u;
#if defined(_DEBUG)
        ComPtr<ID3D12Debug> DebugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
        {
            DebugController->EnableDebugLayer();
            // 打开附加的调试支持
            DXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
#endif
        GRS_THROW_IF_FAILED(CreateDXGIFactory2(DXGIFactoryFlags, IID_PPV_ARGS(&this->pIDXGIFactory5)));
        GRS_SET_DXGI_DEBUGNAME_COMPTR(this->pIDXGIFactory5);
    }

    void Engine::DeviceInit()
    {
        ComPtr<IDXGIFactory6> pIDXGIFactory6;
        ComPtr<IDXGIAdapter1> pIAdapter1;

        GRS_THROW_IF_FAILED(this->pIDXGIFactory5.As(&pIDXGIFactory6));

        GRS_THROW_IF_FAILED(pIDXGIFactory6->EnumAdapterByGpuPreference(
            0, 
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, 
            IID_PPV_ARGS(&pIAdapter1)));

        GRS_THROW_IF_FAILED(D3D12CreateDevice(
            pIAdapter1.Get(),
            D3D_FEATURE_LEVEL_12_1, 
            IID_PPV_ARGS(&this->pID3D12Device4)));

        GRS_SET_D3D12_DEBUGNAME_COMPTR(this->pID3D12Device4);

        TCHAR WndTitle[MAX_PATH] = {};
        DXGI_ADAPTER_DESC1 AdapterDesc = {};
        GRS_THROW_IF_FAILED(pIAdapter1->GetDesc1(&AdapterDesc));

        ::GetWindowText(this->window.hwnd, WndTitle, MAX_PATH); // 标题添加GPU信息
        StringCchPrintf(
            WndTitle,
            MAX_PATH, 
            _T("%s (GPU:%s)"), 
            WndTitle,
            AdapterDesc.Description);
        ::SetWindowText(this->window.hwnd, WndTitle);
    }

    void Engine::CmdListInit()
    {
        D3D12_COMMAND_QUEUE_DESC QueueDesc = { };
        {
            QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        }
        GRS_THROW_IF_FAILED(this->pID3D12Device4->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&this->pIMainCmdQueue)));
        GRS_SET_D3D12_DEBUGNAME_COMPTR(this->pIMainCmdQueue);

        GRS_THROW_IF_FAILED(this->pID3D12Device4->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&this->pICmdAlloc)));
        GRS_SET_D3D12_DEBUGNAME_COMPTR(this->pICmdAlloc);

        GRS_THROW_IF_FAILED(this->pID3D12Device4->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
            this->pICmdAlloc.Get(), nullptr, IID_PPV_ARGS(&this->pICmdList)));
        GRS_SET_D3D12_DEBUGNAME_COMPTR(this->pICmdList);
    }
}