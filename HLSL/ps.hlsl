#include "Vertex.hlsli"
#include "Constants.hlsli"

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

Texture2D gTex:register(t0);

SamplerState gSamLinear:register(s0);

float4 main(VertexOut v):SV_Target{
    float4 ambient, diffuse, specular, A,D,S;
    v.normalW = normalize(v.normalW);
    int i=0;
    for(i=0; i<numDirLight; ++i){
        ComputeDirectionalLight(material, dirLight[i], v.normalW, normalize(eyePos.xyz -v.posW), A, D,S);
        ambient+=A;
        diffuse+=D;
        specular+=S;
    }
    for(i=0; i<numPointLight; ++i){
        ComputePointLight(material, pointLight[i],v.posW,  v.normalW, normalize(eyePos.xyz-v.posW), A, D,S);
        ambient+=A;
        diffuse+=D;
        specular+=S;
    }
    for(i=0; i<numSpotLight; ++i){
        ComputeSpotLight(material, spotLight[i], v.posW ,v.normalW, normalize(eyePos.xyz-v.posW), A, D,S);
        ambient+=A;
        diffuse+=D;
        specular+=S;
    }

    float4 texColor = gTex.Sample(gSamLinear, v.tex);
    // texColor *= gTexMask.Sample(gSamLinear, v.tex);

    float4 color = texColor*(diffuse+ambient)+specular;
    color.a = texColor.a*material.diffuse.a;
    return color;
}