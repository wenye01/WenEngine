#pragma once

namespace Gloria
{
    class GloriaD3D12Device;

    class GloriaD3D12View
    {
    public:
        GloriaD3D12View();

        ~GloriaD3D12View();


    private:
        GloriaD3D12Device* pDevice = nullptr;

        
    };
}
