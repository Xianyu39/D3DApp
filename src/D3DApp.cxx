#include "D3DApp.h"
#include "DXTrace.h"
#include "d3dUtil.h"
#include "DDSTextureLoader11.h"
#include "WICTextureLoader11.h"

#include <sstream>
#include <array>
#include <cassert>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace {
    D3DApp* pD3dApp = nullptr;
} // namespace 

// 回调函数
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return pD3dApp->MsgProc(hwnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance, const std::wstring& windowName, UINT initWidth, UINT initHeight) :
    hInstance(hInstance),
    hMainWnd(nullptr),
    appPaused(false),
    minimized(false),
    maximized(false),
    resizing(false),
    mainWndTitle(windowName),
    clientWidth(initWidth),
    clientHeight(initHeight),
    clock(),
    enable4xMSAA(true),
    MSAAQuality(0),
    pDevice(nullptr),
    pImmediateDeviceContext(nullptr),
    pSwapChain(nullptr),
    // pDevice1(nullptr),
    // pImmediateDeviceContext1(nullptr),
    // pSwapChain1(nullptr),
    pDepthStencilBuffer(nullptr),
    pDepthStencilView(nullptr),
    pRenderTargetView(nullptr),
    pVertexBuffers(),
    vertexOffsets(),
    vertexStrides(),
    pIndexBuffer(nullptr)
{
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    pD3dApp = this;
}
D3DApp::~D3DApp() {
    if (pImmediateDeviceContext) {
        pImmediateDeviceContext->ClearState();
    }
}

// 获取应用实例的句柄
inline HINSTANCE D3DApp::AppInst()const {
    return hInstance;
}
// 获取主窗口句柄
inline HWND D3DApp::MainWnd()const {
    return hMainWnd;
}
float D3DApp::AspectRatio()const {
    return static_cast<float>(clientWidth) / clientHeight;
}

int D3DApp::Run() {
    MSG msg = { 0 };
    clock.Reset();
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            clock.Tick();
            if (appPaused) {
                Sleep(100);
            }
            else {
                CalculateFrameStats();
                UpdateScene(clock.DeltaTime());
                DrawScene();
            }
        }
        InMainLoop();
    }
    return 0;
}

