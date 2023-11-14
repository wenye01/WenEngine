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
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
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
				0
				, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
				, IID_PPV_ARGS(&pIAdapter1)));

			GRS_THROW_IF_FAILED(D3D12CreateDevice(
				pIAdapter1.Get()
				, D3D_FEATURE_LEVEL_12_1
				, IID_PPV_ARGS(&this->pID3D12Device4)));

			GRS_SET_D3D12_DEBUGNAME_COMPTR(this->pID3D12Device4);

			TCHAR pszWndTitle[MAX_PATH] = {};
			DXGI_ADAPTER_DESC1 stAdapterDesc = {};
			GRS_THROW_IF_FAILED(pIAdapter1->GetDesc1(&stAdapterDesc));
			::GetWindowText(hWnd, pszWndTitle, MAX_PATH);
			StringCchPrintf(pszWndTitle
				, MAX_PATH
				, _T("%s (GPU:%s)")
				, pszWndTitle
				, stAdapterDesc.Description);
			::SetWindowText(hWnd, pszWndTitle);
    }
}