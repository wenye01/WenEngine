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
        bool operator==(const PSOInfo pso) const
        {
            return (pso.InputLayoutName == this->InputLayoutName
                && pso.pShader == this->pShader
                && pso.PrimitiveTopologyType == this->PrimitiveTopologyType
                && pso.RasterizerDesc.CullMode == this->RasterizerDesc.CullMode
                && pso.DepthStencilDesc.DepthFunc == this->DepthStencilDesc.DepthFunc);
        }

        std::string InputLayoutName;
        Shader* pShader = nullptr;
        DXGI_FORMAT RTVFormats[8] = { DXGI_FORMAT_R8G8B8A8_UNORM };
        bool _4xMsaaState = false;
        UINT _4xMsaaQuality = 0;
        DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        D3D12_RASTERIZER_DESC RasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        D3D12_BLEND_DESC BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        D3D12_DEPTH_STENCIL_DESC DepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        UINT NumRenderTargets = 1;
    };

    class GraphicsPSOManage
    {
    public:
        GraphicsPSOManage(D3D12Common* pcommon, InputLayout* inputLayout);

        ~GraphicsPSOManage();

        void TryCreatePSO(PSOInfo info);

    private:
        void CreatePSO(PSOInfo info);
    private:
        D3D12Common* pCommon = nullptr;

        InputLayout* pInputLayout = nullptr;

        std::unordered_map<PSOInfo, Microsoft::WRL::ComPtr<ID3D12PipelineState>> PSOMap;
    };
}

namespace std
{
    template <>
    struct hash<Gloria::PSOInfo>
    {
        //TODO
    };
}