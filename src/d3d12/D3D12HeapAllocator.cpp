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

    GloriaD3D12HeapAllocator::HeapSlot GloriaD3D12HeapAllocator::AllocateHeapSlot()
    {
        int EntryIndex = -1;
        for (int i = 0; i < this->HeapMap.size(); i++)
        {
            if (HeapMap[i].FreeList.size() > 0)
            {
                EntryIndex = i;
                break;
            }
        }

        if (EntryIndex == -1)
        {
            AllocateHeap();

            EntryIndex = int(HeapMap.size() - 1);
        }

        HeapEntry& Entry = HeapMap[EntryIndex];
        assert(Entry.FreeList.size() > 0);

        FreeRange& Range = Entry.FreeList.front();
        HeapSlot Slot =
        {
            (uint32_t)EntryIndex,
            Range.start
        };

        Range.start += DescriptorSize;
        if (Range.start == Range.end)
        {
            Entry.FreeList.pop_front();
        }

        return Slot;
    }

    void GloriaD3D12HeapAllocator::FreeHeapSlot(const HeapSlot& slot)
    {
        assert(slot.HeapIndex < HeapMap.size());
        HeapEntry& Entry = HeapMap[slot.HeapIndex];

        FreeRange NewRange =
        {
            slot.Handle.ptr,
            slot.Handle.ptr + this->DescriptorSize
        };

        bool bFound = false;

        for (auto Node = Entry.FreeList.begin(); Node != Entry.FreeList.end() && !bFound; Node++)
        {
            FreeRange& Range = *Node;
            
            if (Range.start == NewRange.end)
            {
                Range.start = NewRange.start;
                bFound = true;
            }

            else if (Range.end == NewRange.start)
            {
                Range.end = NewRange.end;
                bFound = true;
            }

            else
            {
                if (Range.start > NewRange.start)
                {
                    Entry.FreeList.insert(Node, NewRange);
                    bFound = true;
                }
            }
        }

        if (!bFound)
        {
            Entry.FreeList.push_back(NewRange);
        }
    }
}