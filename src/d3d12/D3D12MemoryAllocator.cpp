#include "D3D12MemoryAllocator.h"

namespace Gloria
{
    GloriaD3D12BuddyAllocator::GloriaD3D12BuddyAllocator(ID3D12Device* device, const AllocatorInitData& initDate)
        :pDevice(device), InitData(initDate)
    {
        this->Initialize();
    }

    GloriaD3D12BuddyAllocator::~GloriaD3D12BuddyAllocator()
    {
        if (this->pBackResource)
        {
            delete this->pBackResource;
        }
        if (this->pBackHeap)
        {
            this->pBackHeap->Release();
        }
    }

    void GloriaD3D12BuddyAllocator::Initialize()
    {// 依据分配方式创建不同的堆
        if (InitData.eAllocationStrategy == AllocationStrategy::PlacedResource)
        {
            D3D12_HEAP_PROPERTIES HeapProperties(InitData.HeapType);
            D3D12_HEAP_DESC HeapDesc;
            {
                HeapDesc.SizeInBytes = DEFAULT_POOL_SIZE;
                HeapDesc.Properties = HeapProperties;
                HeapDesc.Alignment = 0;
                HeapDesc.Flags = InitData.HeapFlags;
            }

            ID3D12Heap* pHeap = nullptr;
            ThrowIfFailed(this->pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeap)));
            pHeap->SetName(L"backheap");

