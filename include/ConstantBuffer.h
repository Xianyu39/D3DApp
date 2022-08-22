#if !defined(CONSTANRBUFFER_H)
#define CONSTANRBUFFER_H

#include <DirectXMath.h>
using namespace DirectX;

using float3 = XMFLOAT3;
using float2 = XMFLOAT2;
using float4 = XMFLOAT4;

// �����
struct DirectionalLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float3 direction;
    float pad;
};

// ���
struct PointLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 position;
    float range;

    float3 att;
    float pad;
};

// �۹��
struct SpotLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 position;
    float range;

    float3 direction;
    float Spot;

    float3 att;
    float pad;
};

// ����������
struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular; // w = SpecPower
    float4 reflect;
};

struct CBChangesEveryDrawing
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX worldInvTranspose;
};

struct CBChangesEveryFrame
{
    DirectX::XMMATRIX view;
    DirectX::XMFLOAT4 eyePos;
};

struct CBChangesOnResize
{
    DirectX::XMMATRIX proj;
};

struct CBChangesRarely
{
    DirectionalLight dirLight[10];
    PointLight pointLight[10];
    SpotLight spotLight[10];
    Material material;
    int numDirLight;
    int numPointLight;
    int numSpotLight;
    float pad;		// �����֤16�ֽڶ���
};

#endif // CONSTANRBUFFER_H