bool D3DApp::Init() {
    return InitMainWindow() && InitDirect3D();
}
void D3DApp::OnResize() {
    assert(pImmediateDeviceContext);
    assert(pDevice);
    assert(pSwapChain);

    if (pDevice1 != nullptr)
    {
        assert(pImmediateDeviceContext1);
        assert(pDevice1);
        assert(pSwapChain1);
    }

    
    // 释放渲染管线输出用到的相关资源
    pRenderTargetView.Reset();
    pDepthStencilView.Reset();
    pDepthStencilBuffer.Reset();

    // 重设交换链并且重新创建渲染目标视图
    ComPtr<ID3D11Texture2D> backBuffer;
    HR(pSwapChain->ResizeBuffers(1, clientWidth, clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
    HR(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())));
    HR(pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, pRenderTargetView.GetAddressOf()));

    // 设置调试对象名
    D3D11SetDebugObjectName(backBuffer.Get(), "BackBuffer[0]");

    backBuffer.Reset();


    D3D11_TEXTURE2D_DESC depthStencilDesc;

    depthStencilDesc.Width = clientWidth;
    depthStencilDesc.Height = clientHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // 要使用 4X MSAA? --需要给交换链设置MASS参数
    if (enable4xMSAA)
    {
        depthStencilDesc.SampleDesc.Count = 4;
        depthStencilDesc.SampleDesc.Quality = MSAAQuality - 1;
    }
    else
    {
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
    }



    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    // 创建深度缓冲区以及深度模板视图
    HR(pDevice->CreateTexture2D(&depthStencilDesc, nullptr, pDepthStencilBuffer.GetAddressOf()));
    HR(pDevice->CreateDepthStencilView(pDepthStencilBuffer.Get(), nullptr, pDepthStencilView.GetAddressOf()));


    // 将渲染目标视图和深度/模板缓冲区结合到管线
    pImmediateDeviceContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), pDepthStencilView.Get());

    // 设置视口变换
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(clientWidth);
    viewport.Height = static_cast<float>(clientHeight);
    viewport.MinDepth = 0.0f;    viewport.MaxDepth = 1.0f;

    pImmediateDeviceContext->RSSetViewports(1, &viewport);
}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg)
    {
        // WM_ACTIVATE is sent when the window is activated or deactivated.  
        // We pause the game when the window is deactivated and unpause it 
        // when it becomes active.  
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            appPaused = true;
            clock.Stop();
        }
        else
        {
            appPaused = false;
            clock.Start();
        }
        return 0;

        // WM_SIZE is sent when the user resizes the window.  
    case WM_SIZE:
        // Save the new client area dimensions.
        clientWidth = LOWORD(lParam);
        clientHeight = HIWORD(lParam);
        if (pDevice)
        {
            if (wParam == SIZE_MINIMIZED)
            {
                appPaused = true;
                minimized = true;
                maximized = false;
            }
            else if (wParam == SIZE_MAXIMIZED)
            {
                appPaused = false;
                minimized = false;
                maximized = true;
                OnResize();
            }
            else if (wParam == SIZE_RESTORED)
            {

                // Restoring from minimized state?
                if (minimized)
                {
                    appPaused = false;
                    minimized = false;
                    OnResize();
                }

                // Restoring from maximized state?
                else if (maximized)
                {
                    appPaused = false;
                    maximized = false;
                    OnResize();
                }
                else if (resizing)
                {
                    // If user is dragging the resize bars, we do not resize 
                    // the buffers here because as the user continuously 
                    // drags the resize bars, a stream of WM_SIZE messages are
                    // sent to the window, and it would be pointless (and slow)
                    // to resize for each WM_SIZE message received from dragging
                    // the resize bars.  So instead, we reset after the user is 
                    // done resizing the window and releases the resize bars, which 
                    // sends a WM_EXITSIZEMOVE message.
                }
                else // API call such as SetWindowPos or pSwapChain->SetFullscreenState.
                {
                    OnResize();
                }
            }
        }
        return 0;

        // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
    case WM_ENTERSIZEMOVE:
        appPaused = true;
        resizing = true;
        clock.Stop();
        return 0;

        // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
        // Here we reset everything based on the new window dimensions.
    case WM_EXITSIZEMOVE:
        appPaused = false;
        resizing = false;
        clock.Start();
        OnResize();
        return 0;

        // WM_DESTROY is sent when the window is being destroyed.
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

        // The WM_MENUCHAR message is sent when a menu is active and the user presses 
        // a key that does not correspond to any mnemonic or accelerator key. 
    case WM_MENUCHAR:
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);

        // Catch this message so to prevent the window from becoming too small.
    case WM_GETMINMAXINFO:
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
        return 0;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        return 0;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        return 0;
    case WM_MOUSEMOVE:
        return 0;
    }
    InCallback();

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
bool D3DApp::InitMainWindow() {
    WNDCLASS w;
    w.style = CS_HREDRAW | CS_VREDRAW;
    w.lpfnWndProc = MainWndProc;
    w.cbClsExtra = 0;
    w.cbWndExtra = 0;
    w.hInstance = hInstance;
    w.hIcon = LoadIcon(0, IDI_APPLICATION);
    w.hCursor = LoadCursor(0, IDC_ARROW);
    w.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    w.lpszMenuName = 0;
    w.lpszClassName = L"D3DWndClassName";

    if (!RegisterClass(&w)) {
        MessageBox(0, L"RegisterClass Failed.", 0, 0);
        return false;
    }
    // Compute window rectangle dimensions based on requested client area dimensions.
    RECT R = { 0, 0, (LONG)clientWidth, (LONG)clientHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
    int width = R.right - R.left;
    int height = R.bottom - R.top;

    hMainWnd = CreateWindow(
        L"D3DWndClassName",
        mainWndTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
    if (!hMainWnd) {
        MessageBox(0, L"CreateWindow Failed.", 0, 0);
        return false;
    }

    ShowWindow(hMainWnd, SW_SHOW);
    UpdateWindow(hMainWnd);
    return true;
}
bool D3DApp::InitDirect3D() {
    HRESULT result = S_OK;
    UINT flag = 0;
#if defined(DEBUG) || defined(_DEBUG)
    flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    std::array<D3D_DRIVER_TYPE, 3> driverTypes = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_REFERENCE,
        D3D_DRIVER_TYPE_WARP
    };
    // 注意顺序，等11.1的被发现不可用再去测试11.0；否则先遇见11.0就不会去看11.1了
    std::array<D3D_FEATURE_LEVEL, 2> featureLevels = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };
    D3D_FEATURE_LEVEL featureLevel;
    for (auto driverType : driverTypes) {
        result = D3D11CreateDevice(
            nullptr,
            driverType,
            nullptr,
            flag,
            featureLevels.data(),
            (UINT)featureLevels.size(),
            D3D11_SDK_VERSION,
            pDevice.GetAddressOf(),
            &featureLevel,
            pImmediateDeviceContext.GetAddressOf()
        );
        if (result == E_INVALIDARG) {
            result = D3D11CreateDevice(
                nullptr,
                driverType,
                nullptr,
                flag,
                featureLevels.data() + 1,
                (UINT)featureLevels.size(),
                D3D11_SDK_VERSION,
                pDevice.GetAddressOf(),
                &featureLevel,
                pImmediateDeviceContext.GetAddressOf()
            );
        }
        if (SUCCEEDED(result)) { break; }
    }
    if (FAILED(result)) {
        MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
        return false;
    }
    if (std::find(featureLevels.begin(), featureLevels.end(), featureLevel) == featureLevels.end()) {
        MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
        return false;
    }
    pDevice->CheckMultisampleQualityLevels(
        DXGI_FORMAT_R8G8B8A8_UNORM,
        4,
        &MSAAQuality
    );
    assert(MSAAQuality > 0);

    ComPtr<IDXGIDevice> pDXGIDevice = nullptr;
    ComPtr<IDXGIAdapter> pDXGIAdapter = nullptr;
    ComPtr<IDXGIFactory1> pDXGIFactory1 = nullptr;
    ComPtr<IDXGIFactory2> pDXGIFactory2 = nullptr;

    HR(pDevice.As(&pDXGIDevice));
    HR(pDXGIDevice->GetAdapter(pDXGIAdapter.GetAddressOf()));
    HR(pDXGIAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(pDXGIFactory1.GetAddressOf())));

    result = pDXGIFactory1.As(&pDXGIFactory2);
    // 如果包含，则说明支持D3D11.1
    if (pDXGIFactory2 != nullptr) {
        HR(pDevice.As(&pDevice1));
        HR(pImmediateDeviceContext.As(&pImmediateDeviceContext1));
        // 填充各种结构体用以描述交换链
        DXGI_SWAP_CHAIN_DESC1 sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.Width = clientWidth;
        sd.Height = clientHeight;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        // 是否开启4倍多重采样？
        if (enable4xMSAA) {
            sd.SampleDesc.Count = 4;
            sd.SampleDesc.Quality = MSAAQuality - 1;
        }
        else {
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
        }
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = 0;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fd;
        fd.RefreshRate.Numerator = 60;
        fd.RefreshRate.Denominator = 1;
        fd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        fd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        fd.Windowed = TRUE;
        // 为当前窗口创建交换链
        HR(pDXGIFactory2->CreateSwapChainForHwnd(pDevice.Get(), hMainWnd, &sd, &fd, nullptr, pSwapChain1.GetAddressOf()));
        HR(pSwapChain1.As(&pSwapChain));
    }
    else
    {
        // 填充DXGI_SWAP_CHAIN_DESC用以描述交换链
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferDesc.Width = clientWidth;
        sd.BufferDesc.Height = clientHeight;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        // 是否开启4倍多重采样？
        if (enable4xMSAA)
        {
            sd.SampleDesc.Count = 4;
            sd.SampleDesc.Quality = MSAAQuality - 1;
        }
        else
        {
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
        }
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;
        sd.OutputWindow = hMainWnd;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = 0;
        HR(pDXGIFactory1->CreateSwapChain(pDevice.Get(), &sd, pSwapChain.GetAddressOf()));
    }

    // 可以禁止alt+enter全屏
    pDXGIFactory1->MakeWindowAssociation(hMainWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

    // 设置调试对象名
    D3D11SetDebugObjectName(pImmediateDeviceContext.Get(), "ImmediateContext");
    DXGISetDebugObjectName(pSwapChain.Get(), "SwapChain");

    // 每当窗口被重新调整大小的时候，都需要调用这个OnResize函数。现在调用
    // 以避免代码重复
    OnResize();

    return true;
}

