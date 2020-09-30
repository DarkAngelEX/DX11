#if 1
cbuffer VSConstantBuffer : register(b0)
{
	float4 cpos;
}
#endif

struct VertexIn
{
	float3 pos : POSITION;
	float4 color : COLOR;
};



struct VertexOut
{
	float4 posH : SV_POSITION;
	float4 color : COLOR;
};


