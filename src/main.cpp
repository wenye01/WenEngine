#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN // �� Windows ͷ���ų�����ʹ�õ�����
#include <windows.h>
#include <tchar.h>
#include <fstream>  //for ifstream
//using namespace std;
//���WTL֧�� ����ʹ��COM
#include <wrl.h>
using namespace Microsoft;
using namespace Microsoft::WRL;

#include <dxgi1_6.h>
#include <DirectXMath.h>

//for d3d12
#include <d3d12.h>
#include <d3dcompiler.h>

//linker
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif

//for WIC
#include <wincodec.h>

#include "./windowsCommons/DDSTextureLoader12.h"
#include "./windowsCommons/d3dx12.h"

using namespace DirectX;

#define GRS_WND_CLASS_NAME _T("Game Window Class")
#define GRS_WND_TITLE	_T("DirectX12 Texture Sample")

#define GRS_THROW_IF_FAILED(hr) if (FAILED(hr)){ throw CGRSCOMException(hr); }

//�¶���ĺ�������ȡ������
#define GRS_UPPER_DIV(A,B) ((UINT)(((A)+((B)-1))/(B)))

//���������ϱ߽�����㷨 �ڴ�����г��� ���ס
#define GRS_UPPER(A,B) ((UINT)(((A)+((B)-1))&~(B - 1)))

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

struct WICTranslate
{
    GUID wic;
    DXGI_FORMAT format;
};

static WICTranslate g_WICFormats[] =
{//WIC��ʽ��DXGI���ظ�ʽ�Ķ�Ӧ���ñ��еĸ�ʽΪ��֧�ֵĸ�ʽ
    { GUID_WICPixelFormat128bppRGBAFloat,       DXGI_FORMAT_R32G32B32A32_FLOAT },

    { GUID_WICPixelFormat64bppRGBAHalf,         DXGI_FORMAT_R16G16B16A16_FLOAT },
    { GUID_WICPixelFormat64bppRGBA,             DXGI_FORMAT_R16G16B16A16_UNORM },

    { GUID_WICPixelFormat32bppRGBA,             DXGI_FORMAT_R8G8B8A8_UNORM },
    { GUID_WICPixelFormat32bppBGRA,             DXGI_FORMAT_B8G8R8A8_UNORM }, // DXGI 1.1
    { GUID_WICPixelFormat32bppBGR,              DXGI_FORMAT_B8G8R8X8_UNORM }, // DXGI 1.1

    { GUID_WICPixelFormat32bppRGBA1010102XR,    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM }, // DXGI 1.1
    { GUID_WICPixelFormat32bppRGBA1010102,      DXGI_FORMAT_R10G10B10A2_UNORM },

    { GUID_WICPixelFormat16bppBGRA5551,         DXGI_FORMAT_B5G5R5A1_UNORM },
    { GUID_WICPixelFormat16bppBGR565,           DXGI_FORMAT_B5G6R5_UNORM },

    { GUID_WICPixelFormat32bppGrayFloat,        DXGI_FORMAT_R32_FLOAT },
    { GUID_WICPixelFormat16bppGrayHalf,         DXGI_FORMAT_R16_FLOAT },
    { GUID_WICPixelFormat16bppGray,             DXGI_FORMAT_R16_UNORM },
    { GUID_WICPixelFormat8bppGray,              DXGI_FORMAT_R8_UNORM },

    { GUID_WICPixelFormat8bppAlpha,             DXGI_FORMAT_A8_UNORM },
};

// WIC ���ظ�ʽת����.
struct WICConvert
{
    GUID source;
    GUID target;
};

