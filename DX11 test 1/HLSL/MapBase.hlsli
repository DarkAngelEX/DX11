//纹理数组
Texture2DArray g_Tex : register(t0);
//线性采样器
SamplerState g_SamLinear : register(s0);

struct VertexVS
{
    float3 PosL : POSITION;
    float2 Tex : TEXCOORD;
    float Id : ID;
};

struct VertexPS
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD;
    float Id : ID;
};


