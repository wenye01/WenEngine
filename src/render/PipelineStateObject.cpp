#include "PipelineStateObject.h"

namespace Gloria
{
    GraphicsPSOManage::GraphicsPSOManage(D3D12Common* pcommon, InputLayout* inputLayout)
        :pCommon(pcommon), pInputLayout(inputLayout)
    {

    }

    GraphicsPSOManage::~GraphicsPSOManage()
    {

    }

    void GraphicsPSOManage::TryCreatePSO()
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
        {

        }
    }
}