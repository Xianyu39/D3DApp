#if !defined(VERTEX_H)
#define VERTEX_H

#include <DirectXMath.h>
#include <d3d11.h>

using namespace DirectX;

struct VertexPosColor {
    XMFLOAT3 pos;
    XMFLOAT4 color;
    static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};

struct VertexPosTex {
    XMFLOAT3 pos;
    XMFLOAT2 tex;
    static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};

struct VertexPosNormalTex {
    XMFLOAT3 pos;
    XMFLOAT3 normal;
    XMFLOAT2 tex;
    static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};

#endif // VERTEX_H
