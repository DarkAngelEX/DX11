#include "Triangle.hlsli"

// ������ɫ��
VertexOut VS(VertexIn vIn)
{
    VertexOut vOut;
    vOut.posH = float4(vIn.pos, 1.0f) + cpos;
    vOut.color = vIn.color; // ����alphaͨ����ֵĬ��Ϊ1.0
    //vOut.color.a = 0.5;
    return vOut;
}
