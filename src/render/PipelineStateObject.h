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

    struct PSOInfo
    {
        std::string InputLayoutName;
        Shader* pShader = nullptr;
        DXGI_FORMAT RTVFormats[8] = { DXGI_FORMAT_R8G8B8A8_UNORM };
        bool _4xMsaaState = false;
        UINT _4xMsaaQuality = 0;
        DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    };

    class GraphicsPSOManage
    {
    public:
        GraphicsPSOManage();

        ~GraphicsPSOManage();

        void TryCreatePSO();

    private:
        void CreatePSO();
    private:
        D3D12Common* pCommon = nullptr;

        InputLayout* inputLayout = nullptr;
    };
}