void D3DApp::CalculateFrameStats() {
    // 该代码计算每秒帧速，并计算每一帧渲染需要的时间，显示在窗口标题
    static int frameCnt = 0;
    static float timeElapsed = 0.0f;

    frameCnt++;

    if ((clock.TotalTime() - timeElapsed) >= 1.0f)
    {
        float fps = (float)frameCnt; // fps = frameCnt / 1
        float mspf = 1000.0f / fps;

        std::wostringstream outs;
        outs.precision(6);
        outs << mainWndTitle << L"    "
            << L"FPS: " << fps << L"    "
            << L"Frame Time: " << mspf << L" (ms)";
        SetWindowText(hMainWnd, outs.str().c_str());

        // Reset for next average.
        frameCnt = 0;
        timeElapsed += 1.0f;
    }
}

HRESULT D3DApp::CreateShaderFromFile(
    const std::wstring& input,
    LPCSTR entry,
    LPCSTR shaderModel,
    ID3DBlob** ppBlobOut
) {
    HRESULT hr = S_OK;
    ComPtr<ID3DBlob> pData = nullptr, pErrorMessage = nullptr;
    UINT compileFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    compileFlag |= D3DCOMPILE_DEBUG;
    compileFlag |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    hr = D3DCompileFromFile(
        input.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entry,
        shaderModel,
        compileFlag,
        0,
        ppBlobOut,
        pErrorMessage.GetAddressOf()
    );
    if (FAILED(hr)) {
        if (pErrorMessage) {
            OutputDebugStringA(reinterpret_cast<LPCSTR>(pErrorMessage->GetBufferPointer()));
            pErrorMessage->Release();
        }
        return hr;
    }
    return hr;
}

