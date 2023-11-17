#include "PipelineStates.h"

namespace Gloria
{
    void SinglePipelineState::Init(ID3D12Device4* pdevice)
    {
        GRS_THROW_IF_FAILED(pdevice->CreateGraphicsPipelineState(&this->psoDesc, IID_PPV_ARGS(&this->pIPipelineStateobject)));
        GRS_SET_D3D12_DEBUGNAME_COMPTR(this->pIPipelineStateobject);
    }

    void SinglePipelineState::InitPipelineStateDesc(std::function<D3D12_GRAPHICS_PIPELINE_STATE_DESC()> psodesc)
    {
        this->psoDesc = psodesc();
    }
}