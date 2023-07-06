#include "Toon.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//ランバート反射
	//float3 light = normalize(float3(1,-1,1)); // 右下奥　向きのライト
	//float light_diffuse = saturate(dot(-light, input.normal));
	//float shade_color;
	//shade_color = m_ambient;					//アンビエント値
	//shade_color += m_diffuse * light_diffuse;	//ディフューズ値
	//float4 texcolor = tex.Sample(smp, input.uv);
	//return float4(texcolor.rgb * shade_color, texcolor.a * m_alpha);
	
	//フォンシェーディング
    float4 texcolor = tex.Sample(smp, input.uv);
	//光沢度
    const float shininess = 4.0f;
	//頂点から視点への方向ベクトル
    float3 eyeDir = normalize(cameraPos - input.worldPos.xyz);
	//環境反射光
    float3 ambient = m_ambient;
    //シェーディングカラー
    float4 shade_color = float4(ambientColor * ambient, m_alpha);
    
    for (uint i = 0; i < DIR_LIGHT_NUM; i++)
    {
        if (dirLights[i].active)
        {
	        //ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = smoothstep(0.3f, 0.35f, saturate(dot(dirLights[i].lightv, input.normal)));
	        //反射光ベクトル
            float3 reflect = normalize(dirLights[i].lightv + 2.0f * dotlightnormal * input.normal);
   
	        //拡散反射光
            float3 diffuse = dotlightnormal * m_diffuse;
	        //鏡面反射光
            float3 specular = smoothstep(0.5f,0.55f,pow(saturate(dot(reflect, eyeDir)), shininess)) * m_specular;
            //全加算
            shade_color.rgb += (1.0f - dotlightnormal) * dirLights[i].lightcolor+(diffuse + specular)  ;
            shade_color.a = m_alpha;
        }
    }
	//シェーディングカラーで描画
    return shade_color * texcolor;
}