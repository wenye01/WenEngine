#pragma once

#include <vector>
#include <memory>

#include "../d3d12/D3D12Utils.h"

#include "../Engine/GameTimer.h"

namespace Gloria
{
    class World
    {
    public:
        World();

        virtual ~World();

        virtual void Initialize();

        virtual void Update(const GameTimer& gt);

        virtual void EndFrame(const GameTimer& gt);

    public:
        virtual void OnMouseDown(WPARAM btnState, int x, int y);

        virtual void OnMouseUp(WPARAM btnState, int x, int y);

        virtual void OnMouseMove(WPARAM btnState, int x, int y);

        virtual void OnMouseWheel(float wheelDistance);

        virtual void OnKeyboardInput(const GameTimer& gt);

    protected:
        HWND WindowHandle;
    private:
        POINT LastMousePostion;

    };
}
