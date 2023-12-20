#pragma once

#include <unordered_map>

#include "../d3d12/D3D12Utils.h"
#include "InputLayout.h"
#include "Shaders.h"

namespace Gloria
{
    class D3D12Common;

    class PipelineStateObject
    {

    };

    class GraphicsPSOManage
    {
    public:
        GraphicsPSOManage();

        ~GraphicsPSOManage();

        void TryCreatePSO();

    private:
        D3D12Common* pCommon = nullptr;

        InputLayout* inputLayout = nullptr;
    };
}
