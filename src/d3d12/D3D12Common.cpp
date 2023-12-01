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

        this->pSwapChain = std::make_unique<GloriaD3D12Swapchain>(this, this->SwapChainInfo, wndWidth, wndHeight);
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
        this->GetSwapChain()->Present();
    }

    void D3D12Common::ResizeSwapChain(int width, int height)
    {
        this->GetSwapChain()->OnResize(width, height);
    }

    void D3D12Common::TransitionResource(GloriaD3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter)
    {
        D3D12_RESOURCE_STATES stateBefore = resource->CurrentState;

        if (stateBefore != stateAfter)
        {
            this->GetDevice()->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource->pD3D12Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
            resource->CurrentState = stateAfter;
        }
    }

    void D3D12Common::CopyResource(GloriaD3D12Resource* DstResource, GloriaD3D12Resource* SrcResource)
    {
        this->GetDevice()->GetCommandList()->CopyResource(DstResource->pD3D12Resource.Get(), SrcResource->pD3D12Resource.Get());
    }

    void D3D12Common::CopyBufferRegion(GloriaD3D12Resource* DstResource, UINT64 DstOffset, GloriaD3D12Resource* SrcResource, UINT64 srcOffset, UINT64 Size)
    {
        this->GetDevice()->GetCommandList()->CopyBufferRegion(DstResource->pD3D12Resource.Get(), DstOffset, SrcResource->pD3D12Resource.Get(), srcOffset, Size);
    }

    void D3D12Common::CopyTextureRegion(D3D12_TEXTURE_COPY_LOCATION* Dst, UINT DstX, UINT DstY, UINT DstZ, const D3D12_TEXTURE_COPY_LOCATION* Src, const D3D12_BOX* SrcBox)
    {
        this->GetDevice()->GetCommandList()->CopyTextureRegion(Dst, DstX, DstY, DstZ, Src, SrcBox);
    }

    GloriaConstantBufferRef D3D12Common::CreateConstantnBuffer(const void* Contents, uint32_t Size)
    {
        GloriaConstantBufferRef pConstantBuffer = std::make_shared<GloriaConstantBuffer>();

        auto UploadBufferAllocator = this->GetDevice()->GetUploadBufferAllocator();
        void* MappedData = UploadBufferAllocator->AllocUploadResource(Size, UPLOAD_RESOURCE_ALIGNMENT, pConstantBuffer->ResourceLocation);

        memcpy(MappedData, Contents, Size);

        return pConstantBuffer;
    }

    GloriaStructedBufferRef D3D12Common::CreateStructedBuffer(const void* Contents, uint32_t ElementSize, uint32_t ElementCount)
    {
        assert(Contents != nullptr && ElementSize > 0 && ElementCount > 0);

        GloriaStructedBufferRef pStructedBuffer = std::make_shared< GloriaStructedBuffer>();
        
        auto UploadBufferAllocator = this->GetDevice()->GetUploadBufferAllocator();
        uint32_t dataSize = ElementSize * ElementCount;

        void* MappedData = UploadBufferAllocator->AllocUploadResource(dataSize, ElementSize, pStructedBuffer->ResourceLocation);

        memcpy(MappedData, Contents, dataSize);

        GloriaD3D12ResourceLocation& location = pStructedBuffer->ResourceLocation;
        const uint64_t Offset = location.OffsetFromBaseOfResource;
        ID3D12Resource* BufferResource = location.UnderlyingResource->pD3D12Resource.Get();

        D3D12_SHADER_RESOURCE_VIEW_DESC desc;
        {
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            desc.Format = DXGI_FORMAT_UNKNOWN;
            desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
            desc.Buffer.StructureByteStride = ElementSize;
            desc.Buffer.NumElements = ElementCount;
            desc.Buffer.FirstElement = Offset / ElementSize;
        }
        pStructedBuffer->SetSRV(std::make_unique<GloriaShaderReosurceView>(this->GetDevice(), desc, BufferResource));

        return pStructedBuffer;
    }

    GloriaRWStructedResourceviewRef D3D12Common::CreteRWStructedBuffer(uint32_t ElementSize, uint32_t ElementCount)
    {
        GloriaRWStructedResourceviewRef pRWStructedBuffer = std::make_shared<GloriaRWStructedResourceview>();

        uint32_t Datasize = ElementSize * ElementCount;

        this->CreateDefaultBuffer(Datasize, ElementSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, pRWStructedBuffer->ResourceLocation);

        GloriaD3D12ResourceLocation& Location = pRWStructedBuffer->ResourceLocation;
        const uint64_t Offset = Location.OffsetFromBaseOfResource;
        ID3D12Resource* BufferResource = Location.UnderlyingResource->pD3D12Resource.Get();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
        {
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = DXGI_FORMAT_UNKNOWN;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
            srvDesc.Buffer.StructureByteStride = ElementSize;
            srvDesc.Buffer.NumElements = ElementCount;
            srvDesc.Buffer.FirstElement = Offset / ElementSize;
        }
        pRWStructedBuffer->SetSRV(std::make_unique<GloriaShaderReosurceView>(this->GetDevice(), srvDesc, BufferResource));
        
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        {
            uavDesc.Format = DXGI_FORMAT_UNKNOWN;
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
            uavDesc.Buffer.StructureByteStride = ElementSize;
            uavDesc.Buffer.NumElements = ElementCount;
            uavDesc.Buffer.FirstElement = Offset / ElementSize;
            uavDesc.Buffer.CounterOffsetInBytes = 0;
        }
        pRWStructedBuffer->SetUAV(std::make_unique<GloriaUnorderedAccessView>(this->GetDevice(), uavDesc, BufferResource));

        return pRWStructedBuffer;
    }

    GloriaVertexBufferRef D3D12Common::CreateVertexBuffer(const void* Contents, uint32_t Size)
    {
        GloriaVertexBufferRef pVertex = std::make_shared<GloriaVertexBuffer>();

        this->CreateAndInitDefaultBuffer(Contents, Size, DEFAULT_RESOURCE_ALIGNMENT, pVertex->ResourceLocation);
        
        return pVertex;
    }

    GloriaIndexBufferRef D3D12Common::CreateIndexBuffer(const void* Contents, uint32_t Size)
    {
        GloriaIndexBufferRef pIndex = std::make_shared<GloriaIndexBuffer>();

        this->CreateAndInitDefaultBuffer(Contents, Size, DEFAULT_RESOURCE_ALIGNMENT, pIndex->ResourceLocation);

        return pIndex;
    }

    GloriaReadBackBufferRef D3D12Common::CreateReadBackBuffer(uint32_t Size)
    {
        GloriaReadBackBufferRef pReadBack = std::make_shared<GloriaReadBackBuffer>();

        Microsoft::WRL::ComPtr<ID3D12Resource> resource;

        HRESULT hr = this->pDevice->GetD3DDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(Size),
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&resource));

        ThrowIfFailed(hr);

        GloriaD3D12Resource* newResource = new GloriaD3D12Resource(resource, D3D12_RESOURCE_STATE_COPY_DEST);
        pReadBack->ResourceLocation.UnderlyingResource = newResource;
        pReadBack->ResourceLocation.SetType(GloriaD3D12ResourceLocation::ResourceLocationType::StandAlone);

        return pReadBack;
    }

    GloriaD3D12TextureRef D3D12Common::CreateTexture(const GloriaTextureInfo& textureInfo, uint32_t createFlag, XMFLOAT4 rtvClratValue = XMFLOAT4(0.f, 0.f, 0.f, 0.f))
    {
        GloriaD3D12TextureRef pTexture = this->CreateTextureResource(textureInfo, createFlag, rtvClratValue);

        this->CreateTextureViews(pTexture, textureInfo, createFlag);

        return pTexture;
    }

    GloriaD3D12TextureRef D3D12Common::CreateTexture(Microsoft::WRL::ComPtr<ID3D12Resource> resource, GloriaTextureInfo info, uint32_t createFlag)
    {
        GloriaD3D12TextureRef pTexture = std::make_shared<GloriaD3D12Texture>();

        GloriaD3D12Resource* newResource = new GloriaD3D12Resource(resource, info.initState);
        pTexture->ResourceLocation.UnderlyingResource = newResource;
        pTexture->ResourceLocation.SetType(GloriaD3D12ResourceLocation::ResourceLocationType::StandAlone);

        this->CreateTextureViews(pTexture, info, createFlag);

        return pTexture;
    }

    void D3D12Common::UploadTextureData(GloriaD3D12TextureRef texture, const std::vector<D3D12_SUBRESOURCE_DATA>& initData)
    {
        GloriaD3D12Resource* textureResource = texture->GetResource();
        D3D12_RESOURCE_DESC desc = textureResource->pD3D12Resource->GetDesc();

        const UINT NumSubResources = (UINT)initData.size();
        std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> Layouts(NumSubResources);
        std::vector<uint32_t> NumRows(NumSubResources);
        std::vector<uint64_t> RowSizesInBytes(NumSubResources);

        uint64_t RequiredSize = 0;

        this->pDevice->GetD3DDevice()->GetCopyableFootprints(&desc, 0, NumSubResources, 0, *Layouts[0], &NumRows[0], &RowSizesInBytes[0], &RequiredSize);

        GloriaD3D12ResourceLocation UploadResourceLocation;
        GloriaD3D12UploadBufferAllocator* UploadAllocator = this->GetDevice()->GetUploadBufferAllocator();

        void* MappedData = UploadAllocator->AllocUploadResource((uint32_t)RequiredSize, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT, UploadResourceLocation);
        ID3D12Resource* UploadBuffer = UploadResourceLocation.UnderlyingResource->pD3D12Resource.Get();

        for (uint32_t i = 0; i < NumSubResources; i++)
        {
            if (RowSizesInBytes[i] > SIZE_T(-1))
            {
                assert(0);
            }
            D3D12_MEMCPY_DEST dest = { (BYTE*)MappedData * +Layouts[i].Offset,Layouts[i].Footprint.RowPitch,SIZE_T(Layouts[i].Footprint.RowPitch) * SIZE_T(NumRows[i]) };
            MemcpySubresource(&desc, &(initData[i]), static_cast<SIZE_T>(RowSizesInBytes[i]), NumRows[i], Layouts[i].Footprint.Depth);
        }

        this->TransitionResource(textureResource, D3D12_RESOURCE_STATE_COPY_DEST);

        for (UINT i = 0; i < NumSubResources; i++)
        {
            Layouts[i].Offset += UploadResourceLocation.OffsetFromBaseOfResource;

            CD3DX12_TEXTURE_COPY_LOCATION src;
            {
                src.pResource = UploadBuffer;
                src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                src.PlacedFootprint = Layouts[i];
            }
            CD3DX12_TEXTURE_COPY_LOCATION dst;
            {
                dst.pResource = textureResource->pD3D12Resource.Get();
                dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                dst.SubresourceIndex = i;
            }
            this->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
        }

        this->TransitionResource(textureResource, D3D12_RESOURCE_STATE_COMMON);
    }

    void D3D12Common::SetVertexBuffer(const GloriaVertexBufferRef& vertexBuffer, UINT offset, UINT stride, UINT size)
    {
        const GloriaD3D12ResourceLocation& ResourceLocation = vertexBuffer->ResourceLocation;
        GloriaD3D12Resource* resource = ResourceLocation.UnderlyingResource;
        this->TransitionResource(resource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER);

        D3D12_VERTEX_BUFFER_VIEW view;
        {
            view.BufferLocation = ResourceLocation.GPUVirtualAddress + offset;
            view.StrideInBytes = stride;
            view.SizeInBytes = size;
        }
        this->GetDevice()->GetCommandList()->IASetVertexBuffers(0, 1, &view);
    }

    void D3D12Common::SetIndexBuffer(const GloriaIndexBufferRef& indexBuffer, UINT offset, DXGI_FORMAT format, UINT size)
    {
        const GloriaD3D12ResourceLocation& ResourceLocation = indexBuffer->ResourceLocation;
        GloriaD3D12Resource* resource = ResourceLocation.UnderlyingResource;
        this->TransitionResource(resource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER);

        D3D12_INDEX_BUFFER_VIEW view;
        {
            view.BufferLocation = ResourceLocation.GPUVirtualAddress + offset;
            view.Format = format;
            view.SizeInBytes = size;
        }
        this->GetDevice()->GetCommandList()->IASetVertexBuffers(&view);
    }

    void D3D12Common::EndFrame()
    {
        this->GetDevice()->GetUploadBufferAllocator()->CleanUpAllocations();

        this->GetDevice()->GetDefaultBufferAllocator()->CleanUpAllocations();

        this->GetDevice()->GetTextureBufferAllocator()->CleanUpAllocations();

        this->GetDevice()->GetCommanContext()->EndFrame();
    }

}