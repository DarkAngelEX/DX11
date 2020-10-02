#include "MapBase.hlsli"
float4 PS(VertexPS pIn) : SV_TARGET
{
	return g_Tex.Sample(g_SamLinear, float3(pIn.Tex, pIn.Id));
	//return float4(1,1,1,0);
}