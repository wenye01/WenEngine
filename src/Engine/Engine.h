#pragma once

#include <string>

#include "GameTimer.h"

#include "../d3d12/D3D12Common.h"
#include "../render/Render.h"

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

        bool Initialize();

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

    protected:
        static GloriaEngine* EngineSingleton;
        std::wstring WIndowTitle = L"Engine";

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

        std::unique_ptr<Render> pRender;
    };
}

