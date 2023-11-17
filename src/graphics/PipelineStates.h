#pragma once

#include<functional>
#include <map>
#include <d3d12.h>

#include "Shaders.h"
#include "RootSignatures.h"

namespace Gloria
{
    using namespace Microsoft;
    using namespace Microsoft::WRL;

    class PipelineStates
    {
    public:

    private:
        
    };

    class SinglePipelineState
    {
    public:
        void Init(ID3D12Device4* pDevice);

        void InitPipelineStateDesc(std::function<D3D12_GRAPHICS_PIPELINE_STATE_DESC()> psodesc);

        void Get();
    private:
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;

        ComPtr<ID3D12PipelineState> pIPipelineStateobject;
    };
}
