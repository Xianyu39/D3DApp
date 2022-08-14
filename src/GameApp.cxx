#include "GameApp.h"
#include "d3dcompiler.h"
#include "DXTrace.h"
#include "Vertex.h"
#include "DirectXMath.h"
#include "vector"
#include "Geometry.h"
#include "DDSTextureLoader11.h"

#pragma comment(lib, "D3DCompiler.lib")

inline DirectX::XMMATRIX XM_CALLCONV InverseTranspose(DirectX::FXMMATRIX M)
{
    using namespace DirectX;

    // 世界矩阵的逆的转置仅针对法向量，我们也不需要世界矩阵的平移分量
    // 而且不去掉的话，后续再乘上观察矩阵之类的就会产生错误的变换结果
    XMMATRIX A = M;
    A.r[3] = g_XMIdentityR3;

    return XMMatrixTranspose(XMMatrixInverse(nullptr, A));
}

void GameApp::UpdateScene(float dt) {
    using namespace DirectX;
    static float phi = 0.f, theta = 0.f, a = 0.f;
    XMMATRIX W = XMMatrixRotationX(phi) * XMMatrixRotationY(theta);
    XMStoreFloat4x4(&vcbuffer.world, XMMatrixTranspose(W));
    XMStoreFloat4x4(&vcbuffer.texTrans, XMMatrixRotationZ(a));
    XMStoreFloat4x4(&vcbuffer.wordInv, XMMatrixTranspose(InverseTranspose(W)));
    HR(UpdateConstantBuffer(pVConstantBuffer.Get(), &vcbuffer, sizeof(vcbuffer)));
    
    phi += 0.0001f;
    theta += 0.00015f;
    a += 0.001f;
}

void GameApp::DrawScene() {
    FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    pImmediateDeviceContext->ClearRenderTargetView(pRenderTargetView.Get(), color);
    pImmediateDeviceContext->ClearDepthStencilView(
        pDepthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.f,
        0
    );
    pImmediateDeviceContext->DrawIndexed(indexNum, 0, 0);
    pSwapChain->Present(0, 0);
}

bool GameApp::Init() {
    return D3DApp::Init();
}

bool GameApp::InitEffect() {
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout(VertexPosNormalTex::inputLayout, VertexPosNormalTex::inputLayout + ARRAYSIZE(VertexPosNormalTex::inputLayout));
    HR(GenVertexShader(L"HLSL\\vs.hlsl", L"", layout));
    HR(GenPixelShader(L"HLSL\\ps.hlsl", L""));
    return true;
}

bool GameApp::InitResource() {
    Geometry::MeshData<VertexPosNormalTex, UINT> mesh = Geometry::CreateBox<VertexPosNormalTex, UINT>();
    HR(AddVertexBuffer<VertexPosNormalTex>(mesh.vertexVec));
    SetVertexBuffers();
    HR(SetIndexBuffer<UINT>(mesh.indexVec));
    indexNum = (UINT)mesh.indexVec.size();

    HR(GetConstantBuffer(pVConstantBuffer.GetAddressOf(), sizeof(VConstantBuffer)));
    pImmediateDeviceContext->VSSetConstantBuffers(0, 1, pVConstantBuffer.GetAddressOf());
    HR(GetConstantBuffer(pPConstantBuffer.GetAddressOf(), sizeof(PSConstantBuffer)));
    pImmediateDeviceContext->PSSetConstantBuffers(1, 1, pPConstantBuffer.GetAddressOf());

    DirectX::XMStoreFloat4x4(&vcbuffer.world, DirectX::XMMatrixIdentity());
    DirectX::XMStoreFloat4x4(&vcbuffer.view, DirectX::XMMatrixTranspose(
        DirectX::XMMatrixLookAtLH(
            XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
            XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
            XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
        )
    ));
    DirectX::XMStoreFloat4x4(&vcbuffer.proj, DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio(), 1.0f, 1000.0f)));

    // 初始化用于PS的常量缓冲区的值
    // 这里只使用一盏点光来演示
    pcbuffer.pointLight[0].position = XMFLOAT3(0.0f, 0.0f, -10.0f);
    pcbuffer.pointLight[0].ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    pcbuffer.pointLight[0].diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    pcbuffer.pointLight[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    pcbuffer.pointLight[0].att = XMFLOAT3(0.0f, 0.1f, 0.0f);
    pcbuffer.pointLight[0].range = 25.0f;
    pcbuffer.numDirLight = 0;
    pcbuffer.numPointLight = 1;
    pcbuffer.numSpotLight = 0;
    pcbuffer.material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    pcbuffer.material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    pcbuffer.material.specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 5.0f);
    pcbuffer.eyePos = XMFLOAT4(0.0f, 0.0f, -5.0f, 0.0f);
    UpdateConstantBuffer(pPConstantBuffer, &pcbuffer, sizeof(pcbuffer));
    
    pImmediateDeviceContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );
    // 纹理
    PSSetDDSTexture(L".\\Texture\\flare.dds", 0);
    PSSetDDSTexture(L".\\Texture\\flarealpha.dds", 1);

    // 纹理采样器
    D3D11_SAMPLER_DESC sdc;
    ZeroMemory(&sdc, sizeof(sdc));
    sdc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
    D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_BORDER;
    sdc.AddressU = sdc.AddressV = sdc.AddressW = mode;
    sdc.BorderColor[3] = 0.0f;
    sdc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sdc.MinLOD = 0;
    sdc.MaxLOD = D3D11_FLOAT32_MAX;
    ComPtr<ID3D11SamplerState> pSamplerState;
    HR(pDevice->CreateSamplerState(&sdc, pSamplerState.GetAddressOf()));
    pImmediateDeviceContext->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());

    return true;
}