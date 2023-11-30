#include "D3D12Swapchain.h"
#include "D3D12Common.h"

namespace Gloria
{
    GloriaD3D12Swapchain::GloriaD3D12Swapchain(D3D12Common* common)
        :pCommon(common)
    {
        this->Initialize();
    }

    GloriaD3D12Swapchain::~GloriaD3D12Swapchain() { }

    void GloriaD3D12Swapchain::Initialize()
    {
        this->SwapChain.Reset();

        DXGI_SWAP_CHAIN_DESC desc;
        {
            desc.BufferDesc.Width = ViewportWidth;
            desc.BufferDesc.Height = ViewportHeight;
            desc.BufferDesc.RefreshRate.Numerator = 60;
            desc.BufferDesc.RefreshRate.Denominator = 1;
            desc.BufferDesc.Format = SwapchainInfoData.BackBufferFormat;
            desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
            desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
            desc.SampleDesc.Count = SwapchainInfoData.Enalble4xMSAA ? 4 : 1;
            desc.SampleDesc.Quality = SwapchainInfoData.Enalble4xMSAA ? (SwapchainInfoData.QualityOf4xMSAA - 1) : 0;
            desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            desc.BufferCount = SwapchainBufferCount;
            desc.OutputWindow = SwapchainInfoData.hwnd;
            desc.Windowed = true;
            desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        }

        Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue = this->pCommon->GetDevice()->GetCommandQueue();

        ThrowIfFailed(this->pCommon->GetDxgiFactory()->CreateSwapChain(CommandQueue.Get(), &desc, this->SwapChain.GetAddressOf()));
    }

    void GloriaD3D12Swapchain::OnResize(int width, int height)
    {
        this->ViewportWidth = width;
        this->ViewportHeight = height;

        this->pCommon->GetDevice()->GetCommanContext()->FlushCommandQueue();
        this->pCommon->GetDevice()->GetCommanContext()->ResetCommandGraphicsList();

        for (UINT i = 0; i < this->SwapchainBufferCount; i++)
        {
            this->RenderTargetTextures[i].reset();
        }
        DepthStencilTexture.reset();

        ThrowIfFailed(this->SwapChain->ResizeBuffers(this->SwapchainBufferCount,
            this->ViewportWidth, this->ViewportHeight,
            this->SwapchainInfoData.BackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

        this->CurentBackBuffer = 0;

        for (UINT i = 0; i < this->SwapchainBufferCount; i++)
        {
            Microsoft::WRL::ComPtr<ID3D12Resource> buffer = nullptr;
            ThrowIfFailed(this->SwapChain->GetBuffer(i, IID_PPV_ARGS(&buffer)));

            D3D12_RESOURCE_DESC desc = buffer->GetDesc();

            GloriaTextureInfo info;
            {
                info.rtvFormat = desc.Format;
                info.initState = D3D12_RESOURCE_STATE_PRESENT;
            }
            this->RenderTargetTextures[i] = this->pCommon->CreateTexture(buffer, info, TEXTURE_CREATE_RTV);
        }

        GloriaTextureInfo info;
        {
            info.type = TextureType::Texture_2D;
            info.dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            info.width = this->ViewportWidth;
            info.height = this->ViewportHeight;
            info.depth = 1;
            info.mipCount = 1;
            info.arraySize = 1;
            info.initState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            info.format = DXGI_FORMAT_R24G8_TYPELESS;
            info.dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
            info.srvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        }

        this->DepthStencilTexture = this->pCommon->CreateTexture(info, TEXTURE_CREATE_DSV | TEXTURE_CREATE_SRV);

        this->pCommon->GetDevice()->GetCommanContext()->ExecuteCommandList();

        this->pCommon->GetDevice()->GetCommanContext()->FlushCommandQueue();
    }

    void GloriaD3D12Swapchain::GetViewport(D3D12_VIEWPORT& OutViewPort, D3D12_RECT& OutRect)
    {
        OutViewPort.TopLeftX = 0;
        OutViewPort.TopLeftY = 0;
        OutViewPort.Width = static_cast<float>(this->ViewportWidth);
        OutViewPort.Height = static_cast<float>(this->ViewportHeight);
        OutViewPort.MinDepth = 0.f;
        OutViewPort.MaxDepth = 1.f;

        OutRect = { 0,0,this->ViewportWidth,this->ViewportHeight };
    }

    void GloriaD3D12Swapchain::Present()
    {
        ThrowIfFailed(this->SwapChain->Present(0, 0));
        this->CurentBackBuffer = (this->CurentBackBuffer + 1) % this->SwapchainBufferCount;
    }
}