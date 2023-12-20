#pragma once

#include <string>

#include "GameTimer.h"

#include "../d3d12/D3D12Common.h"
#include "../render/Render.h"
#include "../scene/World.h"

namespace Gloria
{

    class GloriaEngine
    {
    public:
        GloriaEngine(HINSTANCE hInstance);

        GloriaEngine(const GloriaEngine&) = delete;

        virtual ~GloriaEngine();

    public:
        static GloriaEngine* GetEngineSingleton();

        HINSTANCE   GetEngineInstanceHandle()const;
        HWND        GetMainWindow() const;

        bool Initialize(World* world);

        int Run();

        bool Detory();

        LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        bool InitWindow();

        void OnResize();

        void CalculateFrameStates();

    protected:
        void Update(const GameTimer& gametimer);

        void EndFrame(const GameTimer& gametimer);

        void OnMouseDown(WPARAM btnState, int x, int y) { this->pWorld->OnMouseDown(btnState, x, y); }
        void OnMouseUp(WPARAM btnState, int x, int y) { this->pWorld->OnMouseUp(btnState, x, y); }
        void OnMouseMove(WPARAM btnState, int x, int y) { this->pWorld->OnMouseMove(btnState, x, y); }
        void OnMouseWheel(float WheelDistance) { this->pWorld->OnMouseWheel(WheelDistance); }

    protected:
        static GloriaEngine* EngineSingleton;
        std::wstring WindowTitle = L"Engine";

        HINSTANCE EngineInstanceHandle = nullptr;
        HWND      MainWindowHandle = nullptr;

        bool      bAppPaused = false;       // 暂停
        bool      bAppMinimized = false;    // 最小化
        bool      bAppMaximized = false;    // 最大化
        bool      bResizing = false;        // 更改大小
        bool      bFullscreenState = false; // 全屏

        int WindowWidth = 1280;
        int WindowHeight = 720;

        GameTimer Timer;

        std::unique_ptr<D3D12Common> pCommon;

        std::unique_ptr<World> pWorld;

        std::unique_ptr<Render> pRender;
    };
}