            this->pBackHeap = pHeap;
        }
        else // ManualSubAllocation
        {
            CD3DX12_HEAP_PROPERTIES HeapProperties(InitData.HeapType);
            D3D12_RESOURCE_STATES HeapResourceStates;
            if (InitData.HeapType == D3D12_HEAP_TYPE_UPLOAD)
            {
                HeapResourceStates = D3D12_RESOURCE_STATE_GENERIC_READ;
            }
            else //D3D12_HEAP_TYPE_DEFAULT
            {
                HeapResourceStates = D3D12_RESOURCE_STATE_COMMON;
            }

            CD3DX12_RESOURCE_DESC BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(DEFAULT_POOL_SIZE, InitData.ResourceFlags);

            Microsoft::WRL::ComPtr<ID3D12Resource> Resource;
            ThrowIfFailed(this->pDevice->CreateCommittedResource(
                &HeapProperties,
                D3D12_HEAP_FLAG_NONE,
                &BufferDesc,
                HeapResourceStates,
                nullptr,
                IID_PPV_ARGS(&Resource)));

            Resource->SetName(L"backresource");

            this->pBackResource = new GloriaD3D12Resource(Resource);

            if (InitData.HeapType == D3D12_HEAP_TYPE_UPLOAD)
            {
                this->pBackResource->Map();
            }
        }

        // 初始化空闲区块
        MaxOrder = this->UnitSize2Order(this->Size2UnitSize(DEFAULT_POOL_SIZE));

        for (uint32_t i = 0; i <= MaxOrder; i++)
        {
            FreeBlocks.emplace_back(std::set<uint32_t>());
        }

        FreeBlocks[MaxOrder].insert((uint32_t)0);
    }

    bool GloriaD3D12BuddyAllocator::AllocateResource(uint32_t size, uint32_t aligment, GloriaD3D12ResourceLocation& resourceLocation)
    {
        uint32_t size2allocate = this->GetSizeToAllocate(size, aligment);
        if (this->CanAllocate(size2allocate))
        {
            const uint32_t unitSize = this->Size2UnitSize(size2allocate);
            const uint32_t order = this->UnitSize2Order(unitSize);
            const uint32_t offset = this->AllocateBlock(order);
            const uint32_t allocateSize = unitSize * this->MinBlockSize;
            this->AllocatedSize += allocateSize;

            const uint32_t offsetFromBaseResource = this->GetAllocateOffsetInByte(offset);
            uint32_t alignedOffsetFromBaseResource = offsetFromBaseResource;
            if (aligment != 0 && offsetFromBaseResource % aligment != 0)
            {
                alignedOffsetFromBaseResource = AlignArbitrary(offsetFromBaseResource, aligment);

                uint32_t padding = alignedOffsetFromBaseResource - offsetFromBaseResource;
                assert(padding + size <= allocateSize);
            }

            resourceLocation.SetType(GloriaD3D12ResourceLocation::ResourceLocationType::SubAllocation);
            resourceLocation.BlockData.order = order;
            resourceLocation.BlockData.offset = offset;
            resourceLocation.BlockData.actualUsedSize = size;
            resourceLocation.Allocator = this;

            if (InitData.eAllocationStrategy == AllocationStrategy::ManualSubAllocation)
            {
                resourceLocation.UnderlyingResource = this->pBackResource;
                resourceLocation.OffsetFromBaseOfResource = alignedOffsetFromBaseResource;
                resourceLocation.GPUVirtualAddress = this->pBackResource->GPUVirtualAddress + alignedOffsetFromBaseResource;

                if (InitData.HeapType == D3D12_HEAP_TYPE_UPLOAD)
                {
                    resourceLocation.MappedAddress = ((uint8_t*)this->pBackResource->MappedBaseAddress + alignedOffsetFromBaseResource);
                }
            }
            else
            {
                resourceLocation.OffsetFromBaseOfHeap = alignedOffsetFromBaseResource;
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    void GloriaD3D12BuddyAllocator::Deallocate(GloriaD3D12ResourceLocation& resourceLocation)
    {
        this->DeferredDelectionQueue.push_back(resourceLocation.BlockData);
    }

    void GloriaD3D12BuddyAllocator::CleanUpAllocations()
    {
        for (int32_t i = 0; i < this->DeferredDelectionQueue.size(); i++)
        {
            const GloriaD3D12BuddyBlockData& Block = this->DeferredDelectionQueue[i];
            this->DeallocateInternal(Block);
        }

        this->DeferredDelectionQueue.clear();
    }

    uint32_t GloriaD3D12BuddyAllocator::GetSizeToAllocate(uint32_t size, uint32_t aligment)
    {
        uint32_t size2allocate = size;

        if (aligment != 0 && this->MinBlockSize % aligment != 0)
        {
            size2allocate = size + aligment;
        }

        return size2allocate;
    }

    bool GloriaD3D12BuddyAllocator::CanAllocate(uint32_t size2allocate)
    {
        if (this->AllocatedSize == DEFAULT_POOL_SIZE)
        {
            return false;
        }

        uint32_t blockSize = DEFAULT_POOL_SIZE;

        for (int i = (int)this->FreeBlocks.size() - 1; i >= 0; i--)
        {
            if (this->FreeBlocks[i].size() && blockSize >= size2allocate)
            {
                return true;
            }

            blockSize = blockSize >> 1;

            if (blockSize < size2allocate)
            {
                return true;
            }
        }
        return false;
    }

    uint32_t GloriaD3D12BuddyAllocator::AllocateBlock(uint32_t order)
    {
        uint32_t offset;

        if (order > this->MaxOrder)
        {
            assert(false);
        }

        if (this->FreeBlocks[order].size() == 0)
        {
            uint32_t left = this->AllocateBlock(order + 1);

            uint32_t unitSize = this->Order2UnitSize(order);

            uint32_t right = left + unitSize;

            this->FreeBlocks[order].insert(right);

            offset = left;
        }
        else
        {
            auto it = this->FreeBlocks[order].cbegin();

            offset = *it;

            this->FreeBlocks[order].erase(*it);
        }

        return offset;
    }

    void GloriaD3D12BuddyAllocator::DeallocateInternal(const GloriaD3D12BuddyBlockData& block)
    {
        DeallocateBlock(block.offset, block.order);

        uint32_t Size = this->Order2UnitSize(block.order) * MinBlockSize;
        this->AllocatedSize -= Size;

        if (InitData.eAllocationStrategy == AllocationStrategy::PlacedResource)
        {
            // Release place resource
            assert(block.PlacedResource != nullptr);

            delete block.PlacedResource;
        }
    }

    void GloriaD3D12BuddyAllocator::DeallocateBlock(uint32_t offset, uint32_t order)
    {
        // Get buddy block
        uint32_t Size = this->Order2UnitSize(order);
        uint32_t Buddy = GetBuddyOffset(offset, Size);

        auto It = FreeBlocks[order].find(Buddy);
        if (It != FreeBlocks[order].end()) // If buddy block is free, merge it
        {
            // Deallocate merged blocks
            DeallocateBlock(min(offset, Buddy), order + 1);

            // Remove the buddy from the free list  
            FreeBlocks[order].erase(*It);
        }
        else
        {
            // Add the block to the free list
            FreeBlocks[order].insert(offset);
        }
    }

    GloriaD3D12MultiBuddyAllocator::GloriaD3D12MultiBuddyAllocator(ID3D12Device* device, const GloriaD3D12BuddyAllocator::AllocatorInitData initdata)
        :pDevice(device), InitData(initdata)
    {

    }

    GloriaD3D12MultiBuddyAllocator::~GloriaD3D12MultiBuddyAllocator() { }

    bool GloriaD3D12MultiBuddyAllocator::AllocateResource(uint32_t size, uint32_t aligment, GloriaD3D12ResourceLocation& resourceLocation)
    {
        for (auto& allocator : this->Allocators)
        {
            if (allocator->AllocateResource(size, aligment, resourceLocation))
            {
                return true;
            }
        }

        auto allocator = std::make_shared<GloriaD3D12BuddyAllocator>(this->pDevice, this->InitData);
        this->Allocators.push_back(allocator);

        bool result = allocator->AllocateResource(size, aligment, resourceLocation);
        assert(result);
        return true;
    }

    void GloriaD3D12MultiBuddyAllocator::CleanUpAllocations()
    {
        for (auto& allocator : this->Allocators)
        {
            allocator->CleanUpAllocations();
        }
    }

    GloriaD3D12UploadBufferAllocator::GloriaD3D12UploadBufferAllocator(ID3D12Device* InDevice)
    {
        GloriaD3D12BuddyAllocator::AllocatorInitData initdate;
        {
            initdate.eAllocationStrategy = GloriaD3D12BuddyAllocator::AllocationStrategy::ManualSubAllocation;
            initdate.HeapType = D3D12_HEAP_TYPE_UPLOAD;
            initdate.ResourceFlags = D3D12_RESOURCE_FLAG_NONE;
        }
        Allocator = std::make_unique<GloriaD3D12MultiBuddyAllocator>(InDevice, initdate);

        D3DDevice = InDevice;
    }

    void* GloriaD3D12UploadBufferAllocator::AllocUploadResource(uint32_t Size, uint32_t Alignment, GloriaD3D12ResourceLocation& ResourceLocation)
    {
        this->Allocator->AllocateResource(Size, Alignment, ResourceLocation);

        return ResourceLocation.MappedAddress;
    }

    void GloriaD3D12UploadBufferAllocator::CleanUpAllocations()
    {
        Allocator->CleanUpAllocations();
    }



    GloriaD3D12DefaultBufferAllocator::GloriaD3D12DefaultBufferAllocator(ID3D12Device* InDevice)
    {
        {
            GloriaD3D12BuddyAllocator::AllocatorInitData InitData;
            {
                InitData.eAllocationStrategy = GloriaD3D12BuddyAllocator::AllocationStrategy::ManualSubAllocation;
                InitData.HeapType = D3D12_HEAP_TYPE_DEFAULT;
                InitData.ResourceFlags = D3D12_RESOURCE_FLAG_NONE;
            }

            Allocator = std::make_unique<GloriaD3D12MultiBuddyAllocator>(InDevice, InitData);
        }

        {
            GloriaD3D12BuddyAllocator::AllocatorInitData InitData;
            InitData.eAllocationStrategy = GloriaD3D12BuddyAllocator::AllocationStrategy::ManualSubAllocation;
            InitData.HeapType = D3D12_HEAP_TYPE_DEFAULT;
            InitData.ResourceFlags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

            this->UavAllocator = std::make_unique<GloriaD3D12MultiBuddyAllocator>(InDevice, InitData);
        }

        D3DDevice = InDevice;
    }

    void GloriaD3D12DefaultBufferAllocator::AllocDefaultResource(const D3D12_RESOURCE_DESC& ResourceDesc, uint32_t Alignment, GloriaD3D12ResourceLocation& ResourceLocation)
    {
        if (ResourceDesc.Flags == D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
        {
            UavAllocator->AllocateResource((uint32_t)ResourceDesc.Width, Alignment, ResourceLocation);
        }
        else
        {
            Allocator->AllocateResource((uint32_t)ResourceDesc.Width, Alignment, ResourceLocation);
        }
    }

    void GloriaD3D12DefaultBufferAllocator::CleanUpAllocations()
    {
        this->Allocator->CleanUpAllocations();
    }



    GloriaD3D12TextureResourceAllocator::GloriaD3D12TextureResourceAllocator(ID3D12Device* InDevice)
    {
        GloriaD3D12BuddyAllocator::AllocatorInitData InitData;
        InitData.eAllocationStrategy = GloriaD3D12BuddyAllocator::AllocationStrategy::PlacedResource;
        InitData.HeapType = D3D12_HEAP_TYPE_DEFAULT;
        InitData.HeapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;

        Allocator = std::make_unique<GloriaD3D12MultiBuddyAllocator>(InDevice, InitData);

        D3DDevice = InDevice;
    }

    void GloriaD3D12TextureResourceAllocator::AllocTextureResource(const D3D12_RESOURCE_STATES& ResourceState, const D3D12_RESOURCE_DESC& ResourceDesc, GloriaD3D12ResourceLocation& ResourceLocation)
    {
        const D3D12_RESOURCE_ALLOCATION_INFO Info = D3DDevice->GetResourceAllocationInfo(0, 1, &ResourceDesc);

        Allocator->AllocateResource((uint32_t)Info.SizeInBytes, DEFAULT_RESOURCE_ALIGNMENT, ResourceLocation);

        // Create placed resource
        {
            Microsoft::WRL::ComPtr<ID3D12Resource> Resource;
            ID3D12Heap* BackingHeap = ResourceLocation.Allocator->GetBackHeap();
            uint64_t HeapOffset = ResourceLocation.OffsetFromBaseOfHeap;
            D3DDevice->CreatePlacedResource(BackingHeap, HeapOffset, &ResourceDesc, ResourceState, nullptr, IID_PPV_ARGS(&Resource));

            GloriaD3D12Resource* NewResource = new GloriaD3D12Resource(Resource);
            ResourceLocation.UnderlyingResource = NewResource;
            ResourceLocation.BlockData.PlacedResource = NewResource;  // Will delete Resource when ResourceLocation was destroyed
        }
    }

    void GloriaD3D12TextureResourceAllocator::CleanUpAllocations()
    {
        Allocator->CleanUpAllocations();
    }
}