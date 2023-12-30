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

    void GraphicsPSOManage::TryCreatePSO(PSOInfo info)
    {
        
    }

    void GraphicsPSOManage::CreatePSO(PSOInfo info)
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
        ZeroMemory(&desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
        {
            //
            std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;
            this->pInputLayout->GetInputLayout(info.InputLayoutName, InputLayout);
            desc.InputLayout = { InputLayout.data(),(UINT)InputLayout.size() };

            //
            Shader* shader = info.pShader;
            desc.pRootSignature = shader->rootSignature.Get();
            desc.VS = CD3DX12_SHADER_BYTECODE(shader->Shaders.at("VS")->GetBufferPointer(), shader->Shaders.at("VS")->GetBufferSize());
            desc.PS = CD3DX12_SHADER_BYTECODE(shader->Shaders.at("PS")->GetBufferPointer(), shader->Shaders.at("PS")->GetBufferSize());
            desc.RasterizerState = info.RasterizerDesc;
            desc.BlendState = info.BlendDesc;
            desc.DepthStencilState = info.DepthStencilDesc;
            desc.SampleMask = UINT_MAX;
            desc.PrimitiveTopologyType = info.PrimitiveTopologyType;
            desc.NumRenderTargets = info.NumRenderTargets;
            for (int i = 0; i < 8; i++)
            {
                desc.RTVFormats[i] = info.RTVFormats[i];
            }
            desc.SampleDesc.Count = info._4xMsaaState ? 4 : 1;
            desc.SampleDesc.Quality = info._4xMsaaState ? (info._4xMsaaQuality - 1) : 0;
            desc.DSVFormat = info.DepthStencilFormat;

            Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
            auto device = this->pCommon->GetDevice()->GetD3DDevice();
            device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));
            this->PSOMap.insert({ info, pso });
        }
    }
}