static WICConvert g_WICConvert[] =
{
    // Ŀ���ʽһ������ӽ��ı�֧�ֵĸ�ʽ
    { GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

    { GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM

    { GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM
    { GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

    { GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT
    { GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT

    { GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM

    { GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM

    { GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM

    { GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

    { GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
    { GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
    { GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
    { GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
    { GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
    { GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
    { GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT

    { GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
    { GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
    { GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
    { GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
    { GUID_WICPixelFormat32bppRGBE,             GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT

    { GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

    { GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
};

bool GetTargetPixelFormat(const GUID* pSourceFormat, GUID* pTargetFormat)
{//���ȷ�����ݵ���ӽ���ʽ���ĸ�
    *pTargetFormat = *pSourceFormat;
    for (size_t i = 0; i < _countof(g_WICConvert); ++i)
    {
        if (InlineIsEqualGUID(g_WICConvert[i].source, *pSourceFormat))
        {
            *pTargetFormat = g_WICConvert[i].target;
            return true;
        }
    }
    return false;
}

DXGI_FORMAT GetDXGIFormatFromPixelFormat(const GUID* pPixelFormat)
{//���ȷ�����ն�Ӧ��DXGI��ʽ����һ��
    for (size_t i = 0; i < _countof(g_WICFormats); ++i)
    {
        if (InlineIsEqualGUID(g_WICFormats[i].wic, *pPixelFormat))
        {
            return g_WICFormats[i].format;
        }
    }
    return DXGI_FORMAT_UNKNOWN;
}

struct ST_GRS_VERTEX
{//������Ƕ��������ÿ������ķ��ߣ���Shader�л���ʱû����
    XMFLOAT3 m_vPos;		//Position
    XMFLOAT2 m_vTex;		//Texcoord
    XMFLOAT3 m_vNor;		//Normal
};

struct ST_GRS_SKYBOX_VERTEX
{//��պ��ӵĶ���ṹ
    XMFLOAT4 m_vPos;
};

struct ST_GRS_FRAME_MVP_BUFFER
{
    XMFLOAT4X4 m_MVP;			//�����Model-view-projection(MVP)����.
};

UINT nCurrentSamplerNO = 1; //��ǰʹ�õĲ��������� ������Ĭ��ʹ�õ�һ��
UINT nSampleMaxCnt = 5;		//����������͵Ĳ�����

//��ʼ��Ĭ���������λ��
XMFLOAT3 f3EyePos = XMFLOAT3(0.0f, 0.0f, -10.0f); //�۾�λ��
XMFLOAT3 f3LockAt = XMFLOAT3(0.0f, 0.0f, 0.0f);    //�۾�������λ��
XMFLOAT3 f3HeapUp = XMFLOAT3(0.0f, 1.0f, 0.0f);    //ͷ�����Ϸ�λ��

float fYaw = 0.0f;				// ����Z�����ת��.
float fPitch = 0.0f;			// ��XZƽ�����ת��

double fPalstance = 10.0f * XM_PI / 180.0f;	//������ת�Ľ��ٶȣ���λ������/��

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR    lpCmdLine, int nCmdShow)
{
    ::CoInitialize(nullptr);  //for WIC & COM

    const UINT nFrameBackBufCount = 3u;

    int iWndWidth = 1024;
    int iWndHeight = 768;
    UINT nCurrentFrameIndex = 0;

    UINT nDXGIFactoryFlags = 0U;
    UINT nRTVDescriptorSize = 0U;

    HWND hWnd = nullptr;
    MSG	msg = {};

    ST_GRS_FRAME_MVP_BUFFER* pMVPBufEarth = nullptr;
    ST_GRS_FRAME_MVP_BUFFER* pMVPBufSkybox = nullptr;
    //������������С�϶��뵽256Bytes�߽�
    SIZE_T szMVPBuf = GRS_UPPER(sizeof(ST_GRS_FRAME_MVP_BUFFER), 256);

    float fSphereSize = 3.0f;

    D3D12_VERTEX_BUFFER_VIEW stVBVEarth = {};
    D3D12_INDEX_BUFFER_VIEW stIBVEarth = {};

    D3D12_VERTEX_BUFFER_VIEW stVBVSkybox = {};
    D3D12_INDEX_BUFFER_VIEW stIBVSkybox = {};

    UINT64 n64FenceValue = 0ui64;
    HANDLE hFenceEvent = nullptr;

    UINT nTxtWEarth = 0u;
    UINT nTxtHEarth = 0u;
    UINT nTxtWSkybox = 0u;
    UINT nTxtHSkybox = 0u;
    UINT nBPPEarth = 0u;
    UINT nBPPSkybox = 0u;
    UINT nRowPitchEarth = 0;
    UINT nRowPitchSkybox = 0;
    UINT64 n64szUploadBufEarth = 0;
    UINT64 n64szUploadBufSkybox = 0;

    DXGI_FORMAT emTxtFmtEarth = DXGI_FORMAT_UNKNOWN;


    D3D12_PLACED_SUBRESOURCE_FOOTPRINT stTxtLayoutsEarth = {};
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT stTxtLayoutsSkybox = {};
    D3D12_RESOURCE_DESC stTextureDesc = {};
    D3D12_RESOURCE_DESC stDestDesc = {};

    UINT nSamplerDescriptorSize = 0; //��������С

    CD3DX12_VIEWPORT stViewPort(0.0f, 0.0f, static_cast<float>(iWndWidth), static_cast<float>(iWndHeight));
    CD3DX12_RECT	 stScissorRect(0, 0, static_cast<LONG>(iWndWidth), static_cast<LONG>(iWndHeight));

    //�������������
    ST_GRS_VERTEX* pstSphereVertices = nullptr;
    UINT nSphereVertexCnt = 0;
    UINT* pSphereIndices = nullptr;
    UINT nSphereIndexCnt = 0;

    //Sky Box����������
    UINT nSkyboxIndexCnt = 4;
    ST_GRS_SKYBOX_VERTEX stSkyboxVertices[4] = {};

    //======================================================================================================
    //����Skybox��Cube Map��Ҫ�ı���
    std::unique_ptr<uint8_t[]> ddsData;
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DDS_ALPHA_MODE emAlphaMode = DDS_ALPHA_MODE_UNKNOWN;
    bool bIsCube = false;
    //======================================================================================================

    ComPtr<IDXGIFactory5>				pIDXGIFactory5;
    ComPtr<IDXGIAdapter1>				pIAdapter;

    ComPtr<ID3D12Device4>				pID3DDevice;
    ComPtr<ID3D12CommandQueue>			pICommandQueue;

    ComPtr<ID3D12CommandAllocator>		pICmdAllocDirect;
    ComPtr<ID3D12CommandAllocator>		pICmdAllocSkybox;
    ComPtr<ID3D12CommandAllocator>		pICmdAllocEarth;
    ComPtr<ID3D12GraphicsCommandList>	pICmdListDirect;
    ComPtr<ID3D12GraphicsCommandList>   pIBundlesSkybox;
    ComPtr<ID3D12GraphicsCommandList>   pIBundlesEarth;

    ComPtr<IDXGISwapChain1>				pISwapChain1;
    ComPtr<IDXGISwapChain3>				pISwapChain3;
    ComPtr<ID3D12Resource>				pIARenderTargets[nFrameBackBufCount];
    ComPtr<ID3D12DescriptorHeap>		pIRTVHeap;
    ComPtr<ID3D12DescriptorHeap>		pIDSVHeap;			//��Ȼ�����������
    ComPtr<ID3D12Resource>				pIDepthStencilBuffer; //������建����

    ComPtr<ID3D12Heap>					pITxtHpEarth;
    ComPtr<ID3D12Heap>					pIUploadHpEarth;
    ComPtr<ID3D12Heap>					pITxtHpSkybox;
    ComPtr<ID3D12Heap>					pIUploadHpSkybox;

    ComPtr<ID3D12Resource>				pITxtEarth;
    ComPtr<ID3D12Resource>				pITxtUpEarth;
    ComPtr<ID3D12Resource>			    pICBVUpEarth;
    ComPtr<ID3D12Resource>				pIVBEarth;
    ComPtr<ID3D12Resource>				pIIBEarth;

    ComPtr<ID3D12Resource>				pITxtSkybox;
    ComPtr<ID3D12Resource>				pITxtUpSkybox;
    ComPtr<ID3D12Resource>			    pICBVUpSkybox;
    ComPtr<ID3D12Resource>				pIVBSkybox;

    ComPtr<ID3D12DescriptorHeap>		pISRVHpEarth;
    ComPtr<ID3D12DescriptorHeap>		pISampleHpEarth;
    ComPtr<ID3D12DescriptorHeap>		pISRVHpSkybox;
    ComPtr<ID3D12DescriptorHeap>		pISampleHpSkybox;

    ComPtr<ID3D12Fence>					pIFence;
    ComPtr<ID3DBlob>					pIVSEarth;
    ComPtr<ID3DBlob>					pIPSEarth;
    ComPtr<ID3DBlob>					pIVSSkybox;
    ComPtr<ID3DBlob>					pIPSSkybox;

    ComPtr<ID3D12RootSignature>			pIRootSignature;
    ComPtr<ID3D12PipelineState>			pIPSOEarth;
    ComPtr<ID3D12PipelineState>			pIPSOSkyBox;

    ComPtr<IWICImagingFactory>			pIWICFactory;
    ComPtr<IWICBitmapDecoder>			pIWICDecoder;
    ComPtr<IWICBitmapDecoder>           pIWICSkyboxPicDecoder;
    ComPtr<IWICBitmapFrameDecode>		pIWICFrame;
    ComPtr<IWICBitmapSource>			pIBMPEarth;
    ComPtr<IWICBitmapSource>			pIBMPSkybox;

    try
    {
        //1����������
        {
            //---------------------------------------------------------------------------------------------
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
            RECT rtWnd = { 0, 0, iWndWidth, iWndHeight };
            AdjustWindowRect(&rtWnd, dwWndStyle, FALSE);

            hWnd = CreateWindowW(GRS_WND_CLASS_NAME
                , GRS_WND_TITLE
                , dwWndStyle
                , CW_USEDEFAULT
                , 0
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

        //2��ʹ��WIC����ͼƬ����ת��ΪDXGI���ݵĸ�ʽ
        {
            ComPtr<IWICFormatConverter> pIConverter;
            ComPtr<IWICComponentInfo> pIWICmntinfo;
            WICPixelFormatGUID wpf = {};
            GUID tgFormat = {};
            WICComponentType type;
            ComPtr<IWICPixelFormatInfo> pIWICPixelinfo;

            //---------------------------------------------------------------------------------------------
            //ʹ�ô�COM��ʽ����WIC�೧����Ҳ�ǵ���WIC��һ��Ҫ��������
            GRS_THROW_IF_FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory)));

            //ʹ��WIC�೧����ӿڼ�������ͼƬ�����õ�һ��WIC����������ӿڣ�ͼƬ��Ϣ��������ӿڴ���Ķ�������
            WCHAR* pszTexcuteFileName = _T("D:\\Projects_2018_08\\D3D12 Tutorials\\5-SkyBox\\Texture\\����.jpg");
            GRS_THROW_IF_FAILED(pIWICFactory->CreateDecoderFromFilename(
                pszTexcuteFileName,              // �ļ���
                NULL,                            // ��ָ����������ʹ��Ĭ��
                GENERIC_READ,                    // ����Ȩ��
                WICDecodeMetadataCacheOnDemand,  // ����Ҫ�ͻ������� 
                &pIWICDecoder                    // ����������
            ));
            // ��ȡ��һ֡ͼƬ(��ΪGIF�ȸ�ʽ�ļ����ܻ��ж�֡ͼƬ�������ĸ�ʽһ��ֻ��һ֡ͼƬ)
            // ʵ�ʽ���������������λͼ��ʽ����
            GRS_THROW_IF_FAILED(pIWICDecoder->GetFrame(0, &pIWICFrame));
            //��ȡWICͼƬ��ʽ
            GRS_THROW_IF_FAILED(pIWICFrame->GetPixelFormat(&wpf));
            //ͨ����һ��ת��֮���ȡDXGI�ĵȼ۸�ʽ
            if (GetTargetPixelFormat(&wpf, &tgFormat))
            {
                emTxtFmtEarth = GetDXGIFormatFromPixelFormat(&tgFormat);
            }

            if (DXGI_FORMAT_UNKNOWN == emTxtFmtEarth)
            {// ��֧�ֵ�ͼƬ��ʽ Ŀǰ�˳����� 
             // һ�� ��ʵ�ʵ����浱�ж����ṩ�����ʽת�����ߣ�
             // ͼƬ����Ҫ��ǰת���ã����Բ�����ֲ�֧�ֵ�����
                throw CGRSCOMException(S_FALSE);
            }

            if (!InlineIsEqualGUID(wpf, tgFormat))
            {// ����жϺ���Ҫ�����ԭWIC��ʽ����ֱ����ת��ΪDXGI��ʽ��ͼƬʱ
             // ������Ҫ���ľ���ת��ͼƬ��ʽΪ�ܹ�ֱ�Ӷ�ӦDXGI��ʽ����ʽ
                //����ͼƬ��ʽת����
                GRS_THROW_IF_FAILED(pIWICFactory->CreateFormatConverter(&pIConverter));
                //��ʼ��һ��ͼƬת������ʵ��Ҳ���ǽ�ͼƬ���ݽ����˸�ʽת��
                GRS_THROW_IF_FAILED(pIConverter->Initialize(
                    pIWICFrame.Get(),                // ����ԭͼƬ����
                    tgFormat,						 // ָ����ת����Ŀ���ʽ
                    WICBitmapDitherTypeNone,         // ָ��λͼ�Ƿ��е�ɫ�壬�ִ��������λͼ�����õ�ɫ�壬����ΪNone
                    NULL,                            // ָ����ɫ��ָ��
                    0.f,                             // ָ��Alpha��ֵ
                    WICBitmapPaletteTypeCustom       // ��ɫ�����ͣ�ʵ��û��ʹ�ã�����ָ��ΪCustom
                ));
                // ����QueryInterface������ö����λͼ����Դ�ӿ�
                GRS_THROW_IF_FAILED(pIConverter.As(&pIBMPEarth));
            }
            else
            {
                //ͼƬ���ݸ�ʽ����Ҫת����ֱ�ӻ�ȡ��λͼ����Դ�ӿ�
                GRS_THROW_IF_FAILED(pIWICFrame.As(&pIBMPEarth));
            }
            //���ͼƬ��С����λ�����أ�
            GRS_THROW_IF_FAILED(pIBMPEarth->GetSize(&nTxtWEarth, &nTxtHEarth));
            //��ȡͼƬ���ص�λ��С��BPP��Bits Per Pixel����Ϣ�����Լ���ͼƬ�����ݵ���ʵ��С����λ���ֽڣ�
            GRS_THROW_IF_FAILED(pIWICFactory->CreateComponentInfo(tgFormat, pIWICmntinfo.GetAddressOf()));
            GRS_THROW_IF_FAILED(pIWICmntinfo->GetComponentType(&type));
            if (type != WICPixelFormat)
            {
                throw CGRSCOMException(S_FALSE);
            }
            GRS_THROW_IF_FAILED(pIWICmntinfo.As(&pIWICPixelinfo));
            // ���������ڿ��Եõ�BPP�ˣ���Ҳ���ҿ��ıȽ���Ѫ�ĵط���Ϊ��BPP��Ȼ������ô�໷��
            GRS_THROW_IF_FAILED(pIWICPixelinfo->GetBitsPerPixel(&nBPPEarth));
            // ����ͼƬʵ�ʵ��д�С����λ���ֽڣ�������ʹ����һ����ȡ����������A+B-1��/B ��
            // ����������˵��΢���������,ϣ�����Ѿ���������ָ��
            nRowPitchEarth = GRS_UPPER_DIV(uint64_t(nTxtWEarth) * uint64_t(nBPPEarth), 8);
        }

        //3������ʾ��ϵͳ�ĵ���֧��
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

        //4������DXGI Factory����
        {
            GRS_THROW_IF_FAILED(CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&pIDXGIFactory5)));
            // �ر�ALT+ENTER���л�ȫ���Ĺ��ܣ���Ϊ����û��ʵ��OnSize���������ȹر�
            GRS_THROW_IF_FAILED(pIDXGIFactory5->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
        }

        //5��ö�������������豸
        {//ѡ��NUMA�ܹ��Ķ���������3D�豸����,��ʱ�Ȳ�֧�ּ����ˣ���Ȼ������޸���Щ��Ϊ
            DXGI_ADAPTER_DESC1 desc = {};
            D3D12_FEATURE_DATA_ARCHITECTURE stArchitecture = {};
            for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pIDXGIFactory5->EnumAdapters1(adapterIndex, &pIAdapter); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc = {};
                pIAdapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {//������������������豸
                    continue;
                }

                GRS_THROW_IF_FAILED(D3D12CreateDevice(pIAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&pID3DDevice)));
                GRS_THROW_IF_FAILED(pID3DDevice->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE
                    , &stArchitecture, sizeof(D3D12_FEATURE_DATA_ARCHITECTURE)));

                if (!stArchitecture.UMA)
                {
                    break;
                }

                pID3DDevice.Reset();
            }

            //---------------------------------------------------------------------------------------------
            if (nullptr == pID3DDevice.Get())
            {// �����Ļ����Ͼ�Ȼû�ж��� �������˳����� 
                throw CGRSCOMException(E_FAIL);
            }
        }

        //6������ֱ���������
        {
            D3D12_COMMAND_QUEUE_DESC stQueueDesc = {};
            stQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            GRS_THROW_IF_FAILED(pID3DDevice->CreateCommandQueue(&stQueueDesc, IID_PPV_ARGS(&pICommandQueue)));
        }

        //7������ֱ�������б������
        {
            GRS_THROW_IF_FAILED(pID3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT
                , IID_PPV_ARGS(&pICmdAllocDirect)));
            //����ֱ�������б������Ͽ���ִ�м������е��������3Dͼ�����桢�������桢��������ȣ�
            //ע���ʼʱ��û��ʹ��PSO���󣬴�ʱ��ʵ��������б���Ȼ���Լ�¼����
            GRS_THROW_IF_FAILED(pID3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT
                , pICmdAllocDirect.Get(), nullptr, IID_PPV_ARGS(&pICmdListDirect)));

            GRS_THROW_IF_FAILED(pID3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE
                , IID_PPV_ARGS(&pICmdAllocEarth)));
            GRS_THROW_IF_FAILED(pID3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE
                , pICmdAllocEarth.Get(), nullptr, IID_PPV_ARGS(&pIBundlesEarth)));

            GRS_THROW_IF_FAILED(pID3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE
                , IID_PPV_ARGS(&pICmdAllocSkybox)));
            GRS_THROW_IF_FAILED(pID3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE
                , pICmdAllocSkybox.Get(), nullptr, IID_PPV_ARGS(&pIBundlesSkybox)));
        }

        //8������������
        {
            DXGI_SWAP_CHAIN_DESC1 stSwapChainDesc = {};
            stSwapChainDesc.BufferCount = nFrameBackBufCount;
            stSwapChainDesc.Width = iWndWidth;
            stSwapChainDesc.Height = iWndHeight;
            stSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            stSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            stSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            stSwapChainDesc.SampleDesc.Count = 1;

            GRS_THROW_IF_FAILED(pIDXGIFactory5->CreateSwapChainForHwnd(
                pICommandQueue.Get(),		// Swap chain needs the queue so that it can force a flush on it.
                hWnd,
                &stSwapChainDesc,
                nullptr,
                nullptr,
                &pISwapChain1
            ));

            //ע��˴�ʹ���˸߰汾��SwapChain�ӿڵĺ���
            GRS_THROW_IF_FAILED(pISwapChain1.As(&pISwapChain3));
            nCurrentFrameIndex = pISwapChain3->GetCurrentBackBufferIndex();

            //����RTV(��ȾĿ����ͼ)��������(����ѵĺ���Ӧ�����Ϊ������߹̶���СԪ�صĹ̶���С�Դ��)
            D3D12_DESCRIPTOR_HEAP_DESC stRTVHeapDesc = {};
            stRTVHeapDesc.NumDescriptors = nFrameBackBufCount;
            stRTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            stRTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            GRS_THROW_IF_FAILED(pID3DDevice->CreateDescriptorHeap(&stRTVHeapDesc, IID_PPV_ARGS(&pIRTVHeap)));
            //�õ�ÿ��������Ԫ�صĴ�С
            nRTVDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

            //---------------------------------------------------------------------------------------------
            CD3DX12_CPU_DESCRIPTOR_HANDLE stRTVHandle(pIRTVHeap->GetCPUDescriptorHandleForHeapStart());
            for (UINT i = 0; i < nFrameBackBufCount; i++)
            {//���ѭ����©����������ʵ�����Ǹ�����ı���
                GRS_THROW_IF_FAILED(pISwapChain3->GetBuffer(i, IID_PPV_ARGS(&pIARenderTargets[i])));
                pID3DDevice->CreateRenderTargetView(pIARenderTargets[i].Get(), nullptr, stRTVHandle);
                stRTVHandle.Offset(1, nRTVDescriptorSize);
            }


        }

        //9��������Ȼ��弰��Ȼ�����������
        {
            D3D12_DEPTH_STENCIL_VIEW_DESC stDepthStencilDesc = {};
            stDepthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
            stDepthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            stDepthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

            D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
            depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
            depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
            depthOptimizedClearValue.DepthStencil.Stencil = 0;

            //ʹ����ʽĬ�϶Ѵ���һ��������建������
            //��Ϊ��������Ȼ�������һֱ��ʹ�ã����õ����岻������ֱ��ʹ����ʽ�ѣ�ͼ����
            GRS_THROW_IF_FAILED(pID3DDevice->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
                , D3D12_HEAP_FLAG_NONE
                , &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT
                    , iWndWidth, iWndHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
                , D3D12_RESOURCE_STATE_DEPTH_WRITE
                , &depthOptimizedClearValue
                , IID_PPV_ARGS(&pIDepthStencilBuffer)
            ));

            //==============================================================================================
            D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
            dsvHeapDesc.NumDescriptors = 1;
            dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            GRS_THROW_IF_FAILED(pID3DDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&pIDSVHeap)));

            pID3DDevice->CreateDepthStencilView(pIDepthStencilBuffer.Get()
                , &stDepthStencilDesc
                , pIDSVHeap->GetCPUDescriptorHandleForHeapStart());
        }

        //9������ SRV CBV Sample��
        {
            //���ǽ�������ͼ��������CBV����������һ������������
            D3D12_DESCRIPTOR_HEAP_DESC stSRVHeapDesc = {};
            stSRVHeapDesc.NumDescriptors = 2; //1 SRV + 1 CBV
            stSRVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            stSRVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

            GRS_THROW_IF_FAILED(pID3DDevice->CreateDescriptorHeap(&stSRVHeapDesc, IID_PPV_ARGS(&pISRVHpEarth)));

            D3D12_DESCRIPTOR_HEAP_DESC stSamplerHeapDesc = {};
            stSamplerHeapDesc.NumDescriptors = nSampleMaxCnt;
            stSamplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            stSamplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

            GRS_THROW_IF_FAILED(pID3DDevice->CreateDescriptorHeap(&stSamplerHeapDesc, IID_PPV_ARGS(&pISampleHpEarth)));


            //===================================================================================================
            //Skybox �� SRV CBV Sample ��
            GRS_THROW_IF_FAILED(pID3DDevice->CreateDescriptorHeap(&stSRVHeapDesc, IID_PPV_ARGS(&pISRVHpSkybox)));
            stSamplerHeapDesc.NumDescriptors = 1; //��պ��Ӿ�һ��������
            GRS_THROW_IF_FAILED(pID3DDevice->CreateDescriptorHeap(&stSamplerHeapDesc, IID_PPV_ARGS(&pISampleHpSkybox)));

            nSamplerDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        }

        //10��������ǩ��
        {//��������У������Skyboxʹ����ͬ�ĸ�ǩ������Ϊ��Ⱦ��������Ҫ�Ĳ�����һ����
            D3D12_FEATURE_DATA_ROOT_SIGNATURE stFeatureData = {};
            // ����Ƿ�֧��V1.1�汾�ĸ�ǩ��
            stFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
            if (FAILED(pID3DDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &stFeatureData, sizeof(stFeatureData))))
            {
                stFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
            }
            // ��GPU��ִ��SetGraphicsRootDescriptorTable�����ǲ��޸������б��е�SRV��������ǿ���ʹ��Ĭ��Rang��Ϊ:
            // D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE
            CD3DX12_DESCRIPTOR_RANGE1 stDSPRanges[3];
            stDSPRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
            stDSPRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
            stDSPRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

            CD3DX12_ROOT_PARAMETER1 stRootParameters[3];
            stRootParameters[0].InitAsDescriptorTable(1, &stDSPRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);//SRV��PS�ɼ�
            stRootParameters[1].InitAsDescriptorTable(1, &stDSPRanges[1], D3D12_SHADER_VISIBILITY_ALL); //CBV������Shader�ɼ�
            stRootParameters[2].InitAsDescriptorTable(1, &stDSPRanges[2], D3D12_SHADER_VISIBILITY_PIXEL);//SAMPLE��PS�ɼ�

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC stRootSignatureDesc;

            stRootSignatureDesc.Init_1_1(_countof(stRootParameters), stRootParameters
                , 0, nullptr
                , D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

            ComPtr<ID3DBlob> pISignatureBlob;
            ComPtr<ID3DBlob> pIErrorBlob;
            GRS_THROW_IF_FAILED(D3DX12SerializeVersionedRootSignature(&stRootSignatureDesc
                , stFeatureData.HighestVersion
                , &pISignatureBlob
                , &pIErrorBlob));

            GRS_THROW_IF_FAILED(pID3DDevice->CreateRootSignature(0
                , pISignatureBlob->GetBufferPointer()
                , pISignatureBlob->GetBufferSize()
                , IID_PPV_ARGS(&pIRootSignature)));


        }

        //11������Shader������Ⱦ����״̬����
        {

#if defined(_DEBUG)
            // Enable better shader debugging with the graphics debugging tools.
            UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
            UINT compileFlags = 0;
#endif
            //����Ϊ�о�����ʽ	   
            compileFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

            TCHAR pszShaderFileName[] = _T("D:\\Projects_2018_08\\D3D12 Tutorials\\5-SkyBox\\Shader\\TextureCube.hlsl");

            GRS_THROW_IF_FAILED(D3DCompileFromFile(pszShaderFileName, nullptr, nullptr
                , "VSMain", "vs_5_0", compileFlags, 0, &pIVSEarth, nullptr));
            GRS_THROW_IF_FAILED(D3DCompileFromFile(pszShaderFileName, nullptr, nullptr
                , "PSMain", "ps_5_0", compileFlags, 0, &pIPSEarth, nullptr));

            // ���Ƕ������һ�����ߵĶ��壬��ĿǰShader�����ǲ�û��ʹ��
            D3D12_INPUT_ELEMENT_DESC stIALayoutEarth[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };

            // ���� graphics pipeline state object (PSO)����
            D3D12_GRAPHICS_PIPELINE_STATE_DESC stPSODesc = {};
            stPSODesc.InputLayout = { stIALayoutEarth, _countof(stIALayoutEarth) };
            stPSODesc.pRootSignature = pIRootSignature.Get();
            stPSODesc.VS = CD3DX12_SHADER_BYTECODE(pIVSEarth.Get());
            stPSODesc.PS = CD3DX12_SHADER_BYTECODE(pIPSEarth.Get());
            stPSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            stPSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            stPSODesc.DepthStencilState.DepthEnable = FALSE;
            stPSODesc.DepthStencilState.StencilEnable = FALSE;
            stPSODesc.SampleMask = UINT_MAX;
            stPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            stPSODesc.NumRenderTargets = 1;
            stPSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            stPSODesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
            stPSODesc.DepthStencilState.DepthEnable = TRUE;
            stPSODesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//������Ȼ���д�빦��
            stPSODesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;     //��Ȳ��Ժ�������ֵΪ��ͨ����Ȳ��ԣ�
            stPSODesc.SampleDesc.Count = 1;

            GRS_THROW_IF_FAILED(pID3DDevice->CreateGraphicsPipelineState(&stPSODesc
                , IID_PPV_ARGS(&pIPSOEarth)));


            //����Ϊ�о�����ʽ	   
            compileFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

            TCHAR pszSMFileSkybox[] = _T("D:\\Projects_2018_08\\D3D12 Tutorials\\5-SkyBox\\Shader\\SkyBox.hlsl");

            GRS_THROW_IF_FAILED(D3DCompileFromFile(pszSMFileSkybox, nullptr, nullptr
                , "SkyboxVS", "vs_5_0", compileFlags, 0, &pIVSSkybox, nullptr));
            GRS_THROW_IF_FAILED(D3DCompileFromFile(pszSMFileSkybox, nullptr, nullptr
                , "SkyboxPS", "ps_5_0", compileFlags, 0, &pIPSSkybox, nullptr));

            // ��պ���ֻ�ж���ֻ��λ�ò���
            D3D12_INPUT_ELEMENT_DESC stIALayoutSkybox[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };

            // ����Skybox��(PSO)���� ע����պ��Ӳ���Ҫ��Ȳ���
            stPSODesc.DepthStencilState.DepthEnable = FALSE;
            stPSODesc.DepthStencilState.StencilEnable = FALSE;
            stPSODesc.InputLayout = { stIALayoutSkybox, _countof(stIALayoutSkybox) };
            stPSODesc.VS = CD3DX12_SHADER_BYTECODE(pIVSSkybox.Get());
            stPSODesc.PS = CD3DX12_SHADER_BYTECODE(pIPSSkybox.Get());

            GRS_THROW_IF_FAILED(pID3DDevice->CreateGraphicsPipelineState(&stPSODesc
                , IID_PPV_ARGS(&pIPSOSkyBox)));

        }

        //12��ʹ��DDSLoader������������Skybox������
        {
            TCHAR pszSkyboxTextureFile[] = _T("D:\\Projects_2018_08\\D3D12 Tutorials\\5-SkyBox\\Texture\\Texture1.dds");

            //HRESULT DirectX::LoadDDSTextureFromFile(
            //	ID3D12Device* d3dDevice,
            //	const wchar_t* fileName,
            //	ID3D12Resource** texture,
            //	std::unique_ptr<uint8_t[]>& ddsData,
            //	std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
            //	size_t maxsize,
            //	DDS_ALPHA_MODE* alphaMode,
            //	bool* isCubeMap);

            ID3D12Resource* pIResSkyBox = nullptr;
            GRS_THROW_IF_FAILED(LoadDDSTextureFromFile(
                pID3DDevice.Get()
                , pszSkyboxTextureFile
                , &pIResSkyBox
                , ddsData
                , subresources
                , SIZE_MAX
                , &emAlphaMode
                , &bIsCube));

            //���溯�����ص���������ʽĬ�϶��ϣ�����Copy������Ҫ�����Լ����
            pITxtSkybox.Attach(pIResSkyBox);
        }

        //13�����������Ĭ�϶�
        {
            D3D12_HEAP_DESC stTextureHeapDesc = {};
            //Ϊ��ָ������ͼƬ����2����С�Ŀռ䣬����û����ϸȥ�����ˣ�ֻ��ָ����һ���㹻��Ŀռ䣬�����������
            //ʵ��Ӧ����Ҳ��Ҫ�ۺϿ��Ƿ���ѵĴ�С���Ա�������ö�
            stTextureHeapDesc.SizeInBytes = GRS_UPPER(2 * nRowPitchEarth * nTxtHEarth, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
            //ָ���ѵĶ��뷽ʽ������ʹ����Ĭ�ϵ�64K�߽���룬��Ϊ������ʱ����ҪMSAA֧��
            stTextureHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            stTextureHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;		//Ĭ�϶�����
            stTextureHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            stTextureHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            //�ܾ���ȾĿ�������ܾ������������ʵ�ʾ�ֻ�������ڷ���ͨ����
            stTextureHeapDesc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_BUFFERS;

            GRS_THROW_IF_FAILED(pID3DDevice->CreateHeap(&stTextureHeapDesc, IID_PPV_ARGS(&pITxtHpEarth)));

        }

        //14������2D����
        {
            stTextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            stTextureDesc.MipLevels = 1;
            stTextureDesc.Format = emTxtFmtEarth; //DXGI_FORMAT_R8G8B8A8_UNORM;
            stTextureDesc.Width = nTxtWEarth;
            stTextureDesc.Height = nTxtHEarth;
            stTextureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            stTextureDesc.DepthOrArraySize = 1;
            stTextureDesc.SampleDesc.Count = 1;
            stTextureDesc.SampleDesc.Quality = 0;
            //-----------------------------------------------------------------------------------------------------------
            //ʹ�á���λ��ʽ������������ע��������������ڲ�ʵ���Ѿ�û�д洢������ͷŵ�ʵ�ʲ����ˣ��������ܸܺ�
            //ͬʱ������������Ϸ�������CreatePlacedResource��������ͬ��������Ȼǰ�������ǲ��ڱ�ʹ�õ�ʱ�򣬲ſ���
            //���ö�
            GRS_THROW_IF_FAILED(pID3DDevice->CreatePlacedResource(
                pITxtHpEarth.Get()
                , 0
                , &stTextureDesc				//����ʹ��CD3DX12_RESOURCE_DESC::Tex2D���򻯽ṹ��ĳ�ʼ��
                , D3D12_RESOURCE_STATE_COPY_DEST
                , nullptr
                , IID_PPV_ARGS(&pITxtEarth)));
            //-----------------------------------------------------------------------------------------------------------
            //��ȡ�ϴ�����Դ����Ĵ�С������ߴ�ͨ������ʵ��ͼƬ�ĳߴ�
            n64szUploadBufEarth = GetRequiredIntermediateSize(pITxtEarth.Get(), 0, 1);
        }

        //15�������ϴ���
        {
            //-----------------------------------------------------------------------------------------------------------
            D3D12_HEAP_DESC stUploadHeapDesc = {  };
            //�ߴ���Ȼ��ʵ���������ݴ�С��2����64K�߽�����С
            stUploadHeapDesc.SizeInBytes = GRS_UPPER(2 * n64szUploadBufEarth, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
            //ע���ϴ��ѿ϶���Buffer���ͣ����Բ�ָ�����뷽ʽ����Ĭ����64k�߽����
            stUploadHeapDesc.Alignment = 0;
            stUploadHeapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;		//�ϴ�������
            stUploadHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            stUploadHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            //�ϴ��Ѿ��ǻ��壬���԰ڷ���������
            stUploadHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;

            GRS_THROW_IF_FAILED(pID3DDevice->CreateHeap(&stUploadHeapDesc, IID_PPV_ARGS(&pIUploadHpEarth)));

            //-----------------------------------------------------------------------------------------------------------
            //=====================================================================================================
            //��ȡSkybox����Դ��С���������ϴ���
            n64szUploadBufSkybox = GetRequiredIntermediateSize(pITxtSkybox.Get(), 0, static_cast<UINT>(subresources.size()));
            stUploadHeapDesc.SizeInBytes = GRS_UPPER(2 * n64szUploadBufSkybox, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
            GRS_THROW_IF_FAILED(pID3DDevice->CreateHeap(&stUploadHeapDesc, IID_PPV_ARGS(&pIUploadHpSkybox)));
        }

        //16��ʹ�á���λ��ʽ�����������ϴ��������ݵĻ�����Դ
        {
            GRS_THROW_IF_FAILED(pID3DDevice->CreatePlacedResource(pIUploadHpEarth.Get()
                , 0
                , &CD3DX12_RESOURCE_DESC::Buffer(n64szUploadBufEarth)
                , D3D12_RESOURCE_STATE_GENERIC_READ
                , nullptr
                , IID_PPV_ARGS(&pITxtUpEarth)));

            GRS_THROW_IF_FAILED(pID3DDevice->CreatePlacedResource(pIUploadHpSkybox.Get()
                , 0
                , &CD3DX12_RESOURCE_DESC::Buffer(n64szUploadBufSkybox)
                , D3D12_RESOURCE_STATE_GENERIC_READ
                , nullptr
                , IID_PPV_ARGS(&pITxtUpSkybox)));
        }

        //17������ͼƬ�������ϴ��ѣ�����ɵ�һ��Copy��������memcpy������֪������CPU��ɵ�
        {
            //������Դ�����С������ʵ��ͼƬ���ݴ洢���ڴ��С
            void* pbPicData = ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, n64szUploadBufEarth);
            if (nullptr == pbPicData)
            {
                throw CGRSCOMException(HRESULT_FROM_WIN32(GetLastError()));
            }

            //��ͼƬ�ж�ȡ������
            GRS_THROW_IF_FAILED(pIBMPEarth->CopyPixels(nullptr
                , nRowPitchEarth
                , static_cast<UINT>(nRowPitchEarth * nTxtHEarth)   //ע���������ͼƬ������ʵ�Ĵ�С�����ֵͨ��С�ڻ���Ĵ�С
                , reinterpret_cast<BYTE*>(pbPicData)));

            //{//������δ�������DX12��ʾ����ֱ��ͨ����仺�������һ���ڰ׷��������
            // //��ԭ��δ��룬Ȼ��ע�������CopyPixels���ÿ��Կ����ڰ׷��������Ч��
            //	const UINT rowPitch = nRowPitchEarth; //nTxtWEarth * 4; //static_cast<UINT>(n64szUploadBufEarth / nTxtHEarth);
            //	const UINT cellPitch = rowPitch >> 3;		// The width of a cell in the checkboard texture.
            //	const UINT cellHeight = nTxtWEarth >> 3;	// The height of a cell in the checkerboard texture.
            //	const UINT textureSize = static_cast<UINT>(n64szUploadBufEarth);
            //	UINT nTexturePixelSize = static_cast<UINT>(n64szUploadBufEarth / nTxtHEarth / nTxtWEarth);

            //	UINT8* pData = reinterpret_cast<UINT8*>(pbPicData);

            //	for (UINT n = 0; n < textureSize; n += nTexturePixelSize)
            //	{
            //		UINT x = n % rowPitch;
            //		UINT y = n / rowPitch;
            //		UINT i = x / cellPitch;
            //		UINT j = y / cellHeight;

            //		if (i % 2 == j % 2)
            //		{
            //			pData[n] = 0x00;		// R
            //			pData[n + 1] = 0x00;	// G
            //			pData[n + 2] = 0x00;	// B
            //			pData[n + 3] = 0xff;	// A
            //		}
            //		else
            //		{
            //			pData[n] = 0xff;		// R
            //			pData[n + 1] = 0xff;	// G
            //			pData[n + 2] = 0xff;	// B
            //			pData[n + 3] = 0xff;	// A
            //		}
            //	}
            //}

            //��ȡ���ϴ��ѿ����������ݵ�һЩ����ת���ߴ���Ϣ
            //���ڸ��ӵ�DDS�������Ƿǳ���Ҫ�Ĺ���

            UINT   nNumSubresources = 1u;  //����ֻ��һ��ͼƬ��������Դ����Ϊ1
            UINT   nTextureRowNum = 0u;
            UINT64 n64TextureRowSizes = 0u;
            UINT64 n64RequiredSize = 0u;

            stDestDesc = pITxtEarth->GetDesc();

            pID3DDevice->GetCopyableFootprints(&stDestDesc
                , 0
                , nNumSubresources
                , 0
                , &stTxtLayoutsEarth
                , &nTextureRowNum
                , &n64TextureRowSizes
                , &n64RequiredSize);

            //��Ϊ�ϴ���ʵ�ʾ���CPU�������ݵ�GPU���н�
            //�������ǿ���ʹ����Ϥ��Map����������ӳ�䵽CPU�ڴ��ַ��
            //Ȼ�����ǰ��н����ݸ��Ƶ��ϴ�����
            //��Ҫע�����֮���԰��п�������ΪGPU��Դ���д�С
            //��ʵ��ͼƬ���д�С���в����,���ߵ��ڴ�߽����Ҫ���ǲ�һ����
            BYTE* pData = nullptr;
            GRS_THROW_IF_FAILED(pITxtUpEarth->Map(0, NULL, reinterpret_cast<void**>(&pData)));

            BYTE* pDestSlice = reinterpret_cast<BYTE*>(pData) + stTxtLayoutsEarth.Offset;
            BYTE* pSrcSlice = reinterpret_cast<BYTE*>(pbPicData);
            for (UINT y = 0; y < nTextureRowNum; ++y)
            {
                memcpy(pDestSlice + static_cast<SIZE_T>(stTxtLayoutsEarth.Footprint.RowPitch) * y
                    , pSrcSlice + static_cast<SIZE_T>(nRowPitchEarth) * y
                    , nRowPitchEarth);
            }
            //ȡ��ӳ�� �����ױ��������ÿ֡�ı任��������ݣ�������������Unmap�ˣ�
            //������פ�ڴ�,������������ܣ���Ϊÿ��Map��Unmap�Ǻܺ�ʱ�Ĳ���
            //��Ϊ�������붼��64λϵͳ��Ӧ���ˣ���ַ�ռ����㹻�ģ�������ռ�ò���Ӱ��ʲô
            pITxtUpEarth->Unmap(0, NULL);

            //�ͷ�ͼƬ���ݣ���һ���ɾ��ĳ���Ա
            ::HeapFree(::GetProcessHeap(), 0, pbPicData);
        }

        //18���ϴ�Skybox������
        {
            UpdateSubresources(pICmdListDirect.Get()
                , pITxtSkybox.Get()
                , pITxtUpSkybox.Get()
                , 0
                , 0
                , static_cast<UINT>(subresources.size())
                , subresources.data());

            pICmdListDirect->ResourceBarrier(1
                , &CD3DX12_RESOURCE_BARRIER::Transition(pITxtSkybox.Get()
                    , D3D12_RESOURCE_STATE_COPY_DEST
                    , D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

        }

        //19����ֱ�������б������ϴ��Ѹ����������ݵ�Ĭ�϶ѵ����ִ�в�ͬ���ȴ�������ɵڶ���Copy��������GPU�ϵĸ����������
        //ע���ʱֱ�������б�û�а�PSO���������Ҳ�ǲ���ִ��3Dͼ������ģ����ǿ���ִ�и��������Ϊ�������治��Ҫʲô
        //�����״̬����֮��Ĳ���
        {
            CD3DX12_TEXTURE_COPY_LOCATION Dst(pITxtEarth.Get(), 0);
            CD3DX12_TEXTURE_COPY_LOCATION Src(pITxtUpEarth.Get(), stTxtLayoutsEarth);
            pICmdListDirect->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);

            //����һ����Դ���ϣ�ͬ����ȷ�ϸ��Ʋ������
            //ֱ��ʹ�ýṹ��Ȼ����õ���ʽ
            D3D12_RESOURCE_BARRIER stResBar = {};
            stResBar.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            stResBar.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            stResBar.Transition.pResource = pITxtEarth.Get();
            stResBar.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
            stResBar.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            stResBar.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

            pICmdListDirect->ResourceBarrier(1, &stResBar);

        }

        //20��ִ�еڶ���Copy���ȷ�����е������ϴ�����Ĭ�϶���
        {
            //---------------------------------------------------------------------------------------------
            // ִ�������б��ȴ�������Դ�ϴ���ɣ���һ���Ǳ����
            GRS_THROW_IF_FAILED(pICmdListDirect->Close());
            ID3D12CommandList* ppCommandLists[] = { pICmdListDirect.Get() };
            pICommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

            //---------------------------------------------------------------------------------------------
            // 17������һ��ͬ�����󡪡�Χ�������ڵȴ���Ⱦ��ɣ���Ϊ����Draw Call���첽����
            GRS_THROW_IF_FAILED(pID3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pIFence)));
            n64FenceValue = 1;

            //---------------------------------------------------------------------------------------------
            // 18������һ��Eventͬ���������ڵȴ�Χ���¼�֪ͨ
            hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (hFenceEvent == nullptr)
            {
                GRS_THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
            }

            //---------------------------------------------------------------------------------------------
            // 19���ȴ�������Դ��ʽ���������
            const UINT64 fence = n64FenceValue;
            GRS_THROW_IF_FAILED(pICommandQueue->Signal(pIFence.Get(), fence));
            n64FenceValue++;

            //---------------------------------------------------------------------------------------------
            // ��������û������ִ�е�Χ����ǵ����û�о������¼�ȥ�ȴ���ע��ʹ�õ���������ж����ָ��
            if (pIFence->GetCompletedValue() < fence)
            {
                GRS_THROW_IF_FAILED(pIFence->SetEventOnCompletion(fence, hFenceEvent));
                WaitForSingleObject(hFenceEvent, INFINITE);
            }

            //---------------------------------------------------------------------------------------------
            //�����������Resetһ�£��ղ��Ѿ�ִ�й���һ���������������
            GRS_THROW_IF_FAILED(pICmdAllocDirect->Reset());
            //Reset�����б�������ָ�������������PSO����
            GRS_THROW_IF_FAILED(pICmdListDirect->Reset(pICmdAllocDirect.Get(), pIPSOEarth.Get()));
            //---------------------------------------------------------------------------------------------

        }

        //21������Sky��3D���ݽṹ
        {
            float fHighW = -1.0f - (1.0f / (float)iWndWidth);
            float fHighH = -1.0f - (1.0f / (float)iWndHeight);
            float fLowW = 1.0f + (1.0f / (float)iWndWidth);
            float fLowH = 1.0f + (1.0f / (float)iWndHeight);

            stSkyboxVertices[0].m_vPos = XMFLOAT4(fLowW, fLowH, 1.0f, 1.0f);
            stSkyboxVertices[1].m_vPos = XMFLOAT4(fLowW, fHighH, 1.0f, 1.0f);
            stSkyboxVertices[2].m_vPos = XMFLOAT4(fHighW, fLowH, 1.0f, 1.0f);
            stSkyboxVertices[3].m_vPos = XMFLOAT4(fHighW, fHighH, 1.0f, 1.0f);
        }

        //22�������������������
        {
            std::ifstream fin;
            char input;

            fin.open("D:\\Projects_2018_08\\D3D12 Tutorials\\5-SkyBox\\Mesh\\sphere.txt");
            if (fin.fail())
            {
                throw CGRSCOMException(E_FAIL);
            }
            fin.get(input);
            while (input != ':')
            {
                fin.get(input);
            }
            fin >> nSphereVertexCnt;
            nSphereIndexCnt = nSphereVertexCnt;
            fin.get(input);
            while (input != ':')
            {
                fin.get(input);
            }
            fin.get(input);
            fin.get(input);

            pstSphereVertices = (ST_GRS_VERTEX*)HeapAlloc(::GetProcessHeap()
                , HEAP_ZERO_MEMORY
                , nSphereVertexCnt * sizeof(ST_GRS_VERTEX));
            pSphereIndices = (UINT*)HeapAlloc(::GetProcessHeap()
                , HEAP_ZERO_MEMORY
                , nSphereVertexCnt * sizeof(ST_GRS_VERTEX));

            for (UINT i = 0; i < nSphereVertexCnt; i++)
            {
                fin >> pstSphereVertices[i].m_vPos.x >> pstSphereVertices[i].m_vPos.y >> pstSphereVertices[i].m_vPos.z;
                fin >> pstSphereVertices[i].m_vTex.x >> pstSphereVertices[i].m_vTex.y;

                //pstSphereVertices[i].m_vTex.x = fabs(pstSphereVertices[i].m_vTex.x);
                //pstSphereVertices[i].m_vTex.x *= 0.5f;

                fin >> pstSphereVertices[i].m_vNor.x >> pstSphereVertices[i].m_vNor.y >> pstSphereVertices[i].m_vNor.z;

                pSphereIndices[i] = i;
            }
        }

        //23��ʹ�á���λ��ʽ���������㻺����������壬ʹ�����ϴ��������ݻ�����ͬ��һ���ϴ���
        {
            //---------------------------------------------------------------------------------------------
            //ʹ�ö�λ��ʽ����ͬ���ϴ������ԡ���λ��ʽ���������㻺�壬ע��ڶ�������ָ���˶��е�ƫ��λ��
            //���նѱ߽�����Ҫ������������ƫ��λ�ö��뵽��64k�ı߽���
            UINT64 n64BufferOffset = GRS_UPPER(n64szUploadBufEarth, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
            GRS_THROW_IF_FAILED(pID3DDevice->CreatePlacedResource(
                pIUploadHpEarth.Get()
                , n64BufferOffset
                , &CD3DX12_RESOURCE_DESC::Buffer(nSphereVertexCnt * sizeof(ST_GRS_VERTEX))
                , D3D12_RESOURCE_STATE_GENERIC_READ
                , nullptr
                , IID_PPV_ARGS(&pIVBEarth)));

            //ʹ��map-memcpy-unmap�󷨽����ݴ������㻺�����
            //ע�ⶥ�㻺��ʹ���Ǻ��ϴ��������ݻ�����ͬ��һ���ѣ��������
            UINT8* pVertexDataBegin = nullptr;
            CD3DX12_RANGE stReadRange(0, 0);		// We do not intend to read from this resource on the CPU.

            GRS_THROW_IF_FAILED(pIVBEarth->Map(0, &stReadRange, reinterpret_cast<void**>(&pVertexDataBegin)));
            memcpy(pVertexDataBegin, pstSphereVertices, nSphereVertexCnt * sizeof(ST_GRS_VERTEX));
            pIVBEarth->Unmap(0, nullptr);

            //������Դ��ͼ��ʵ�ʿ��Լ����Ϊָ�򶥵㻺����Դ�ָ��
            stVBVEarth.BufferLocation = pIVBEarth->GetGPUVirtualAddress();
            stVBVEarth.StrideInBytes = sizeof(ST_GRS_VERTEX);
            stVBVEarth.SizeInBytes = nSphereVertexCnt * sizeof(ST_GRS_VERTEX);

            //����߽�������ȷ��ƫ��λ��
            n64BufferOffset = GRS_UPPER(n64BufferOffset + nSphereVertexCnt * sizeof(ST_GRS_VERTEX), D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

            GRS_THROW_IF_FAILED(pID3DDevice->CreatePlacedResource(
                pIUploadHpEarth.Get()
                , n64BufferOffset
                , &CD3DX12_RESOURCE_DESC::Buffer(nSphereIndexCnt * sizeof(UINT))
                , D3D12_RESOURCE_STATE_GENERIC_READ
                , nullptr
                , IID_PPV_ARGS(&pIIBEarth)));

            UINT8* pIndexDataBegin = nullptr;
            GRS_THROW_IF_FAILED(pIIBEarth->Map(0, &stReadRange, reinterpret_cast<void**>(&pIndexDataBegin)));
            memcpy(pIndexDataBegin, pSphereIndices, nSphereIndexCnt * sizeof(UINT));
            pIIBEarth->Unmap(0, nullptr);

            stIBVEarth.BufferLocation = pIIBEarth->GetGPUVirtualAddress();
            stIBVEarth.Format = DXGI_FORMAT_R32_UINT;
            stIBVEarth.SizeInBytes = nSphereIndexCnt * sizeof(UINT);

            n64BufferOffset = GRS_UPPER(n64BufferOffset + nSphereIndexCnt * sizeof(UINT), D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

            // ������������ ע�⻺��ߴ�����Ϊ256�߽�����С
            GRS_THROW_IF_FAILED(pID3DDevice->CreatePlacedResource(
                pIUploadHpEarth.Get()
                , n64BufferOffset
                , &CD3DX12_RESOURCE_DESC::Buffer(szMVPBuf)
                , D3D12_RESOURCE_STATE_GENERIC_READ
                , nullptr
                , IID_PPV_ARGS(&pICBVUpEarth)));

            // Map ֮��Ͳ���Unmap�� ֱ�Ӹ������ݽ�ȥ ����ÿ֡������map-copy-unmap�˷�ʱ����
            GRS_THROW_IF_FAILED(pICBVUpEarth->Map(0, nullptr, reinterpret_cast<void**>(&pMVPBufEarth)));
            //---------------------------------------------------------------------------------------------

            //---------------------------------------------------------------------------------------------
            //������պ��ӵ�����
            n64BufferOffset = GRS_UPPER(n64szUploadBufSkybox, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
            GRS_THROW_IF_FAILED(pID3DDevice->CreatePlacedResource(
                pIUploadHpSkybox.Get()
                , n64BufferOffset
                , &CD3DX12_RESOURCE_DESC::Buffer(nSkyboxIndexCnt * sizeof(ST_GRS_SKYBOX_VERTEX))
                , D3D12_RESOURCE_STATE_GENERIC_READ
                , nullptr
                , IID_PPV_ARGS(&pIVBSkybox)));

            //ʹ��map-memcpy-unmap�󷨽����ݴ������㻺�����
            //ע�ⶥ�㻺��ʹ���Ǻ��ϴ��������ݻ�����ͬ��һ���ѣ��������
            pVertexDataBegin = nullptr;

            GRS_THROW_IF_FAILED(pIVBSkybox->Map(0, &stReadRange, reinterpret_cast<void**>(&pVertexDataBegin)));
            memcpy(pVertexDataBegin, &stSkyboxVertices, nSkyboxIndexCnt * sizeof(ST_GRS_SKYBOX_VERTEX));
            pIVBSkybox->Unmap(0, nullptr);

            //������Դ��ͼ��ʵ�ʿ��Լ����Ϊָ�򶥵㻺����Դ�ָ��
            stVBVSkybox.BufferLocation = pIVBSkybox->GetGPUVirtualAddress();
            stVBVSkybox.StrideInBytes = sizeof(ST_GRS_SKYBOX_VERTEX);
            stVBVSkybox.SizeInBytes = nSkyboxIndexCnt * sizeof(ST_GRS_SKYBOX_VERTEX);

            n64BufferOffset = GRS_UPPER(n64BufferOffset + nSkyboxIndexCnt * sizeof(ST_GRS_SKYBOX_VERTEX), D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

            // ������������ ע�⻺��ߴ�����Ϊ256�߽�����С
            GRS_THROW_IF_FAILED(pID3DDevice->CreatePlacedResource(
                pIUploadHpSkybox.Get()
                , n64BufferOffset
                , &CD3DX12_RESOURCE_DESC::Buffer(szMVPBuf)
                , D3D12_RESOURCE_STATE_GENERIC_READ
                , nullptr
                , IID_PPV_ARGS(&pICBVUpSkybox)));

            // Map ֮��Ͳ���Unmap�� ֱ�Ӹ������ݽ�ȥ ����ÿ֡������map-copy-unmap�˷�ʱ����
            GRS_THROW_IF_FAILED(pICBVUpSkybox->Map(0, nullptr, reinterpret_cast<void**>(&pMVPBufSkybox)));
            //---------------------------------------------------------------------------------------------
        }

        //24������SRV������
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC stSRVDesc = {};
            stSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            stSRVDesc.Format = emTxtFmtEarth;
            stSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            stSRVDesc.Texture2D.MipLevels = 1;
            pID3DDevice->CreateShaderResourceView(pITxtEarth.Get(), &stSRVDesc, pISRVHpEarth->GetCPUDescriptorHandleForHeapStart());

            //---------------------------------------------------------------------------------------------
            D3D12_RESOURCE_DESC stDescSkybox = pITxtSkybox->GetDesc();
            stSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
            stSRVDesc.Format = stDescSkybox.Format;
            stSRVDesc.TextureCube.MipLevels = stDescSkybox.MipLevels;
            pID3DDevice->CreateShaderResourceView(pITxtSkybox.Get(), &stSRVDesc, pISRVHpSkybox->GetCPUDescriptorHandleForHeapStart());
        }

        //25������CBV������
        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = pICBVUpEarth->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = static_cast<UINT>(szMVPBuf);

            CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvHandle(pISRVHpEarth->GetCPUDescriptorHandleForHeapStart()
                , 1
                , pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

            pID3DDevice->CreateConstantBufferView(&cbvDesc, cbvSrvHandle);

            //---------------------------------------------------------------------------------------------
            cbvDesc.BufferLocation = pICBVUpSkybox->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = static_cast<UINT>(szMVPBuf);

            CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvHandleSkybox(pISRVHpSkybox->GetCPUDescriptorHandleForHeapStart()
                , 1
                , pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

            pID3DDevice->CreateConstantBufferView(&cbvDesc, cbvSrvHandleSkybox);
            //---------------------------------------------------------------------------------------------

        }

        //26���������ֲ�����
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE hSamplerHeap(pISampleHpEarth->GetCPUDescriptorHandleForHeapStart());

            D3D12_SAMPLER_DESC stSamplerDesc = {};
            stSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;

            stSamplerDesc.MinLOD = 0;
            stSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
            stSamplerDesc.MipLODBias = 0.0f;
            stSamplerDesc.MaxAnisotropy = 1;
            stSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

            // Sampler 1
            stSamplerDesc.BorderColor[0] = 1.0f;
            stSamplerDesc.BorderColor[1] = 0.0f;
            stSamplerDesc.BorderColor[2] = 1.0f;
            stSamplerDesc.BorderColor[3] = 1.0f;
            stSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
            stSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
            stSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
            pID3DDevice->CreateSampler(&stSamplerDesc, hSamplerHeap);

            hSamplerHeap.Offset(nSamplerDescriptorSize);

            // Sampler 2
            stSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            stSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            stSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            pID3DDevice->CreateSampler(&stSamplerDesc, hSamplerHeap);

            hSamplerHeap.Offset(nSamplerDescriptorSize);

            // Sampler 3
            stSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            stSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            stSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            pID3DDevice->CreateSampler(&stSamplerDesc, hSamplerHeap);

            hSamplerHeap.Offset(nSamplerDescriptorSize);

            // Sampler 4
            stSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
            stSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
            stSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
            pID3DDevice->CreateSampler(&stSamplerDesc, hSamplerHeap);

            hSamplerHeap.Offset(nSamplerDescriptorSize);

            // Sampler 5
            stSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
            stSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
            stSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
            pID3DDevice->CreateSampler(&stSamplerDesc, hSamplerHeap);

            //---------------------------------------------------------------------------------------------
            //����Skybox�Ĳ�����
            stSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;

            stSamplerDesc.MinLOD = 0;
            stSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
            stSamplerDesc.MipLODBias = 0.0f;
            stSamplerDesc.MaxAnisotropy = 1;
            stSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            stSamplerDesc.BorderColor[0] = 0.0f;
            stSamplerDesc.BorderColor[1] = 0.0f;
            stSamplerDesc.BorderColor[2] = 0.0f;
            stSamplerDesc.BorderColor[3] = 0.0f;
            stSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            stSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            stSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

            pID3DDevice->CreateSampler(&stSamplerDesc, pISampleHpSkybox->GetCPUDescriptorHandleForHeapStart());
            //---------------------------------------------------------------------------------------------

        }


        //27�����������¼�̻�������
        {
            //=================================================================================================
            //����������
            pIBundlesEarth->SetGraphicsRootSignature(pIRootSignature.Get());
            pIBundlesEarth->SetPipelineState(pIPSOEarth.Get());
            ID3D12DescriptorHeap* ppHeapsEarth[] = { pISRVHpEarth.Get(),pISampleHpEarth.Get() };
            pIBundlesEarth->SetDescriptorHeaps(_countof(ppHeapsEarth), ppHeapsEarth);
            //����SRV
            pIBundlesEarth->SetGraphicsRootDescriptorTable(0, pISRVHpEarth->GetGPUDescriptorHandleForHeapStart());

            CD3DX12_GPU_DESCRIPTOR_HANDLE stGPUCBVHandleEarth(pISRVHpEarth->GetGPUDescriptorHandleForHeapStart()
                , 1
                , pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
            //����CBV
            pIBundlesEarth->SetGraphicsRootDescriptorTable(1, stGPUCBVHandleEarth);
            CD3DX12_GPU_DESCRIPTOR_HANDLE hGPUSamplerEarth(pISampleHpEarth->GetGPUDescriptorHandleForHeapStart()
                , nCurrentSamplerNO
                , nSamplerDescriptorSize);
            //����Sample
            pIBundlesEarth->SetGraphicsRootDescriptorTable(2, hGPUSamplerEarth);
            //ע������ʹ�õ���Ⱦ�ַ����������б�Ҳ����ͨ����Mesh����
            pIBundlesEarth->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            pIBundlesEarth->IASetVertexBuffers(0, 1, &stVBVEarth);
            pIBundlesEarth->IASetIndexBuffer(&stIBVEarth);

            //Draw Call������
            pIBundlesEarth->DrawIndexedInstanced(nSphereIndexCnt, 1, 0, 0, 0);
            pIBundlesEarth->Close();
            //=================================================================================================

            //=================================================================================================
            //Skybox�������
            pIBundlesSkybox->SetPipelineState(pIPSOSkyBox.Get());
            pIBundlesSkybox->SetGraphicsRootSignature(pIRootSignature.Get());
            ID3D12DescriptorHeap* ppHeaps[] = { pISRVHpSkybox.Get(),pISampleHpSkybox.Get() };
            pIBundlesSkybox->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
            //����SRV
            pIBundlesSkybox->SetGraphicsRootDescriptorTable(0, pISRVHpSkybox->GetGPUDescriptorHandleForHeapStart());
            CD3DX12_GPU_DESCRIPTOR_HANDLE stGPUCBVHandleSkybox(pISRVHpSkybox->GetGPUDescriptorHandleForHeapStart()
                , 1
                , pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
            //����CBV
            pIBundlesSkybox->SetGraphicsRootDescriptorTable(1, stGPUCBVHandleSkybox);
            pIBundlesSkybox->SetGraphicsRootDescriptorTable(2, pISampleHpSkybox->GetGPUDescriptorHandleForHeapStart());
            pIBundlesSkybox->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            pIBundlesSkybox->IASetVertexBuffers(0, 1, &stVBVSkybox);

            //Draw Call������
            pIBundlesSkybox->DrawInstanced(_countof(stSkyboxVertices), 1, 0, 0);
            pIBundlesSkybox->Close();
            //=================================================================================================
        }

        //---------------------------------------------------------------------------------------------
        //28��������ʱ�������Ա��ڴ�����Ч����Ϣѭ��
        HANDLE phWait = CreateWaitableTimer(NULL, FALSE, NULL);
        LARGE_INTEGER liDueTime = {};
        liDueTime.QuadPart = -1i64;//1���ʼ��ʱ
        SetWaitableTimer(phWait, &liDueTime, 1, NULL, NULL, 0);//40ms������

        //29����¼֡��ʼʱ�䣬�͵�ǰʱ�䣬��ѭ������Ϊ��
        ULONGLONG n64tmFrameStart = ::GetTickCount64();
        ULONGLONG n64tmCurrent = n64tmFrameStart;
        //������ת�Ƕ���Ҫ�ı���
        double dModelRotationYAngle = 0.0f;

        //---------------------------------------------------------------------------------------------
        //30����ʼ��Ϣѭ�����������в�����Ⱦ
        DWORD dwRet = 0;
        BOOL bExit = FALSE;

        while (!bExit)
        {//ע���������ǵ�������Ϣѭ�������ȴ�ʱ������Ϊ0��ͬʱ����ʱ�Ե���Ⱦ���ĳ���ÿ��ѭ������Ⱦ
         //���ⲻ��ʾ˵MsgWait������ûɶ���ˣ����ʹ��������Ϊ������������������߳̿��ƾͷǳ�����
            dwRet = ::MsgWaitForMultipleObjects(1, &phWait, FALSE, 0, QS_ALLINPUT);
            switch (dwRet - WAIT_OBJECT_0)
            {
            case 0:
            case WAIT_TIMEOUT:
            {//��ʱ��ʱ�䵽

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
            default:
                break;
            }

            //GRS_TRACE(_T("��ʼ��%u֡��Ⱦ{Frame Index = %u}��\n"),nFrame,nCurrentFrameIndex);
            //��ʼ��¼����
            //---------------------------------------------------------------------------------------------
            // ׼��һ���򵥵���תMVP���� �÷���ת����
            {
                n64tmCurrent = ::GetTickCount();
                //������ת�ĽǶȣ���ת�Ƕ�(����) = ʱ��(��) * ���ٶ�(����/��)
                //�����������൱�ھ�����Ϸ��Ϣѭ���е�OnUpdate��������Ҫ��������
                dModelRotationYAngle += ((n64tmCurrent - n64tmFrameStart) / 1000.0f) * fPalstance;

                n64tmFrameStart = n64tmCurrent;

                //��ת�Ƕ���2PI���ڵı�����ȥ����������ֻ�������0���ȿ�ʼ��С��2PI�Ļ��ȼ���
                if (dModelRotationYAngle > XM_2PI)
                {
                    dModelRotationYAngle = fmod(dModelRotationYAngle, XM_2PI);
                }

                //���� �Ӿ��� view * �ü����� projection
                XMMATRIX xmMVP = XMMatrixMultiply(XMMatrixLookAtLH(XMLoadFloat3(&f3EyePos)
                    , XMLoadFloat3(&f3LockAt)
                    , XMLoadFloat3(&f3HeapUp))
                    , XMMatrixPerspectiveFovLH(XM_PIDIV4
                        , (FLOAT)iWndWidth / (FLOAT)iWndHeight, 0.1f, 1000.0f));

                //����Skybox��MVP
                XMStoreFloat4x4(&pMVPBufSkybox->m_MVP, xmMVP);

                //ģ�;��� model �����ǷŴ����ת
                XMMATRIX xmRot = XMMatrixMultiply(XMMatrixScaling(fSphereSize, fSphereSize, fSphereSize)
                    , XMMatrixRotationY(static_cast<float>(dModelRotationYAngle)));

                //���������MVP
                xmMVP = XMMatrixMultiply(xmRot, xmMVP);

                XMStoreFloat4x4(&pMVPBufEarth->m_MVP, xmMVP);
            }
            //---------------------------------------------------------------------------------------------
            // ͨ����Դ�����ж��󻺳��Ѿ��л���Ͽ��Կ�ʼ��Ⱦ��
            pICmdListDirect->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pIARenderTargets[nCurrentFrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

            //ƫ��������ָ�뵽ָ��֡������ͼλ��
            CD3DX12_CPU_DESCRIPTOR_HANDLE stRTVHandle(pIRTVHeap->GetCPUDescriptorHandleForHeapStart(), nCurrentFrameIndex, nRTVDescriptorSize);
            CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(pIDSVHeap->GetCPUDescriptorHandleForHeapStart());
            //������ȾĿ��
            pICmdListDirect->OMSetRenderTargets(1, &stRTVHandle, FALSE, &dsvHandle);
            //---------------------------------------------------------------------------------------------
            pICmdListDirect->RSSetViewports(1, &stViewPort);
            pICmdListDirect->RSSetScissorRects(1, &stScissorRect);

            //---------------------------------------------------------------------------------------------
            // ������¼�����������ʼ��һ֡����Ⱦ
            const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
            pICmdListDirect->ClearRenderTargetView(stRTVHandle, clearColor, 0, nullptr);
            pICmdListDirect->ClearDepthStencilView(pIDSVHeap->GetCPUDescriptorHandleForHeapStart()
                , D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

            //===============================================================================================
            //31��ִ��Skybox�������
            ID3D12DescriptorHeap* ppHeapsSkybox[] = { pISRVHpSkybox.Get(),pISampleHpSkybox.Get() };
            pICmdListDirect->SetDescriptorHeaps(_countof(ppHeapsSkybox), ppHeapsSkybox);
            pICmdListDirect->ExecuteBundle(pIBundlesSkybox.Get());
            //===============================================================================================

            //===============================================================================================
            //32��ִ������������
            ID3D12DescriptorHeap* ppHeapsEarth[] = { pISRVHpEarth.Get(),pISampleHpEarth.Get() };
            pICmdListDirect->SetDescriptorHeaps(_countof(ppHeapsEarth), ppHeapsEarth);
            pICmdListDirect->ExecuteBundle(pIBundlesEarth.Get());
            //===============================================================================================

            //---------------------------------------------------------------------------------------------
            //��һ����Դ���ϣ�����ȷ����Ⱦ�Ѿ����������ύ����ȥ��ʾ��
            pICmdListDirect->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pIARenderTargets[nCurrentFrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
            //�ر������б�����ȥִ����
            GRS_THROW_IF_FAILED(pICmdListDirect->Close());

            //---------------------------------------------------------------------------------------------
            //ִ�������б�
            ID3D12CommandList* ppCommandLists[] = { pICmdListDirect.Get() };
            pICommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

            //---------------------------------------------------------------------------------------------
            //�ύ����
            GRS_THROW_IF_FAILED(pISwapChain3->Present(1, 0));

            //---------------------------------------------------------------------------------------------
            //��ʼͬ��GPU��CPU��ִ�У��ȼ�¼Χ�����ֵ
            const UINT64 fence = n64FenceValue;
            GRS_THROW_IF_FAILED(pICommandQueue->Signal(pIFence.Get(), fence));
            n64FenceValue++;

            //---------------------------------------------------------------------------------------------
            // ��������û������ִ�е�Χ����ǵ����û�о������¼�ȥ�ȴ���ע��ʹ�õ���������ж����ָ��
            if (pIFence->GetCompletedValue() < fence)
            {
                GRS_THROW_IF_FAILED(pIFence->SetEventOnCompletion(fence, hFenceEvent));
                WaitForSingleObject(hFenceEvent, INFINITE);
            }
            //ִ�е�����˵��һ���������������ִ�����ˣ�������ʹ������ǵ�һ֡�Ѿ���Ⱦ���ˣ�����׼��ִ����һ֡��Ⱦ

            //---------------------------------------------------------------------------------------------
            //��ȡ�µĺ󻺳���ţ���ΪPresent�������ʱ�󻺳����ž͸�����
            nCurrentFrameIndex = pISwapChain3->GetCurrentBackBufferIndex();

            //---------------------------------------------------------------------------------------------
            //�����������Resetһ��
            GRS_THROW_IF_FAILED(pICmdAllocDirect->Reset());
            //Reset�����б�������ָ�������������PSO����
            GRS_THROW_IF_FAILED(pICmdListDirect->Reset(pICmdAllocDirect.Get(), pIPSOEarth.Get()));

            //GRS_TRACE(_T("��%u֡��Ⱦ����.\n"), nFrame++);
        }
        //::CoUninitialize();

        ::HeapFree(::GetProcessHeap(), 0, pstSphereVertices);
        ::HeapFree(::GetProcessHeap(), 0, pSphereIndices);
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
    case WM_KEYDOWN:
    {
        USHORT n16KeyCode = (wParam & 0xFF);
        if (VK_SPACE == n16KeyCode)
        {//���ո���л���ͬ�Ĳ�������Ч����������ÿ�ֲ���������ĺ���
            //UINT nCurrentSamplerNO = 0; //��ǰʹ�õĲ���������
            //UINT nSampleMaxCnt = 5;		//����������͵Ĳ�����
            ++nCurrentSamplerNO;
            nCurrentSamplerNO %= nSampleMaxCnt;

            //=================================================================================================
            //������������������
            //pIBundlesEarth->Reset(pICmdAllocEarth.Get(), pIPSOEarth.Get());
            //pIBundlesEarth->SetGraphicsRootSignature(pIRootSignature.Get());
            //pIBundlesEarth->SetPipelineState(pIPSOEarth.Get());
            //ID3D12DescriptorHeap* ppHeapsEarth[] = { pISRVHpEarth.Get(),pISampleHpEarth.Get() };
            //pIBundlesEarth->SetDescriptorHeaps(_countof(ppHeapsEarth), ppHeapsEarth);
//����SRV
                //pIBundlesEarth->SetGraphicsRootDescriptorTable(0, pISRVHpEarth->GetGPUDescriptorHandleForHeapStart());

                //CD3DX12_GPU_DESCRIPTOR_HANDLE stGPUCBVHandleEarth(pISRVHpEarth->GetGPUDescriptorHandleForHeapStart()
                //	, 1
                //	, pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
                //����CBV
                //pIBundlesEarth->SetGraphicsRootDescriptorTable(1, stGPUCBVHandleEarth);
                //CD3DX12_GPU_DESCRIPTOR_HANDLE hGPUSamplerEarth(pISampleHpEarth->GetGPUDescriptorHandleForHeapStart()
                //	, nCurrentSamplerNO
                //	, nSamplerDescriptorSize);
                //����Sample
                //pIBundlesEarth->SetGraphicsRootDescriptorTable(2, hGPUSamplerEarth);
                //ע������ʹ�õ���Ⱦ�ַ����������б�Ҳ����ͨ����Mesh����
                //pIBundlesEarth->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                //pIBundlesEarth->IASetVertexBuffers(0, 1, &stVBVEarth);
                //pIBundlesEarth->IASetIndexBuffer(&stIBVEarth);

                //Draw Call������
                //pIBundlesEarth->DrawIndexedInstanced(nSphereIndexCnt, 1, 0, 0, 0);
                //pIBundlesEarth->Close();
                //=================================================================================================
        }
        if (VK_ADD == n16KeyCode || VK_OEM_PLUS == n16KeyCode)
        {
            //double fPalstance = 10.0f * XM_PI / 180.0f;	//������ת�Ľ��ٶȣ���λ������/��
            fPalstance += 10 * XM_PI / 180.0f;
            if (fPalstance > XM_PI)
            {
                fPalstance = XM_PI;
            }
        }

        if (VK_SUBTRACT == n16KeyCode || VK_OEM_MINUS == n16KeyCode)
        {
            fPalstance -= 10 * XM_PI / 180.0f;
            if (fPalstance < 0.0f)
            {
                fPalstance = XM_PI / 180.0f;
            }
        }

        //�����û�����任
        //XMVECTOR f3EyePos = XMVectorSet(0.0f, 5.0f, -10.0f, 0.0f); //�۾�λ��
        //XMVECTOR f3LockAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);  //�۾�������λ��
        //XMVECTOR f3HeapUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);  //ͷ�����Ϸ�λ��
        XMFLOAT3 move(0, 0, 0);
        float fMoveSpeed = 2.0f;
        float fTurnSpeed = XM_PIDIV2 * 0.005f;

        if ('w' == n16KeyCode || 'W' == n16KeyCode)
        {
            move.z -= 1.0f;
        }

        if ('s' == n16KeyCode || 'S' == n16KeyCode)
        {
            move.z += 1.0f;
        }

        if ('d' == n16KeyCode || 'D' == n16KeyCode)
        {
            move.x += 1.0f;
        }

        if ('a' == n16KeyCode || 'A' == n16KeyCode)
        {
            move.x -= 1.0f;
        }

        if (fabs(move.x) > 0.1f && fabs(move.z) > 0.1f)
        {
            XMVECTOR vector = XMVector3Normalize(XMLoadFloat3(&move));
            move.x = XMVectorGetX(vector);
            move.z = XMVectorGetZ(vector);
        }

        if (VK_UP == n16KeyCode)
        {
            fPitch += fTurnSpeed;
        }

        if (VK_DOWN == n16KeyCode)
        {
            fPitch -= fTurnSpeed;
        }

        if (VK_RIGHT == n16KeyCode)
        {
            fYaw -= fTurnSpeed;
        }

        if (VK_LEFT == n16KeyCode)
        {
            fYaw += fTurnSpeed;
        }

        // Prevent looking too far up or down.
        fPitch = min(fPitch, XM_PIDIV4);
        fPitch = max(-XM_PIDIV4, fPitch);

        // Move the camera in model space.
        float x = move.x * -cosf(fYaw) - move.z * sinf(fYaw);
        float z = move.x * sinf(fYaw) - move.z * cosf(fYaw);
        f3EyePos.x += x * fMoveSpeed;
        f3EyePos.z += z * fMoveSpeed;

        // Determine the look direction.
        float r = cosf(fPitch);
        f3LockAt.x = r * sinf(fYaw);
        f3LockAt.y = sinf(fPitch);
        f3LockAt.z = r * cosf(fYaw);

        if (VK_TAB == n16KeyCode)
        {//��Tab����ԭ�����λ��
            f3EyePos = XMFLOAT3(0.0f, 0.0f, -10.0f); //�۾�λ��
            f3LockAt = XMFLOAT3(0.0f, 0.0f, 0.0f);    //�۾�������λ��
            f3HeapUp = XMFLOAT3(0.0f, 1.0f, 0.0f);    //ͷ�����Ϸ�λ��
        }

    }

    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
