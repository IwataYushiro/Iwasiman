#include "FBX.hlsli"
//0番スロットに設定されたテクスチャ
Texture2D<float4> tex : register(t0);
//0番スロットに設定されたサンプラー
SamplerState smp : register(s0);

struct PSOutPut
{
    float4 target0 : SV_TARGET0;
    float4 target1 : SV_TARGET1;
};
//エントリーポイント
PSOutPut main(VSOutput input)
{
    PSOutPut output;
	//テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv);
	//ランバート反射
    float3 light = normalize(float3(1.0f, -1.0f, 1.0f));//右下奥向きのライト
    float diffuse = saturate(dot(-light, input.normal));
    float brightness = diffuse + 0.3f;
    float4 shadecolor = float4(brightness, brightness, brightness, 1.0f);
	//陰影とテクスチャの色を合成
    output.target0 = shadecolor * texcolor;
    output.target1 = float4(1.0f - (shadecolor * texcolor).rgb, 1.0f);
    return output;
}