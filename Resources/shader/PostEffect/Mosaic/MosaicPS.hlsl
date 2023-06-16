#include "Mosaic.hlsli"

Texture2D<float4> tex : register(t0);	//0番スロットに設定されたテクスチャ
SamplerState smp      : register(s0);	//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    //正方形モザイク
    float2 windowsSize = { 1280.0f, 720.0f };
    
    float density = 20.0f;
    float4 col = tex.Sample(smp, floor(input.uv * windowsSize / density) * density / windowsSize);
    return col;
}

/*
長方形モザイク
float4 main(VSOutput input) : SV_TARGET
{
    float density = 100;
    float4 col = tex.Sample(smp, floor(input.uv * density) / density);
}
*/