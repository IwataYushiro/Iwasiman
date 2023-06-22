#include "Toon.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{ 
    ////テクスチャ
    //float4 texcolor = tex.Sample(smp, input.uv);
    ////シェーディングカラー 
    //float shade_color;
    ////光沢度
    //const float shininess = 4.0f;
    ////頂点から視点への方向ベクトル
    //float3 eyeDir = normalize(cameraPos - input.worldPos.xyz);
    ////ライトに向かうベクトルと法線の内積
    ////float3 dotlightnormal=dot(lightv,input.normal)
    ////反射光ベクトル
    
    
    //float3 light = normalize(float3(-1.0f, -1.0f, 1.0f)); // 右下奥　向きのライト
    //float light_diffuse = saturate(dot(-light, input.normal));
   
    //float intensity = smoothstep(0.3f, 0.35f, saturate(dot(normalize(input.normal), light_diffuse)));
    
    //shade_color = (1.0f - intensity) * m_ambient; //アンビエント値
    //shade_color += m_diffuse * light_diffuse; //ディフューズ値
   
    //return float4(texcolor.rgb * shade_color, texcolor.a * m_alpha);
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}