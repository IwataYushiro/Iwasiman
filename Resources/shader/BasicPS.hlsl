#include "Basic.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp      : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = float4(tex.Sample(smp,input.uv));

	float3 light = normalize(float3(1.0f,-1.0f,1.0f));				//右下奥　向きのライト
	float diffuse = saturate(dot(-light, input.normal));			//diffuseを[0,1]の範囲にてclampする
	float brightness = diffuse + 0.3f;								//光源へのベクトルと法線ベクトルの内積
	
	return float4(texcolor.rgb * brightness, texcolor.a) * color;	//輝度をRGBに代入して出力
}

//float4(赤,緑,青,α値(1)) 範囲　0.0f～1.0f