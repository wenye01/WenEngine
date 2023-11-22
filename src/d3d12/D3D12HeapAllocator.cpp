#include "D3D12HeapAllocator.h"

namespace Gloria
{
    GloriaD3D12HeapAllocator::GloriaD3D12HeapAllocator(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap)
        :pDevice(device), HeapDesc(CreateHeapDesc(type, numDescriptorsPerHeap)), DescriptorSize(this->pDevice->GetDescriptorHandleIncrementSize(HeapDesc.Type))
    {

    }

    GloriaD3D12HeapAllocator::~GloriaD3D12HeapAllocator() { }

    D3D12_DESCRIPTOR_HEAP_DESC GloriaD3D12HeapAllocator::CreateHeapDesc(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorPerHeap)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        {
            desc.Type = type;
            desc.NumDescriptors = numDescriptorPerHeap;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            desc.NodeMask = 0;
        }
        return desc;
    }

    void GloriaD3D12HeapAllocator::AllocateHeap()
    {
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap;
        ThrowIfFailed(this->pDevice->CreateDescriptorHeap(&this->HeapDesc, IID_PPV_ARGS(&Heap)));
        SetDebugName(Heap.Get(), L"GloriaD3D12HeapAllocator");

        D3D12_CPU_DESCRIPTOR_HANDLE HeapBase = Heap->GetCPUDescriptorHandleForHeapStart();

        HeapEntry Entry;
        {
            Entry.Heap = Heap;
            Entry.FreeList.push_back({ HeapBase.ptr,HeapBase.ptr + (SIZE_T)HeapDesc.NumDescriptors * DescriptorSize });
        }

        this->HeapMap.push_back(Entry);
    }


}