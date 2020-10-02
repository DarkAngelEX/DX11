#include "MapBase.hlsli"
VertexPS VS(VertexVS vIn)
{
	VertexPS vOut;
	vOut.PosH = float4(vIn.PosL, 1.0f);
	vOut.Tex = vIn.Tex;
	vOut.Id = vIn.Id;
	return vOut;
}
