#include "FBX.hlsli"
//0番スロットに設定されたテクスチャ
Texture2D<float4> tex : register(t0);
//0番スロットに設定されたサンプラー
SamplerState smp : register(s0);
//エントリーポイント
float4 main(VSOutput input) : SV_TARGET
{
	//テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv);
	//ランバート反射
    float3 light = normalize(float3(1.0f, -1.0f, 1.0f));//右下奥向きのライト
    float diffuse = saturate(dot(-light, input.normal));
    float brightness = diffuse + 0.3f;
    float4 shadecolor = float4(brightness, brightness, brightness, 1.0f);
	//陰影とテクスチャの色を合成
    return shadecolor * texcolor;
}