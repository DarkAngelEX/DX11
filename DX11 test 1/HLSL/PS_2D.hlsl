#include"Basic.hlsli"

// ÏñËØ×ÅÉ«Æ÷(2D)
float4 PS_2D(VertexPosHTex pIn) : SV_Target
{
    //return g_Tex.Sample(g_SamLinear, pIn.Tex);


    //if(pIn.PrimID > 0.5) return g_Tex.Sample(g_SamLinear, 1);
    //else return g_Tex.Sample(g_SamLinear, 0);


    return g_Tex.Sample(g_SamLinear, float3(pIn.Tex, pIn.Id));
    //return g_Tex.Sample(g_SamLinear, float3(pIn.Tex, pIn.PrimID));


    //return g_Tex[1].Sample(g_SamLinear, pIn.Tex);
    //return g_Tex.Sample(g_SamLinear, pIn.Tex);
}