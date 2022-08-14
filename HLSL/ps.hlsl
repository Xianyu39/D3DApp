#include "Vertex.hlsli"

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



void ComputeDirectionalLight(Material mat, DirectionalLight L,
    float3 normal, float3 toEye,
    out float4 ambient,
    out float4 diffuse,
    out float4 spec)
{
    // ��ʼ�����
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // �����������䷽���෴
    float3 lightVec = -L.direction;

    // ��ӻ�����
    ambient = mat.ambient * L.ambient;

    // ����������;����
    float diffuseFactor = dot(lightVec, normal);

    // չ�������⶯̬��֧
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

        diffuse = diffuseFactor * mat.diffuse * L.diffuse;
        spec = specFactor * mat.specular * L.specular;
    }
}


void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,
    out float4 ambient, out float4 diffuse, out float4 spec)
{
    // ��ʼ�����
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // �ӱ��浽��Դ������
    float3 lightVec = L.position - pos;

    // ���浽���ߵľ���
    float d = length(lightVec);

    // �ƹⷶΧ����
    if (d > L.range)
        return;

    // ��׼��������
    lightVec /= d;

    // ���������
    ambient = mat.ambient * L.ambient;

    // ������;������
    float diffuseFactor = dot(lightVec, normal);

    // չ���Ա��⶯̬��֧
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

        diffuse = diffuseFactor * mat.diffuse * L.diffuse;
        spec = specFactor * mat.specular * L.specular;
    }

    // ���˥��
    float att = 1.0f / dot(L.att, float3(1.0f, d, d * d));

    diffuse *= att;
    spec *= att;
}


void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye,
    out float4 ambient, out float4 diffuse, out float4 spec)
{
    // ��ʼ�����
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // // �ӱ��浽��Դ������
    float3 lightVec = L.position - pos;

    // ���浽��Դ�ľ���
    float d = length(lightVec);

    // ��Χ����
    if (d > L.range)
        return;

    // ��׼��������
    lightVec /= d;

    // ���㻷���ⲿ��
    ambient = mat.ambient * L.ambient;


    // �����������;��淴��ⲿ��
    float diffuseFactor = dot(lightVec, normal);

    // չ���Ա��⶯̬��֧
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

        diffuse = diffuseFactor * mat.diffuse * L.diffuse;
        spec = specFactor * mat.specular * L.specular;
    }

    // ���������Ӻ�˥��ϵ��
    float spot = pow(max(dot(-lightVec, L.direction), 0.0f), L.Spot);
    float att = spot / dot(L.att, float3(1.0f, d, d * d));

    ambient *= spot;
    diffuse *= att;
    spec *= att;
}

cbuffer PSConstantBuffer : register(b1)
{
    DirectionalLight g_DirLight[10];
    PointLight g_PointLight[10];
    SpotLight g_SpotLight[10];
    Material g_Material;
    int g_NumDirLight;
    int g_NumPointLight;
    int g_NumSpotLight;
    float g_Pad1;

    float3 g_EyePosW;
    float g_Pad2;
}
Texture2D gTex:register(t0);
Texture2D gTexMask:register(t1);

SamplerState gSamLinear:register(s0);

float4 main(VertexOut v):SV_Target{
    float4 ambient, diffuse, specular, A,D,S;
    v.normalW = normalize(v.normalW);
    int i=0;
    for(i=0; i<g_NumDirLight; ++i){
        ComputeDirectionalLight(g_Material, g_DirLight[i], v.normalW, normalize(g_EyePosW-v.posW), A, D,S);
        ambient+=A;
        diffuse+=D;
        specular+=S;
    }
    for(i=0; i<g_NumPointLight; ++i){
        ComputePointLight(g_Material, g_PointLight[i],v.posW,  v.normalW, normalize(g_EyePosW-v.posW), A, D,S);
        ambient+=A;
        diffuse+=D;
        specular+=S;
    }
    for(i=0; i<g_NumSpotLight; ++i){
        ComputeSpotLight(g_Material, g_SpotLight[i], v.posW ,v.normalW, normalize(g_EyePosW-v.posW), A, D,S);
        ambient+=A;
        diffuse+=D;
        specular+=S;
    }

    float4 texColor = gTex.Sample(gSamLinear, v.tex);
    texColor *= gTexMask.Sample(gSamLinear, v.tex);

    float4 color = texColor*(diffuse+ambient)+specular;
    color.a = texColor.a*g_Material.diffuse.a;
    return color;
}