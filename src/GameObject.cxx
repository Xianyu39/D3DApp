#include "GameObject.h"
#include "ConstantBuffer.h"

using namespace DirectX;

GameObject::GameObject()
    :transform(),
    pTexture(nullptr),
    pVertexBuffer(nullptr),
    pIndexBuffer(nullptr)
{
    
}
// template<class VertexType, class IndexType>
// void GameObject::SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData) {
//     D3D11_BUFFER_DESC dsc;
//     ZeroMemory(&dsc, sizeof(dsc));
//     dsc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//     dsc.Usage = D3D11_USAGE_IMMUTABLE;
//     dsc.CPUAccessFlags = 0;
//     dsc.ByteWidth = sizeof(VertexType) * meshData.vertexVec.size();
//     D3D11_SUBRESOURCE_DATA initData;
//     initData.pSysMem = meshData.vertexVec.data();
//     HR(device->CreateBuffer(&dsc, &initData, pVertexBuffer.ReleaseAndGetAddressOf()));
//     vertexStride = sizeof(VertexType);

//     dsc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//     dsc.ByteWidth = sizeof(IndexType) * meshData.indexVec.size();
//     initData.pSysMem = meshData.indexVec.data();
//     HR(device->CreateBuffer(&dsc, &initData, pIndexBuffer.ReleaseAndGetAddressOf()));
//     indexCount = meshData.indexVec.size();
// }
// 设置纹理
void GameObject::SetTexture(ID3D11ShaderResourceView* texture) { pTexture = texture; }
// 设置矩阵
void GameObject::SetWorldMatrix(const DirectX::XMFLOAT4X4& world) {
    
}
void XM_CALLCONV GameObject::SetWorldMatrix(DirectX::FXMMATRIX world){}
// 绘制
void GameObject::Draw(ID3D11DeviceContext* deviceContext) {
    UINT strides = vertexStride;
    UINT offsets = 0;
    deviceContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &vertexStride, &offsets);
    deviceContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    ComPtr<ID3D11Buffer> cb;
    deviceContext->VSGetConstantBuffers(0, 1, cb.GetAddressOf());
    CBChangesEveryDrawing cbDrawing;
    // 内部进行转置
    XMMATRIX W = transform.GetLocalToWorldMatrixXM();
    cbDrawing.world = XMMatrixTranspose(W);
    W.r[3] = g_XMIdentityR3;
    cbDrawing.worldInvTranspose = XMMatrixTranspose(XMMatrixTranspose(XMMatrixInverse(nullptr, W)));

    D3D11_MAPPED_SUBRESOURCE mapped;
    deviceContext->Map(cb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy_s(mapped.pData, sizeof(cbDrawing), &cbDrawing, sizeof(cbDrawing));
    deviceContext->Unmap(cb.Get(), 0);

    deviceContext->PSSetShaderResources(0, 1, pTexture.GetAddressOf());
    deviceContext->DrawIndexed(indexCount, 0, 0);
}

// 设置调试对象名
// 若缓冲区被重新设置，调试对象名也需要被重新设置
void GameObject::SetDebugObjectName(const std::string& name){}