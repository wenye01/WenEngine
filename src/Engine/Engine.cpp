#include <Windows.h>
#include <vector>
#include<iostream>
#include <assert.h>

#include "Engine.h"

namespace Gloria
{
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        return GloriaEngine::GetEngineSingleton()->MsgProc(hwnd, msg, wParam, lParam);
    }

    GloriaEngine* GloriaEngine::GetEngineSingleton()
    {
        return EngineSingleton;
    }

    GloriaEngine* GloriaEngine::EngineSingleton = nullptr;
    GloriaEngine::GloriaEngine(HINSTANCE hInstance)
        :EngineInstanceHandle(hInstance)
    {
        assert(EngineSingleton == nullptr);
        EngineSingleton = this;
    }

    GloriaEngine::~GloriaEngine() 
    {

    }

    HINSTANCE GloriaEngine::GetEngineInstanceHandle() const
    {
        return this->EngineInstanceHandle;
    }

    HWND GloriaEngine::GetMainWindow() const
    {
        return this->MainWindowHandle;
    }

    bool GloriaEngine::Initialize(World* world)
    {
        this->InitWindow();

        this->pCommon = std::make_unique<D3D12Common>();

        this->pRender = std::make_unique<Render>();

        this->pWorld.reset(world);
        this->pWorld->Initialize();
        
        this->pRender->Initialize();

        return true;
    }

    int GloriaEngine::Run()
    {
        MSG msg = { 0 };

        Timer.Reset();

        while (msg.message != WM_QUIT)
        {
            // If there are Window messages then process them.
            if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            // Otherwise, do animation/game stuff.
            else
            {
                Timer.Tick();

                if (!bAppPaused)
                {
                    CalculateFrameStates();

                    Update(Timer);

                    EndFrame(Timer);
                }
                else
                {
                    Sleep(100);
                }
            }
        }
        return (int)msg.wParam;
    }

    bool GloriaEngine::Detory()
    {// TODO
        return true;
    }

    LRESULT GloriaEngine::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }


    bool GloriaEngine::InitWindow()
    {
        WNDCLASS wc;
        {
            wc.style = CS_HREDRAW | CS_VREDRAW;
            wc.lpfnWndProc = WndProc;
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hInstance = EngineInstanceHandle;
            wc.hIcon = NULL;
            wc.hCursor = LoadCursor(0, IDC_ARROW);
            wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
            wc.lpszMenuName = 0;
            wc.lpszClassName = L"MainWnd";
        }

        if (!RegisterClass(&wc))
        {
            MessageBox(0, L"RegisterClass Failed.", 0, 0);
            return false;
        }

        RECT R = { 0, 0, WindowWidth, WindowHeight };
        AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
        int width = R.right - R.left;
        int height = R.bottom - R.top;

        MainWindowHandle = CreateWindow(L"MainWnd", WindowTitle.c_str(),
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
            width, height, 0, 0,
            EngineInstanceHandle, 0);

        if (!MainWindowHandle)
        {
            MessageBox(0, L"CreateWindow Failed.", 0, 0);
            return false;
        }

        ShowWindow(MainWindowHandle, SW_SHOW);
        UpdateWindow(MainWindowHandle);

        return true;
    }

    void GloriaEngine::OnResize()
    {
        
    }

    void GloriaEngine::CalculateFrameStates()
    {
        // Code computes the average frames per second, and also the 
    // average time it takes to render one frame.  These stats 
    // are appended to the window caption bar.

        static int frameCnt = 0;
        static float timeElapsed = 0.0f;

        frameCnt++;

        // Compute averages over one second period.
        if ((Timer.TotalTime() - timeElapsed) >= 1.0f)
        {
            float fps = (float)frameCnt; // fps = frameCnt / 1
            float mspf = 1000.0f / fps;

            std::wstring fpsStr = std::to_wstring(fps);
            std::wstring mspfStr = std::to_wstring(mspf);

            std::wstring windowText = WindowTitle +
                L"    fps: " + fpsStr +
                L"   mspf: " + mspfStr;

            SetWindowText(MainWindowHandle, windowText.c_str());

            // Reset for next average.
            frameCnt = 0;
            timeElapsed += 1.0f;
        }
    }

    void GloriaEngine::Update(const GameTimer& gametimer)
    {
        this->pWorld->Update(gametimer);

        this->pRender->Draw();
    }

    void GloriaEngine::EndFrame(const GameTimer& gametimer)
    {
        this->pWorld->EndFrame(gametimer);

        this->pRender->EndFrame();
    }
    
}