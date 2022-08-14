#if !defined(D3DAPP_H)
#define D3DAPP_H

#include <Windows.h>
#include <wrl/client.h>
#include <d3d11_1.h>
#include <string>
#include <vector>
#include <functional>

#include "CpuTimer.h"

class D3DApp {
protected:
    // 窗口相关
    HINSTANCE hInstance;
    HWND hMainWnd;
    bool appPaused;
    bool minimized;
    bool maximized;
    bool resizing;
    std::wstring mainWndTitle;
    UINT clientWidth;
    UINT clientHeight;

    std::function<void(void)> InMainLoop = []() {};
    std::function<void(void)> InCallback = []() {};

protected:
    // D3D相关
    bool enable4xMSAA;
    UINT MSAAQuality;
    template<class T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;
    // d3d11
    ComPtr<ID3D11Device> pDevice;
    ComPtr<ID3D11DeviceContext> pImmediateDeviceContext;
    ComPtr<IDXGISwapChain> pSwapChain;
    // d3d11.1
    ComPtr<ID3D11Device1> pDevice1;
    ComPtr<ID3D11DeviceContext1> pImmediateDeviceContext1;
    ComPtr<IDXGISwapChain1> pSwapChain1;
    // res
    ComPtr<ID3D11Texture2D> pDepthStencilBuffer;
    ComPtr<ID3D11DepthStencilView> pDepthStencilView;
    ComPtr<ID3D11RenderTargetView> pRenderTargetView;
    D3D11_VIEWPORT viewport;
    CpuTimer clock;

    // 缓冲区
    std::vector<ComPtr<ID3D11Buffer>> pVertexBuffers;
    std::vector<UINT> vertexStrides;
    std::vector<UINT> vertexOffsets;
    ComPtr<ID3D11Buffer> pIndexBuffer;

public:
    D3DApp(HINSTANCE hInstance, const std::wstring& windowName, UINT initWidth, UINT initHeight);
    virtual ~D3DApp();

    HINSTANCE AppInst()const;                 // 获取应用实例的句柄
    HWND      MainWnd()const;                 // 获取主窗口句柄
    float     AspectRatio()const;             // 获取屏幕宽高比

    int Run();                                // 运行程序，进行游戏主循环

    // 框架方法。客户派生类需要重载这些方法以实现特定的应用需求
    virtual bool Init();                      // 该父类方法需要初始化窗口和Direct3D部分
    virtual void OnResize();                  // 该父类方法需要在窗口大小变动的时候调用
    virtual void UpdateScene(float dt) = 0;   // 子类需要实现该方法，完成每一帧的更新
    virtual void DrawScene() = 0;             // 子类需要实现该方法，完成每一帧的绘制
    // 窗口的消息回调函数
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    /**
     * @brief 创建顶点着色器并且将其设置到渲染管线
     * @param srcPath 源文件全名
     * @param outputPath 如果需要输出编译好的二进制文件的话设置的二进制文件全名
     * @param inputLayout 顶点输入格式数组*/
    virtual HRESULT GenVertexShader(const std::wstring& srcPath, const std::wstring& outputPath, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputlayout);
    /**
     * @brief 创建像素着色器并且将其设置到渲染管线
     * @param srcPath 源文件全名
     * @param outputPath 如果需要输出编译好的二进制文件的话设置的二进制文件全名
     */
    virtual HRESULT GenPixelShader(const std::wstring& srcPath, const std::wstring& outputPath);
    /**
     * @brief 更新dynamic的常量缓冲区的值
     * @param pBuffer 要更新的缓冲区指针
     * @param byteData 更新的数据
     * @param byteSize 数据大小
     * */
    virtual HRESULT UpdateConstantBuffer(ComPtr<ID3D11Buffer> pBuffer, void* byteData, size_t byteSize, D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD, UINT subsources = 0, UINT mapFlags = 0);
    /**
     * @brief 创建新的顶点缓冲区*/
    template<class VertexType>
    HRESULT AddVertexBuffer(const std::vector<VertexType>& vertices, UINT offset = 0) {
        D3D11_BUFFER_DESC vbd;
        ZeroMemory(&vbd, sizeof(vbd));
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = 0;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        vbd.ByteWidth = (UINT)vertices.size() * sizeof(VertexType);
        HRESULT hr = S_OK;
        ComPtr<ID3D11Buffer> pBuffer;
        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = vertices.data();
        hr = pDevice->CreateBuffer(&vbd, &initData, pBuffer.ReleaseAndGetAddressOf());
        this->pVertexBuffers.push_back(pBuffer);
        this->vertexOffsets.push_back(std::move(offset));
        this->vertexStrides.push_back(sizeof(VertexType));
        return hr;
    }
    /**
     * @brief 创建新的索引缓冲区并且送入渲染管线
     * @param IndexType 索引类型
     * @param indices 索引数组
     * @param format 索引的dxgi格式
     * @param offset 偏移*/
    template<class IndexType = UINT>
    HRESULT SetIndexBuffer(const std::vector<IndexType>& indices, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT, UINT offset = 0) {
        D3D11_BUFFER_DESC ibd;
        ZeroMemory(&ibd, sizeof(D3D11_BUFFER_DESC));
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.ByteWidth = sizeof(IndexType) * (UINT)indices.size();
        ibd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = indices.data();
        ComPtr<ID3D11Buffer> indexBuffer;
        auto hr = pDevice->CreateBuffer(&ibd, &data, indexBuffer.GetAddressOf());
        pImmediateDeviceContext->IASetIndexBuffer(indexBuffer.Get(), format, offset);
        return hr;
    }
    /**
     * @brief 创建常量缓冲区，默认是动态常量缓冲区
     * @param ppConstantBuffer 创建好的常量缓冲区
     * @param byteWidth 缓冲区宽度
     * @param usage 缓冲区类型
     * @param cpuAccess CPU访问权限
     * @param initData 初始化数据，不需要可以填null*/
    virtual HRESULT GetConstantBuffer(
        ID3D11Buffer** ppConstantBuffer,
        UINT byteWidth,
        D3D11_USAGE usage = D3D11_USAGE_DYNAMIC,
        D3D11_CPU_ACCESS_FLAG cpuAccess = D3D11_CPU_ACCESS_WRITE,
        void* initData = nullptr
    );
    /**
     * @brief 把所有顶点缓冲区交给渲染管线*/
    virtual void SetVertexBuffers();
    virtual HRESULT PSSetDDSTexture(
        const wchar_t* fullname,
        UINT slot,
        ID3D11ShaderResourceView** ppTextureView = nullptr,
        ID3D11Resource** ppTexture = nullptr
    );
    /**
 * @brief 把HLSL源代码文件编译为二进制数据
 * @param input 带路径的源文件名
 * @param entry 主函数名称
 * @param shaderModel shader类型,比如cs_5_0代表dx11的compute shader
 * @param ppBlobOut 编译好的二进制数据*/
    static HRESULT CreateShaderFromFile(
        const std::wstring& input,
        LPCSTR entry,
        LPCSTR shaderModel,
        ID3DBlob** ppBlobOut = nullptr
    );
protected:
    bool InitMainWindow();       // 窗口初始化
    bool InitDirect3D();         // Direct3D初始化

    void CalculateFrameStats();  // 计算每秒帧数并在窗口显示
};

#endif // D3DAPP_H
