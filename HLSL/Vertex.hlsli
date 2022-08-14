struct VertexIn{
    float3 pos:POSITION;
    float3 normal:NORMAL;
    float2 tex:TEXCOORD;
};

struct VertexOut{
    float4 posH:SV_POSITION;
    float3 posW:POSITION;
    float3 normalW:NORMAL;
    float2 tex:TEXCOORD;
    float4 color:COLOR;
};