#include "Vignette.hlsli"

Texture2D<float4> tex : register(t0);	//0番スロットに設定されたテクスチャ

SamplerState smp      : register(s0);	//0番スロットに設定されたサンプラー

//ビネット通常
float4 main(VSOutput input) : SV_TARGET
{
    float2 samplePoint = input.uv;
    float4 texColor = tex.Sample(smp, samplePoint);
    //真ん中にビネット
    float vignette = length(float2(0.5f, 0.5f) - input.uv);
    float power = 0.05f; //小さいほど強い
    vignette = clamp(vignette - power, 0.0f, 1.0f);
    texColor.rgb -= vignette * (1.0f - color.rgb);
    return texColor;
}
//ビネット赤
//float4 main(VSOutput input) : SV_TARGET
//{
//    float2 samplePoint = input.uv;
//    float4 texColor = tex.Sample(smp, samplePoint);
//    //真ん中にビネット
//    float vignette = length(float2(0.5f, 0.5f) - input.uv);
//    float power = 0.2f; //小さいほど強い
//    vignette = clamp(vignette - power, 0.0f, 1.0f);
//    texColor.gb -= vignette;
//    return texColor;
//}