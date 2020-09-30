
Texture2DArray g_Tex : register(t0);
//Texture2D g_Tex[2] : register(t0);

SamplerState g_SamLinear : register(s0);

cbuffer VSConstantBuffer : register(b0)
{
    float4 cpos;
}

struct VertexPosTex
{
    float3 PosL : POSITION;
    float2 Tex : TEXCOORD;
};

struct VertexPosHTex
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD;
    float PrimID : ID;
};


