#include "Vertex.hlsli"
#include "Constants.hlsli"

VertexOut main(VertexIn v){
    VertexOut ans;
    matrix viewproj = mul(view, proj);
    float4 posW = mul(float4(v.pos, 1.f), world);
    ans.posH = mul(posW, viewproj);
    ans.posW = posW.xyz;
    ans.tex = v.tex;
    ans.normalW = mul(v.normal, (float3x3)worldInv);
    ans.color = float4(normalize(v.normal), 1.0f);
    return ans;
}