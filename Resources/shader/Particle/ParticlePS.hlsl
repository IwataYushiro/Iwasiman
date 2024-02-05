#include "Particle.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(GSOutput input) : SV_TARGET
{
    //ソフトパーティクル…？
    //float2 fetchUV = input.svpos.xy;
    //fetchUV.xy /= input.svpos.w;
    //fetchUV.x = 0.5f * (fetchUV.x + 1.0f);
    //fetchUV.y = -0.5f * (fetchUV.y - 1.0f);
    //float vz = input.svpos.z / input.svpos.w;
    //float z = tex.Sample(smp, fetchUV.xy);
    //float d = abs(vz - z);
    //float threshold = 0.00075f;
    //if (d < threshold)
    //    d = d / threshold;
    //else
    //    d = 1.0f;
    //float4 color = tex.Sample(smp, input.uv) * input.col;
    //color.a *= d;
    //return color;
    
    //デフォルト
    return tex.Sample(smp, input.uv) * input.col;
	//return float4(1, 1, 1, 1);
}
