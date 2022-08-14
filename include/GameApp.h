#if !defined(GAMEAPP_H)
#define GAMEAPP_H

#include "D3DApp.h"
#include "wrl/client.h"
#include "Vertex.h"
#include "ConstantBuffer.h"

class GameApp :public D3DApp {
protected:
    template<class T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    std::wstring vertexShaderSrcPath;
    std::wstring pixelShaderSrcPath;
    ComPtr<ID3D11VertexShader> pVertexShader;
    ComPtr<ID3D11PixelShader> pPixelShader;
    ComPtr<ID3D11InputLayout> pInputLayout;
    ComPtr<ID3D11Buffer> pVertexBuffer;
    ComPtr<ID3D11Buffer> pIndexBuffer;
    ComPtr<ID3D11Buffer> pVConstantBuffer;
    ComPtr<ID3D11Buffer> pPConstantBuffer;

    VConstantBuffer vcbuffer;
    PSConstantBuffer pcbuffer;
    UINT indexNum;

public:
    
    GameApp(HINSTANCE hInstance)
        :D3DApp(hInstance, L"DX11", 800, 450) {}
    GameApp(HINSTANCE hInstance, const WCHAR* caption, UINT width, UINT height)
        :D3DApp(hInstance, caption, width, height){}
    ~GameApp() {}
    void UpdateScene(float dt)override;
    void DrawScene()override;
    bool InitEffect();
    bool InitResource();
    bool Init()override;
    void setVertexShaderSrc(const std::wstring& fullname) { vertexShaderSrcPath = fullname; }
    void setPixelShaderSrc(const std::wstring& fullname) { pixelShaderSrcPath = fullname; }
};
#endif // GAMEAPP_H