HRESULT D3DApp::GenVertexShader(const std::wstring& srcPath, const std::wstring& outputPath, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputlayout, ID3D11VertexShader** ppShader) {
    HRESULT hr = S_OK;
    ComPtr<ID3DBlob> pB;
    ComPtr<ID3D11VertexShader> pVertexShader;
    hr = CreateShaderFromFile(srcPath, "main", "vs_5_0", pB.ReleaseAndGetAddressOf());
    if (FAILED(hr)) { return hr; }
    hr = pDevice->CreateVertexShader(pB->GetBufferPointer(), pB->GetBufferSize(), nullptr, pVertexShader.ReleaseAndGetAddressOf());
    pImmediateDeviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0);
    if (FAILED(hr)) { return hr; }
    if (ppShader) {
        *ppShader = pVertexShader.Get();
    }

    // 输入布局
    ComPtr<ID3D11InputLayout> pInputLayout;
    hr = pDevice->CreateInputLayout(inputlayout.data(), (UINT)inputlayout.size(), pB->GetBufferPointer(), pB->GetBufferSize(), pInputLayout.ReleaseAndGetAddressOf());
    pImmediateDeviceContext->IASetInputLayout(pInputLayout.Get());
    return hr;
}
HRESULT D3DApp::GenPixelShader(const std::wstring& srcPath, const std::wstring& outputPath, ID3D11PixelShader** ppShader) {
    HRESULT hr = S_OK;
    ComPtr<ID3DBlob> pB;
    ComPtr<ID3D11PixelShader> pPixelShader;
    hr = CreateShaderFromFile(srcPath, "main", "ps_5_0", pB.ReleaseAndGetAddressOf());
    if (FAILED(hr)) { return hr; }
    hr = pDevice->CreatePixelShader(reinterpret_cast<void*>(pB->GetBufferPointer()), pB->GetBufferSize(), nullptr, pPixelShader.ReleaseAndGetAddressOf());
    pImmediateDeviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0);
    if (ppShader) {
        *ppShader = pPixelShader.Get();
    }
    return hr;
}

