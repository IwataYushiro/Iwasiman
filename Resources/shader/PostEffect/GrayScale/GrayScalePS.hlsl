#include "GrayScale.hlsli"

Texture2D<float4> tex : register(t0);	//0番スロットに設定されたテクスチャ
SamplerState smp      : register(s0);	//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    float grayScale = (texcolor.r * 0.299f + texcolor.g * 0.587f + texcolor.b * 0.114f) * power; //power=強さ
    float4 gray = float4(grayScale, grayScale, grayScale, 1.0f);
    return gray + (texcolor * (1.0f - power));//パワーが上がるにつれ徐々に元の色に
}
