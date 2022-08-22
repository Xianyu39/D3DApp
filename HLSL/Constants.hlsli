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

cbuffer CBChangesEveryDrawing : register(b0)
{
    float4x4 world;
    float4x4 worldInv;
};
cbuffer CBChangesEveryFrame : register(b1)
{
    float4x4 view;
    float4 eyePos;
};
cbuffer CBChangesOnResize : register(b2)
{
    float4x4 proj;
};
cbuffer CBChangesRarely : register(b3)
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