HRESULT D3DApp::UpdateConstantBuffer(
    ComPtr<ID3D11Buffer> pBuffer,
    void* byteData,
    size_t byteSize,
    D3D11_MAP mapType,
    UINT subsources,
    UINT mapFlags
) {
    HRESULT hr = S_OK;
    D3D11_MAPPED_SUBRESOURCE mappedData;
    hr = pImmediateDeviceContext->Map(pBuffer.Get(), subsources, mapType, mapFlags, &mappedData);
    memcpy_s(mappedData.pData, byteSize, byteData, byteSize);
    pImmediateDeviceContext->Unmap(pBuffer.Get(), subsources);
    return hr;
}

void D3DApp::SetVertexBuffers() {
    std::vector<ID3D11Buffer*> buffers(pVertexBuffers.size(), nullptr);
    for (size_t i = 0; i < pVertexBuffers.size();++i) {
        buffers[i] = pVertexBuffers[i].Get();
    }
    pImmediateDeviceContext->IASetVertexBuffers(0, (UINT)pVertexBuffers.size(), buffers.data(), vertexStrides.data(), vertexOffsets.data());
}

HRESULT D3DApp::GetConstantBuffer(
    ID3D11Buffer** ppConstantBuffer,
    UINT byteWidth,
    D3D11_USAGE usage,
    D3D11_CPU_ACCESS_FLAG cpuAccess,
    void* initData
) {
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC cbc;
    ZeroMemory(&cbc, sizeof(cbc));
    cbc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbc.CPUAccessFlags = cpuAccess;
    cbc.Usage = usage;
    cbc.ByteWidth = byteWidth;
    if (initData) {
        D3D11_SUBRESOURCE_DATA ini;
        ini.pSysMem = initData;
        hr = pDevice->CreateBuffer(&cbc, &ini, ppConstantBuffer);
    }
    else {
        hr = pDevice->CreateBuffer(&cbc, nullptr, ppConstantBuffer);
    }
    return hr;
}


HRESULT D3DApp::PSSetDDSTexture(
    const wchar_t* fullname,
    UINT slot,
    ID3D11ShaderResourceView** ppTextureView,
    ID3D11Resource** ppTexture
) {
    using namespace DirectX;
    HRESULT hr = S_OK;
    if (ppTextureView) {
        hr = CreateDDSTextureFromFile(pDevice.Get(), fullname, ppTexture, ppTextureView);
        pImmediateDeviceContext->PSSetShaderResources(slot, 1, ppTextureView);
    }
    else {
        ComPtr<ID3D11ShaderResourceView> pv;
        hr = CreateDDSTextureFromFile(pDevice.Get(), fullname, ppTexture, pv.GetAddressOf());
        pImmediateDeviceContext->PSSetShaderResources(slot, 1, pv.GetAddressOf());
    }
    return hr;
}