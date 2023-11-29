#include "D3D12Common.h"

namespace Gloria
{
    D3D12Common::D3D12Common() { }

    D3D12Common::~D3D12Common()
    {
        this->Destory();
    }

    void D3D12Common::Initialize(HWND hwnd, int wndWidth, int wndHeight)
    {
        UINT flags = 0;

        ThrowIfFailed(CreateDXGIFactory2(flags, IID_PPV_ARGS(this->DxgiFactory.GetAddressOf())));

        this->pDevice = std::make_unique<GloriaD3D12Device>(this);

        {
            this->SwapChainInfo.hwnd = hwnd;
            this->SwapChainInfo.BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            this->SwapChainInfo.DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
            this->SwapChainInfo.Enalble4xMSAA = false;
            this->SwapChainInfo.QualityOf4xMSAA = GetSupportMSAAQuality(this->SwapChainInfo.BackBufferFormat);
        }


    }

    void D3D12Common::Destory()
    {
        EndFrame();

        this->pSwapChain.reset();

        this->pDevice.reset();
    }

    void D3D12Common::FlushCommandQueue()
    {
        this->GetDevice()->GetCommanContext()->FlushCommandQueue();
    }

    void D3D12Common::ExecuteCommandList()
    {
        this->GetDevice()->GetCommanContext()->ExecuteCommandList();
    }

    void D3D12Common::ResetCommandList()
    {
        this->GetDevice()->GetCommanContext()->ResetCommandGraphicsList();
    }

    void D3D12Common::ResetCommandAlocator()
    {
        this->GetDevice()->GetCommanContext()->ResetCommandAllocator();
    }

    void D3D12Common::Present()
    {
        this->GetSwapChain()->
    }
}