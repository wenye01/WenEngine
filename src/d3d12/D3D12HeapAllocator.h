#pragma once

#include <list>
#include "D3D12Utils.h"

namespace Gloria
{
    class GloriaD3D12HeapAllocator
    {
    public:
        typedef decltype(D3D12_CPU_DESCRIPTOR_HANDLE::ptr) D3D12_CPU_DESCRIPTOR_HANDLE_PTR;
        struct HeapSlot
        {
            uint32_t HeapIndex;
            D3D12_CPU_DESCRIPTOR_HANDLE Handle;
        };

    private:
        struct FreeRange
        {
            D3D12_CPU_DESCRIPTOR_HANDLE_PTR start;
            D3D12_CPU_DESCRIPTOR_HANDLE_PTR end;
        };

        struct HeapEntry
        {
            Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap = nullptr;
            std::list<GloriaD3D12HeapAllocator::FreeRange> FreeList;

            HeapEntry() { }
        };

    public:
        GloriaD3D12HeapAllocator(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap);

        ~GloriaD3D12HeapAllocator();

        HeapSlot AllocateHeapSlot();

        void FreeHeapSlot(const HeapSlot& slot);

    private:
        D3D12_DESCRIPTOR_HEAP_DESC CreateHeapDesc(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorPerHeap);

        void AllocateHeap();

        ID3D12Device* pDevice;

        const D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;

        const uint32_t DescriptorSize;

        std::vector<HeapEntry> HeapMap;
    };
}
