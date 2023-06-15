#include "Test.hlsli"

Texture2D<float4> tex : register(t0);	//0番スロットに設定されたテクスチャ
SamplerState smp      : register(s0);	//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    return float4(texcolor.rgb, 1.0f);
}

/*
ベース
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    return float4(texcolor.rgb, 1.0f);

}

明度変更
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
     return float4(texcolor.rgb * 2.0f, 1.0f);

}
緑だけ変更
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    return float4(texcolor.r, texcolor.g * 2.0f, texcolor.b, 1.0f);

}

色反転(ネガポジ?)
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    return float4(float3(1.0f, 1.0f, 1.0f) - texcolor.rgb, 1.0f);

}

UVずらし(サンプラー＝D3D12_TEXTURE_ADDRESS_MODE_WRAP限定？)
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv + float2(0.5f,0.0f));
    return float4(texcolor.rgb, 1.0f);

}

グレースケール
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    float grayScale = texcolor.r * 0.299f + texcolor.g * 0.587f + texcolor.b * 0.114f;
    float4 gray = float4(grayScale, grayScale, grayScale, 1.0f);
    return gray;

}





*/