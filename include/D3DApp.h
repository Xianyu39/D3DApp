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
    // �������
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
    // D3D���
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

    // ������
    std::vector<ComPtr<ID3D11Buffer>> pVertexBuffers;
    std::vector<UINT> vertexStrides;
    std::vector<UINT> vertexOffsets;
    ComPtr<ID3D11Buffer> pIndexBuffer;

public:
    D3DApp(HINSTANCE hInstance, const std::wstring& windowName, UINT initWidth, UINT initHeight);
    virtual ~D3DApp();

    HINSTANCE AppInst()const;                 // ��ȡӦ��ʵ���ľ��
    HWND      MainWnd()const;                 // ��ȡ�����ھ��
    float     AspectRatio()const;             // ��ȡ��Ļ��߱�

    int Run();                                // ���г��򣬽�����Ϸ��ѭ��

    // ��ܷ������ͻ���������Ҫ������Щ������ʵ���ض���Ӧ������
    virtual bool Init();                      // �ø��෽����Ҫ��ʼ�����ں�Direct3D����
    virtual void OnResize();                  // �ø��෽����Ҫ�ڴ��ڴ�С�䶯��ʱ�����
    virtual void UpdateScene(float dt) = 0;   // ������Ҫʵ�ָ÷��������ÿһ֡�ĸ���
    virtual void DrawScene() = 0;             // ������Ҫʵ�ָ÷��������ÿһ֡�Ļ���
    // ���ڵ���Ϣ�ص�����
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    /**
     * @brief ����������ɫ�����ҽ������õ���Ⱦ����
     * @param srcPath Դ�ļ�ȫ��
     * @param outputPath �����Ҫ�������õĶ������ļ��Ļ����õĶ������ļ�ȫ��
     * @param inputLayout ���������ʽ����*/
    virtual HRESULT GenVertexShader(const std::wstring& srcPath, const std::wstring& outputPath, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputlayout);
    /**
     * @brief ����������ɫ�����ҽ������õ���Ⱦ����
     * @param srcPath Դ�ļ�ȫ��
     * @param outputPath �����Ҫ�������õĶ������ļ��Ļ����õĶ������ļ�ȫ��
     */
    virtual HRESULT GenPixelShader(const std::wstring& srcPath, const std::wstring& outputPath);
    /**
     * @brief ����dynamic�ĳ�����������ֵ
     * @param pBuffer Ҫ���µĻ�����ָ��
     * @param byteData ���µ�����
     * @param byteSize ���ݴ�С
     * */
    virtual HRESULT UpdateConstantBuffer(ComPtr<ID3D11Buffer> pBuffer, void* byteData, size_t byteSize, D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD, UINT subsources = 0, UINT mapFlags = 0);
    /**
     * @brief �����µĶ��㻺����*/
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
     * @brief �����µ���������������������Ⱦ����
     * @param IndexType ��������
     * @param indices ��������
     * @param format ������dxgi��ʽ
     * @param offset ƫ��*/
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
     * @brief ����������������Ĭ���Ƕ�̬����������
     * @param ppConstantBuffer �����õĳ���������
     * @param byteWidth ���������
     * @param usage ����������
     * @param cpuAccess CPU����Ȩ��
     * @param initData ��ʼ�����ݣ�����Ҫ������null*/
    virtual HRESULT GetConstantBuffer(
        ID3D11Buffer** ppConstantBuffer,
        UINT byteWidth,
        D3D11_USAGE usage = D3D11_USAGE_DYNAMIC,
        D3D11_CPU_ACCESS_FLAG cpuAccess = D3D11_CPU_ACCESS_WRITE,
        void* initData = nullptr
    );
    /**
     * @brief �����ж��㻺����������Ⱦ����*/
    virtual void SetVertexBuffers();
    virtual HRESULT PSSetDDSTexture(
        const wchar_t* fullname,
        UINT slot,
        ID3D11ShaderResourceView** ppTextureView = nullptr,
        ID3D11Resource** ppTexture = nullptr
    );
    /**
 * @brief ��HLSLԴ�����ļ�����Ϊ����������
 * @param input ��·����Դ�ļ���
 * @param entry ����������
 * @param shaderModel shader����,����cs_5_0����dx11��compute shader
 * @param ppBlobOut ����õĶ���������*/
    static HRESULT CreateShaderFromFile(
        const std::wstring& input,
        LPCSTR entry,
        LPCSTR shaderModel,
        ID3DBlob** ppBlobOut = nullptr
    );
protected:
    bool InitMainWindow();       // ���ڳ�ʼ��
    bool InitDirect3D();         // Direct3D��ʼ��

    void CalculateFrameStats();  // ����ÿ��֡�����ڴ�����ʾ
};

#endif // D3DAPP_H
