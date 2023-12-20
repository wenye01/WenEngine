#pragma once

#include <wrl/client.h>

#include"../d3d12/D3D12Common.h"

namespace Gloria
{
    class Render
    {
    public:
        Render();

        ~Render();

        void Initialize();

        void Draw();

        void EndFrame();
    private:
    };
}
