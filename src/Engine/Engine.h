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

        bool      bAppPaused = false;       // ��ͣ
        bool      bAppMinimized = false;    // ��С��
        bool      bAppMaximized = false;    // ���
        bool      bResizing = false;        // ���Ĵ�С
        bool      bFullscreenState = false; // ȫ��

        int WindowWidth = 1280;
        int WindowHeight = 720;

        GameTimer Timer;

        std::unique_ptr<D3D12Common> pCommon;

        std::unique_ptr<Render> pRender;
    };
}

