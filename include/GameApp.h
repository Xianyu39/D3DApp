#if !defined(GAMEAPP_H)
#define GAMEAPP_H

#include <DirectXMath.h>
#include <vector>
#include <memory>

#include "D3DApp.h"
#include "wrl/client.h"
#include "Vertex.h"
#include "ConstantBuffer.h"
#include "Transform.h"
#include "Geometry.h"
#include "Camera.h"
#include "GameObject.h"

class GameApp :public D3DApp {
protected:
    template<class T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<ID3D11VertexShader> pVertexShader;
    ComPtr<ID3D11PixelShader> pPixelShader;
    ComPtr<ID3D11InputLayout> pInputLayout;
    std::vector<ComPtr<ID3D11Buffer>> pConstantBuffers;

    CBChangesEveryFrame cbFrame;
    CBChangesEveryDrawing cbDrawing;
    CBChangesOnResize cbResize;
    CBChangesRarely cbRare;

    std::shared_ptr<Camera> pCamera;
    ComPtr<ID3D11SamplerState> pSamplerState;

    GameObject woodBox;
    GameObject floor;
    std::vector<GameObject> walls;

public:
    GameApp(HINSTANCE hInstance)
        :D3DApp(hInstance, L"DX11", 800, 450), pConstantBuffers(4, nullptr), walls(4) {}
    GameApp(HINSTANCE hInstance, const WCHAR* caption, UINT width, UINT height)
        :D3DApp(hInstance, caption, width, height), pConstantBuffers(4, nullptr), walls(4) {}
    ~GameApp() {}
    void UpdateScene(float dt)override;
    void DrawScene()override;
    bool InitEffect();
    bool InitResource();
    bool Init()override;
};
#endif // GAMEAPP_H