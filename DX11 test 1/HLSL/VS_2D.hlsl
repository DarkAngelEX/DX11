#include"Basic.hlsli"


// ¶¥µã×ÅÉ«Æ÷(2D)
VertexPosHTex VS_2D(VertexPosTex vIn)
{
    VertexPosHTex vOut;
    vOut.PosH = float4(vIn.PosL, 1.0f) + cpos;
    vOut.Tex = vIn.Tex;
    vOut.Id = 1;
    return vOut;
}
