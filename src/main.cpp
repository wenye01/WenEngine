#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN // �� Windows ͷ���ų�����ʹ�õ�����
#include <windows.h>
#include <tchar.h>
#include <wrl.h>  //���WTL֧�� ����ʹ��COM
#include <strsafe.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3d12.h>	//for d3d12
#include <d3d12shader.h>
#include <d3dcompiler.h>
#if defined(_DEBUG)
#include <dxgidebug.h>
#endif

using namespace Microsoft;
using namespace Microsoft::WRL;
using namespace DirectX;

//linker
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define GRS_WND_CLASS_NAME _T("GRS Game Window Class")
#define GRS_WND_TITLE	_T("GRS DirectX12 Trigger Sample")

#define GRS_THROW_IF_FAILED(hr) {HRESULT _hr = (hr);if (FAILED(_hr)){ throw CGRSCOMException(_hr); }}

class CGRSCOMException
{
public:
    CGRSCOMException(HRESULT hr) : m_hrError(hr)
    {
    }
    HRESULT Error() const
    {
        return m_hrError;
    }
private:
    const HRESULT m_hrError;
};

struct GRS_VERTEX
{
    XMFLOAT4 m_vtPos;
    XMFLOAT4 m_vtColor;
};

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR    lpCmdLine, int nCmdShow)
{
    const UINT							nFrameBackBufCount = 3u;

    int									iWidth = 1024;
    int									iHeight = 768;
    UINT								nFrameIndex = 0;

    DXGI_FORMAT							emRenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    const float							faClearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    UINT								nDXGIFactoryFlags = 0U;
    UINT								nRTVDescriptorSize = 0U;

    HWND								hWnd = nullptr;
    MSG									msg = {};
    TCHAR								pszAppPath[MAX_PATH] = {};

    float								fTrangleSize = 3.0f;

    D3D12_VERTEX_BUFFER_VIEW			stVertexBufferView = {};

    UINT64								n64FenceValue = 0ui64;
    HANDLE								hEventFence = nullptr;

    D3D12_VIEWPORT						stViewPort = { 0.0f, 0.0f, static_cast<float>(iWidth), static_cast<float>(iHeight), D3D12_MIN_DEPTH, D3D12_MAX_DEPTH };
    D3D12_RECT							stScissorRect = { 0, 0, static_cast<LONG>(iWidth), static_cast<LONG>(iHeight) };

    D3D_FEATURE_LEVEL					emFeatureLevel = D3D_FEATURE_LEVEL_12_1;

    ComPtr<IDXGIFactory5>				pIDXGIFactory5;
    ComPtr<IDXGIAdapter1>				pIAdapter1;
    ComPtr<ID3D12Device4>				pID3D12Device4;
    ComPtr<ID3D12CommandQueue>			pICMDQueue;
    ComPtr<IDXGISwapChain1>				pISwapChain1;
    ComPtr<IDXGISwapChain3>				pISwapChain3;
    ComPtr<ID3D12DescriptorHeap>		pIRTVHeap;
    ComPtr<ID3D12Resource>				pIARenderTargets[nFrameBackBufCount];
    ComPtr<ID3D12RootSignature>			pIRootSignature;
    ComPtr<ID3D12PipelineState>			pIPipelineState;

    ComPtr<ID3D12CommandAllocator>		pICMDAlloc;
    ComPtr<ID3D12GraphicsCommandList>	pICMDList;
    ComPtr<ID3D12Resource>				pIVertexBuffer;
    ComPtr<ID3D12Fence>					pIFence;

    try
    {
        // �õ���ǰ�Ĺ���Ŀ¼����������ʹ�����·�������ʸ�����Դ�ļ�
        {
            if (0 == ::GetModuleFileName(nullptr, pszAppPath, MAX_PATH))
            {
                GRS_THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
            }

            WCHAR* lastSlash = _tcsrchr(pszAppPath, _T('\\'));
            if (lastSlash)
            {//ɾ��Exe�ļ���
                *(lastSlash) = _T('\0');
            }

            lastSlash = _tcsrchr(pszAppPath, _T('\\'));
            if (lastSlash)
            {//ɾ��x64·��
                *(lastSlash) = _T('\0');
            }

            lastSlash = _tcsrchr(pszAppPath, _T('\\'));
            if (lastSlash)
            {//ɾ��Debug �� Release·��
                *(lastSlash + 1) = _T('\0');
            }
        }

        // ��������
        {
            WNDCLASSEX wcex = {};
            wcex.cbSize = sizeof(WNDCLASSEX);
            wcex.style = CS_GLOBALCLASS;
            wcex.lpfnWndProc = WndProc;
            wcex.cbClsExtra = 0;
            wcex.cbWndExtra = 0;
            wcex.hInstance = hInstance;
            wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);		//��ֹ���ĵı����ػ�
            wcex.lpszClassName = GRS_WND_CLASS_NAME;
            RegisterClassEx(&wcex);

            DWORD dwWndStyle = WS_OVERLAPPED | WS_SYSMENU;
            RECT rtWnd = { 0, 0, iWidth, iHeight };
            AdjustWindowRect(&rtWnd, dwWndStyle, FALSE);

            // ���㴰�ھ��е���Ļ����
            INT posX = (GetSystemMetrics(SM_CXSCREEN) - rtWnd.right - rtWnd.left) / 2;
            INT posY = (GetSystemMetrics(SM_CYSCREEN) - rtWnd.bottom - rtWnd.top) / 2;

            hWnd = CreateWindowW(GRS_WND_CLASS_NAME
                , GRS_WND_TITLE
                , dwWndStyle
                , posX
                , posY
                , rtWnd.right - rtWnd.left
                , rtWnd.bottom - rtWnd.top
                , nullptr
                , nullptr
                , hInstance
                , nullptr);

            if (!hWnd)
            {
                return FALSE;
            }

            ShowWindow(hWnd, nCmdShow);
            UpdateWindow(hWnd);
        }

        // ����ʾ��ϵͳ�ĵ���֧��
        {
#if defined(_DEBUG)
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
            {
                debugController->EnableDebugLayer();
                // �򿪸��ӵĵ���֧��
                nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            }
#endif
        }

        // ����DXGI Factory����
        {
            GRS_THROW_IF_FAILED(CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&pIDXGIFactory5)));
            // �ر�ALT+ENTER���л�ȫ���Ĺ��ܣ���Ϊ����û��ʵ��OnSize���������ȹر�
            GRS_THROW_IF_FAILED(pIDXGIFactory5->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

        }

        // ö������������ѡ����ʵ�������������3D�豸����
        {
            DXGI_ADAPTER_DESC1 stAdapterDesc = {};
            for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pIDXGIFactory5->EnumAdapters1(adapterIndex, &pIAdapter1); ++adapterIndex)
            {
                pIAdapter1->GetDesc1(&stAdapterDesc);

                if (stAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {//������������������豸
                    continue;
                }
                //�����������D3D֧�ֵļ��ݼ�������ֱ��Ҫ��֧��12.1��������ע�ⷵ�ؽӿڵ��Ǹ���������Ϊ��nullptr������
                //�Ͳ���ʵ�ʴ���һ���豸�ˣ�Ҳ�������ǆ��µ��ٵ���release���ͷŽӿڡ���Ҳ��һ����Ҫ�ļ��ɣ����ס��
                if (SUCCEEDED(D3D12CreateDevice(pIAdapter1.Get(), emFeatureLevel, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
            // ����D3D12.1���豸
            GRS_THROW_IF_FAILED(D3D12CreateDevice(pIAdapter1.Get(), emFeatureLevel, IID_PPV_ARGS(&pID3D12Device4)));

            TCHAR pszWndTitle[MAX_PATH] = {};
            GRS_THROW_IF_FAILED(pIAdapter1->GetDesc1(&stAdapterDesc));
            ::GetWindowText(hWnd, pszWndTitle, MAX_PATH);
            StringCchPrintf(pszWndTitle
                , MAX_PATH
                , _T("%s (GPU:%s)")
                , pszWndTitle
                , stAdapterDesc.Description);
            ::SetWindowText(hWnd, pszWndTitle);
        }

        // ����ֱ���������
        {
            D3D12_COMMAND_QUEUE_DESC stQueueDesc = {};
            stQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            GRS_THROW_IF_FAILED(pID3D12Device4->CreateCommandQueue(&stQueueDesc, IID_PPV_ARGS(&pICMDQueue)));
        }

        // ����������
        {
            DXGI_SWAP_CHAIN_DESC1 stSwapChainDesc = {};
            stSwapChainDesc.BufferCount = nFrameBackBufCount;
            stSwapChainDesc.Width = iWidth;
            stSwapChainDesc.Height = iHeight;
            stSwapChainDesc.Format = emRenderTargetFormat;
            stSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            stSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            stSwapChainDesc.SampleDesc.Count = 1;

            GRS_THROW_IF_FAILED(pIDXGIFactory5->CreateSwapChainForHwnd(
                pICMDQueue.Get(),		// ��������Ҫ������У�Present����Ҫִ��
                hWnd,
                &stSwapChainDesc,
                nullptr,
                nullptr,
                &pISwapChain1
            ));

            GRS_THROW_IF_FAILED(pISwapChain1.As(&pISwapChain3));
            //�õ���ǰ�󻺳�������ţ�Ҳ������һ����Ҫ������ʾ�Ļ����������
            //ע��˴�ʹ���˸߰汾��SwapChain�ӿڵĺ���
            nFrameIndex = pISwapChain3->GetCurrentBackBufferIndex();

            //����RTV(��ȾĿ����ͼ)��������(����ѵĺ���Ӧ�����Ϊ������߹̶���СԪ�صĹ̶���С�Դ��)
            {
                D3D12_DESCRIPTOR_HEAP_DESC stRTVHeapDesc = {};
                stRTVHeapDesc.NumDescriptors = nFrameBackBufCount;
                stRTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                stRTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

                GRS_THROW_IF_FAILED(pID3D12Device4->CreateDescriptorHeap(
                    &stRTVHeapDesc
                    , IID_PPV_ARGS(&pIRTVHeap)));
                //�õ�ÿ��������Ԫ�صĴ�С
                nRTVDescriptorSize = pID3D12Device4->GetDescriptorHandleIncrementSize(
                    D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            }

            //����RTV��������
            {
                D3D12_CPU_DESCRIPTOR_HANDLE stRTVHandle
                    = pIRTVHeap->GetCPUDescriptorHandleForHeapStart();
                for (UINT i = 0; i < nFrameBackBufCount; i++)
                {
                    GRS_THROW_IF_FAILED(pISwapChain3->GetBuffer(i
                        , IID_PPV_ARGS(&pIARenderTargets[i])));

                    pID3D12Device4->CreateRenderTargetView(pIARenderTargets[i].Get()
                        , nullptr
                        , stRTVHandle);

                    stRTVHandle.ptr += nRTVDescriptorSize;
                }
            }

        }

        // ����һ���յĸ���������Ҳ����Ĭ�ϵĸ�������
        {
            D3D12_ROOT_SIGNATURE_DESC stRootSignatureDesc =
            {
                0
                , nullptr
                , 0
                , nullptr
                , D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
            };

            ComPtr<ID3DBlob> pISignatureBlob;
            ComPtr<ID3DBlob> pIErrorBlob;

            GRS_THROW_IF_FAILED(D3D12SerializeRootSignature(
                &stRootSignatureDesc
                , D3D_ROOT_SIGNATURE_VERSION_1
                , &pISignatureBlob
                , &pIErrorBlob));

            GRS_THROW_IF_FAILED(pID3D12Device4->CreateRootSignature(0
                , pISignatureBlob->GetBufferPointer()
                , pISignatureBlob->GetBufferSize()
                , IID_PPV_ARGS(&pIRootSignature)));
        }

        // ���������б������
        {
            GRS_THROW_IF_FAILED(pID3D12Device4->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT
                , IID_PPV_ARGS(&pICMDAlloc)));

            // ����ͼ�������б�
            GRS_THROW_IF_FAILED(pID3D12Device4->CreateCommandList(
                0
                , D3D12_COMMAND_LIST_TYPE_DIRECT
                , pICMDAlloc.Get()
                , pIPipelineState.Get()
                , IID_PPV_ARGS(&pICMDList)));

        }

        // ����Shader������Ⱦ����״̬����
        {
            ComPtr<ID3DBlob> pIBlobVertexShader;
            ComPtr<ID3DBlob> pIBlobPixelShader;

#if defined(_DEBUG)
            //����״̬�£���Shader����ĵ��Ա�־�����Ż�
            UINT nCompileFlags =
                D3DCOMPILE_DEBUG
                | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
            UINT nCompileFlags = 0;
#endif
            TCHAR pszShaderFileName[MAX_PATH] = {};
            StringCchPrintf(pszShaderFileName
                , MAX_PATH
                , _T("./shaders/test.hlsl")
                , pszAppPath);

            GRS_THROW_IF_FAILED(D3DCompileFromFile(pszShaderFileName
                , nullptr
                , nullptr
                , "VSMain"
                , "vs_5_0"
                , nCompileFlags
                , 0
                , &pIBlobVertexShader
                , nullptr));
            GRS_THROW_IF_FAILED(D3DCompileFromFile(pszShaderFileName
                , nullptr
                , nullptr
                , "PSMain"
                , "ps_5_0"
                , nCompileFlags
                , 0
                , &pIBlobPixelShader
                , nullptr));

            // Define the vertex input layout.
            D3D12_INPUT_ELEMENT_DESC stInputElementDescs[] =
            {
                {
                    "POSITION"
                    , 0
                    , DXGI_FORMAT_R32G32B32A32_FLOAT
                    , 0
                    , 0
                    , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA
                    , 0
                },
                {
                    "COLOR"
                    , 0
                    , DXGI_FORMAT_R32G32B32A32_FLOAT
                    , 0
                    , 16
                    , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA
                    , 0
                }
            };

            // ������Ⱦ����״̬�����ṹ��������Ⱦ���߶���
            D3D12_GRAPHICS_PIPELINE_STATE_DESC stPSODesc = {};
            stPSODesc.InputLayout = { stInputElementDescs, _countof(stInputElementDescs) };
            stPSODesc.pRootSignature = pIRootSignature.Get();
            stPSODesc.VS.pShaderBytecode = pIBlobVertexShader->GetBufferPointer();
            stPSODesc.VS.BytecodeLength = pIBlobVertexShader->GetBufferSize();
            stPSODesc.PS.pShaderBytecode = pIBlobPixelShader->GetBufferPointer();
            stPSODesc.PS.BytecodeLength = pIBlobPixelShader->GetBufferSize();

            stPSODesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
            stPSODesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

            stPSODesc.BlendState.AlphaToCoverageEnable = FALSE;
            stPSODesc.BlendState.IndependentBlendEnable = FALSE;
            stPSODesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

            stPSODesc.DepthStencilState.DepthEnable = FALSE;
            stPSODesc.DepthStencilState.StencilEnable = FALSE;

            stPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

            stPSODesc.NumRenderTargets = 1;
            stPSODesc.RTVFormats[0] = emRenderTargetFormat;

            stPSODesc.SampleMask = UINT_MAX;
            stPSODesc.SampleDesc.Count = 1;

            GRS_THROW_IF_FAILED(pID3D12Device4->CreateGraphicsPipelineState(&stPSODesc, IID_PPV_ARGS(&pIPipelineState)));
        }

        // �������㻺��
        {
            // ���������ε�3D���ݽṹ��ÿ������ʹ����ԭɫ֮һ
            GRS_VERTEX stTriangleVertices[] =
            {
                { { 0.0f, 0.25f * fTrangleSize, 0.0f ,1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
                { { 0.25f * fTrangleSize, -0.25f * fTrangleSize, 0.0f ,1.0f  }, { 0.0f, 1.0f, 0.0f, 1.0f } },
                { { -0.25f * fTrangleSize, -0.25f * fTrangleSize, 0.0f  ,1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
            };

            const UINT nVertexBufferSize = sizeof(stTriangleVertices);

            D3D12_HEAP_PROPERTIES stHeapProp = { D3D12_HEAP_TYPE_UPLOAD };

            D3D12_RESOURCE_DESC stResSesc = {};
            stResSesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            stResSesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            stResSesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            stResSesc.Format = DXGI_FORMAT_UNKNOWN;
            stResSesc.Width = nVertexBufferSize;
            stResSesc.Height = 1;
            stResSesc.DepthOrArraySize = 1;
            stResSesc.MipLevels = 1;
            stResSesc.SampleDesc.Count = 1;
            stResSesc.SampleDesc.Quality = 0;

            GRS_THROW_IF_FAILED(pID3D12Device4->CreateCommittedResource(
                &stHeapProp,
                D3D12_HEAP_FLAG_NONE,
                &stResSesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&pIVertexBuffer)));

            UINT8* pVertexDataBegin = nullptr;
            D3D12_RANGE stReadRange = { 0, 0 };
            GRS_THROW_IF_FAILED(pIVertexBuffer->Map(
                0
                , &stReadRange
                , reinterpret_cast<void**>(&pVertexDataBegin)));

            memcpy(pVertexDataBegin
                , stTriangleVertices
                , sizeof(stTriangleVertices));

            pIVertexBuffer->Unmap(0, nullptr);

            stVertexBufferView.BufferLocation = pIVertexBuffer->GetGPUVirtualAddress();
            stVertexBufferView.StrideInBytes = sizeof(GRS_VERTEX);
            stVertexBufferView.SizeInBytes = nVertexBufferSize;
        }

        // ����һ��ͬ�����󡪡�Χ�������ڵȴ���Ⱦ��ɣ���Ϊ����Draw Call���첽����
        {
            GRS_THROW_IF_FAILED(pID3D12Device4->CreateFence(0
                , D3D12_FENCE_FLAG_NONE
                , IID_PPV_ARGS(&pIFence)));
            n64FenceValue = 1;

            // ����һ��Eventͬ���������ڵȴ�Χ���¼�֪ͨ
            hEventFence = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (hEventFence == nullptr)
            {
                GRS_THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
            }
        }

        // �����Դ���Ͻṹ
        D3D12_RESOURCE_BARRIER stBeginResBarrier = {};
        stBeginResBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        stBeginResBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        stBeginResBarrier.Transition.pResource = pIARenderTargets[nFrameIndex].Get();
        stBeginResBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        stBeginResBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        stBeginResBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        D3D12_RESOURCE_BARRIER stEndResBarrier = {};
        stEndResBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        stEndResBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        stEndResBarrier.Transition.pResource = pIARenderTargets[nFrameIndex].Get();
        stEndResBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        stEndResBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        stEndResBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        D3D12_CPU_DESCRIPTOR_HANDLE stRTVHandle = pIRTVHeap->GetCPUDescriptorHandleForHeapStart();
        DWORD dwRet = 0;
        BOOL bExit = FALSE;

        GRS_THROW_IF_FAILED(pICMDList->Close());
        SetEvent(hEventFence);

        // ��ʼ��Ϣѭ�����������в�����Ⱦ
        while (!bExit)
        {
            dwRet = ::MsgWaitForMultipleObjects(1, &hEventFence, FALSE, INFINITE, QS_ALLINPUT);
            switch (dwRet - WAIT_OBJECT_0)
            {
            case 0:
            {
                //��ȡ�µĺ󻺳���ţ���ΪPresent�������ʱ�󻺳����ž͸�����
                nFrameIndex = pISwapChain3->GetCurrentBackBufferIndex();

                //�����������Resetһ��
                GRS_THROW_IF_FAILED(pICMDAlloc->Reset());
                //Reset�����б�������ָ�������������PSO����
                GRS_THROW_IF_FAILED(pICMDList->Reset(pICMDAlloc.Get(), pIPipelineState.Get()));

                //��ʼ��¼����
                pICMDList->SetGraphicsRootSignature(pIRootSignature.Get());
                pICMDList->SetPipelineState(pIPipelineState.Get());
                pICMDList->RSSetViewports(1, &stViewPort);
                pICMDList->RSSetScissorRects(1, &stScissorRect);

                // ͨ����Դ�����ж��󻺳��Ѿ��л���Ͽ��Կ�ʼ��Ⱦ��
                stBeginResBarrier.Transition.pResource = pIARenderTargets[nFrameIndex].Get();
                pICMDList->ResourceBarrier(1, &stBeginResBarrier);

                stRTVHandle = pIRTVHeap->GetCPUDescriptorHandleForHeapStart();
                stRTVHandle.ptr += nFrameIndex * nRTVDescriptorSize;
                //������ȾĿ��
                pICMDList->OMSetRenderTargets(1, &stRTVHandle, FALSE, nullptr);

                // ������¼�����������ʼ��һ֡����Ⱦ

                pICMDList->ClearRenderTargetView(stRTVHandle, faClearColor, 0, nullptr);
                pICMDList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
                pICMDList->IASetVertexBuffers(0, 1, &stVertexBufferView);

                //Draw Call������
                pICMDList->DrawInstanced(3, 1, 0, 0);

                //��һ����Դ���ϣ�����ȷ����Ⱦ�Ѿ����������ύ����ȥ��ʾ��
                stEndResBarrier.Transition.pResource = pIARenderTargets[nFrameIndex].Get();
                pICMDList->ResourceBarrier(1, &stEndResBarrier);
                //�ر������б�����ȥִ����
                GRS_THROW_IF_FAILED(pICMDList->Close());

                //ִ�������б�
                ID3D12CommandList* ppCommandLists[] = { pICMDList.Get() };
                pICMDQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

                //�ύ����
                GRS_THROW_IF_FAILED(pISwapChain3->Present(1, 0));

                //��ʼͬ��GPU��CPU��ִ�У��ȼ�¼Χ�����ֵ
                const UINT64 n64CurrentFenceValue = n64FenceValue;
                GRS_THROW_IF_FAILED(pICMDQueue->Signal(pIFence.Get(), n64CurrentFenceValue));
                n64FenceValue++;
                GRS_THROW_IF_FAILED(pIFence->SetEventOnCompletion(n64CurrentFenceValue, hEventFence));
            }
            break;
            case 1:
            {//������Ϣ
                while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    if (WM_QUIT != msg.message)
                    {
                        ::TranslateMessage(&msg);
                        ::DispatchMessage(&msg);
                    }
                    else
                    {
                        bExit = TRUE;
                    }
                }
            }
            break;
            case WAIT_TIMEOUT:
            {

            }
            break;
            default:
                break;
            }
        }
    }
    catch (CGRSCOMException& e)
    {//������COM�쳣
        e;
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}