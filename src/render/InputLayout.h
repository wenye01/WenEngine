#pragma once

#include <unordered_map>
#include "../d3d12/D3D12Utils.h"

namespace Gloria
{
    class InputLayout
    {
    public:
        void AddInputLayout(const std::string& name, _In_ const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout);

        void GetInputLayout(const std::string& name, _Out_ std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout);
    private:
        std::unordered_map<std::string, std::vector<D3D12_INPUT_ELEMENT_DESC>> layoutMap;
    };
}
