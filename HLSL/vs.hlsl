#include "Vertex.hlsli"

cbuffer VCbuffer:register(b0){
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float4x4 worldInv;
    float4x4 texTrans;
};

VertexOut main(VertexIn v){
    VertexOut ans;
    matrix viewproj = mul(view, proj);
    float4 posW = mul(float4(v.pos, 1.f), world);
    ans.posH = mul(posW, viewproj);
    ans.posW = posW.xyz;
    ans.tex = mul(float4(v.tex-0.5, 0, 1), texTrans).xy;
    ans.tex += 0.5;
    ans.normalW = mul(v.normal, (float3x3)worldInv);
    ans.color = float4(normalize(v.normal), 1.0f);
    return ans;
}