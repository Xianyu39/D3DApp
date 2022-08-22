#include "GameApp.h"
#include "DDSTextureLoader11.h"
#include "FirstPersonCamera.h"
#include <DirectXColors.h>

void GameApp::UpdateScene(float dt) {
    cbFrame.eyePos = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
    cbFrame.view = XMMatrixTranspose(pCamera->GetWorldToLocal());
    UpdateConstantBuffer(pConstantBuffers[1], &cbFrame, sizeof(CBChangesEveryFrame));
}
void GameApp::DrawScene() {
    cbDrawing.world = XMMatrixTranspose(pCamera->GetLocalToWorld());
    cbDrawing.worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(nullptr, XMMatrixTranspose(pCamera->GetLocalToWorld())));
    UpdateConstantBuffer(pConstantBuffers[0], &cbDrawing, sizeof(cbDrawing));
    pImmediateDeviceContext->ClearRenderTargetView(pRenderTargetView.Get(), reinterpret_cast<const float*>(&DirectX::Colors::Black));
    pImmediateDeviceContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    floor.Draw(pImmediateDeviceContext.Get());
    woodBox.Draw(pImmediateDeviceContext.Get());
    for (auto &i : walls) {
        i.Draw(pImmediateDeviceContext.Get());
    }
    
    HR(pSwapChain->Present(0, 0));
}
bool GameApp::InitEffect() {
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout(VertexPosNormalTex::inputLayout, VertexPosNormalTex::inputLayout + 3);
    GenVertexShader(L"./HLSL/vs.hlsl", L"./build/vs.cso", layout, pVertexShader.GetAddressOf());
    GenPixelShader(L"./HLSL/ps.hlsl", L"./build/ps.cso", pPixelShader.GetAddressOf());
    return true;
}
bool GameApp::InitResource() {
    // 创建常量缓冲区
    HR(GetConstantBuffer(pConstantBuffers[0].GetAddressOf(), sizeof(CBChangesEveryDrawing)));
    HR(GetConstantBuffer(pConstantBuffers[1].GetAddressOf(), sizeof(CBChangesEveryFrame)));
    HR(GetConstantBuffer(pConstantBuffers[2].GetAddressOf(), sizeof(CBChangesOnResize)));
    HR(GetConstantBuffer(pConstantBuffers[3].GetAddressOf(), sizeof(CBChangesRarely)));

    // 纹理
    ComPtr<ID3D11ShaderResourceView> texture;
    // 木箱
    HR(CreateDDSTextureFromFile(pDevice.Get(), L"./Texture/WoodCrate.dds", nullptr, texture.GetAddressOf()));
    woodBox.SetTexture(texture.Get());
    woodBox.SetBuffer<VertexPosNormalTex, DWORD>(pDevice.Get(), Geometry::CreateBox());
    // 地板
    HR(CreateDDSTextureFromFile(pDevice.Get(), L"./Texture/floor.dds", nullptr, texture.ReleaseAndGetAddressOf()));
    floor.SetTexture(texture.Get());
    floor.SetBuffer<VertexPosNormalTex, DWORD>(pDevice.Get(), Geometry::CreatePlane(XMFLOAT2(20.0f, 20.0f), XMFLOAT2(5.0f, 5.0f)));
    floor.GetTransform().SetPosition(0.0f, -1.0f, 0.0f);
    // 围墙
    HR(CreateDDSTextureFromFile(pDevice.Get(), L"./Texture/brick.dds", nullptr, texture.ReleaseAndGetAddressOf()));
    for (int i = 0; i < 4; ++i) {
        walls[i].SetBuffer(pDevice.Get(), Geometry::CreatePlane(XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 1.5f)));
        walls[i].SetTexture(texture.Get());
        Transform& t = walls[i].GetTransform();
        t.SetRotation(-XM_PIDIV2, XM_PIDIV2 * i, 0.0f);
        t.SetPosition(i % 2 ? -10.0f * (i - 2) : 0.0f, 3.0f, i % 2 == 0 ? -10.0f * (i - 1) : 0.0f);
    }
    // 采样器
    D3D11_SAMPLER_DESC dsc;
    ZeroMemory(&dsc, sizeof(dsc));
    dsc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    dsc.AddressU = dsc.AddressV = dsc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    dsc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    dsc.MinLOD = 0;
    dsc.MaxLOD = D3D11_FLOAT32_MAX;
    HR(pDevice->CreateSamplerState(&dsc, pSamplerState.GetAddressOf()));

    // 摄像机
    auto c = std::make_shared<FirstPersonCamera>();
    c->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.f);
    c->SetViewPort(0.f, 0.f, (float)clientWidth, (float)clientHeight);
    c->LookAt({ 10.f,5.f,0.f }, { 0.f, 0.f, 0.f }, { 0.f,1.f,0.f });
    pCamera = c;
    // 常量缓冲区
    cbResize.proj = XMMatrixTranspose(pCamera->GetProjXM());
    // 初始化不会变化的值
    // 环境光
    cbRare.dirLight[0].ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    cbRare.dirLight[0].diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    cbRare.dirLight[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    cbRare.dirLight[0].direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
    // 灯光
    cbRare.pointLight[0].position = XMFLOAT3(0.0f, 10.0f, 0.0f);
    cbRare.pointLight[0].ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    cbRare.pointLight[0].diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    cbRare.pointLight[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    cbRare.pointLight[0].att = XMFLOAT3(0.0f, 0.1f, 0.0f);
    cbRare.pointLight[0].range = 25.0f;
    cbRare.numDirLight = 1;
    cbRare.numPointLight = 1;
    cbRare.numSpotLight = 0;
    // 初始化材质
    cbRare.material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    cbRare.material.diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
    cbRare.material.specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 50.0f);

    UpdateConstantBuffer(pConstantBuffers[3], &cbRare, sizeof(cbRare));
    UpdateConstantBuffer(pConstantBuffers[2], &cbResize, sizeof(cbResize));

    // 渲染管线
    pImmediateDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    pImmediateDeviceContext->VSSetConstantBuffers(0, 1, pConstantBuffers[0].GetAddressOf());
    pImmediateDeviceContext->VSSetConstantBuffers(1, 1, pConstantBuffers[1].GetAddressOf());
    pImmediateDeviceContext->VSSetConstantBuffers(2, 1, pConstantBuffers[2].GetAddressOf());
    
    pImmediateDeviceContext->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());
    pImmediateDeviceContext->PSSetConstantBuffers(1, 1, pConstantBuffers[1].GetAddressOf());
    pImmediateDeviceContext->PSSetConstantBuffers(3, 1, pConstantBuffers[3].GetAddressOf());

    return true;
}
bool GameApp::Init() {
    return
        D3DApp::Init() &&
        InitEffect() &&
        InitResource();
}