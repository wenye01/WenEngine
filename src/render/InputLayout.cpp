#include "InputLayout.h"

namespace Gloria
{
    void InputLayout::AddInputLayout(const std::string& name, _In_ const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout)
    {
        this->layoutMap.insert({ name, inputLayout });
    }

    void InputLayout::GetInputLayout(const std::string& name, _Out_ std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout)
    {
        auto iter = this->layoutMap.find(name);
        if (iter == this->layoutMap.end())
        {
            // TODO
        }
        else
        {
            inputLayout = iter->second;
        }
    }
}