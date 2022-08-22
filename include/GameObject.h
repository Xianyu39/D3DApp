#if !defined(GAMEOBJECT_H)
#define GAMEOBJECT_H

#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Transform.h"
#include "Geometry.h"
#include "DXTrace.h"

class GameObject {
protected:
    template<class T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;
    Transform transform;
    ComPtr<ID3D11ShaderResourceView> pTexture;
    ComPtr<ID3D11Buffer> pVertexBuffer, pIndexBuffer;
    UINT vertexStride;
    UINT indexCount;

public:
    GameObject();
    ~GameObject() = default;
    // 获取位置
    XMFLOAT3 GetPosition() const { return transform.GetPosition(); }
    Transform& GetTransform() { return transform; }
    // 设置缓冲区
    template<class VertexType, class IndexType>
    void SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData) {
        D3D11_BUFFER_DESC dsc;
        ZeroMemory(&dsc, sizeof(dsc));
        dsc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        dsc.Usage = D3D11_USAGE_IMMUTABLE;
        dsc.CPUAccessFlags = 0;
        dsc.ByteWidth = sizeof(VertexType) * meshData.vertexVec.size();
        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = meshData.vertexVec.data();
        HR(device->CreateBuffer(&dsc, &initData, pVertexBuffer.ReleaseAndGetAddressOf()));

        dsc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        dsc.ByteWidth = sizeof(IndexType) * meshData.indexVec.size();
        initData.pSysMem = meshData.indexVec.data();
        HR(device->CreateBuffer(&dsc, &initData, pIndexBuffer.ReleaseAndGetAddressOf()));

        vertexStride = sizeof(VertexType);
        indexCount = meshData.indexVec.size();
    }
    // 设置纹理
    void SetTexture(ID3D11ShaderResourceView* texture);
    // 设置矩阵
    void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);
    void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);
    // 绘制
    void Draw(ID3D11DeviceContext* deviceContext);

    // 设置调试对象名
    // 若缓冲区被重新设置，调试对象名也需要被重新设置
    void SetDebugObjectName(const std::string& name);
};

#endif // GAMEOBJECT_H
