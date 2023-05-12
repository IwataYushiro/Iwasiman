#include "OBJ.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//float3 light = normalize(float3(1,-1,1)); // 右下奥　向きのライト
	//float light_diffuse = saturate(dot(-light, input.normal));
	//float shade_color;
	//shade_color = m_ambient;					//アンビエント値
	//shade_color += m_diffuse * light_diffuse;	//ディフューズ値
	//float4 texcolor = tex.Sample(smp, input.uv);
	//return float4(texcolor.rgb * shade_color, texcolor.a * m_alpha);
	 
    float4 texcolor = tex.Sample(smp, input.uv);
    //シェーディングカラー
	float4 shade_color;
	//光沢度
    const float shininess = 4.0f;
	//頂点から視点への方向ベクトル
    float3 eyeDir = normalize(cameraPos - input.worldPos.xyz);
    //ライトに向かうベクトルと法線の内積
    float3 dotlightnormal = dot(lightv, input.normal);
	//反射光ベクトル
    float3 reflect = normalize(-lightv + 2.0f * dotlightnormal * input.normal);
    //環境反射光
    float3 ambient = m_ambient;
	//拡散反射光
    float3 diffuse = dotlightnormal * m_diffuse;
	//鏡面反射光
    float3 specular = pow(saturate(dot(reflect, eyeDir)), shininess) * m_specular;
    //全加算
    shade_color.rgb = (ambient + diffuse + specular) * lightcolor;
    shade_color.a = m_alpha;
	
	//シェーディングカラーで描画
    return shade_color *texcolor;
	
}