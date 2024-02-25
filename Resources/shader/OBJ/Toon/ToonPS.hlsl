#include "Toon.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//トゥーンシェーディング
    float4 texcolor = tex.Sample(smp, input.uv) * (1.0f-color);
	//光沢度
    const float shininess = 4.0f;
	//頂点から視点への方向ベクトル
    float3 eyeDir = normalize(cameraPos - input.worldPos.xyz);
	//環境反射光
    float3 ambient = m_ambient;
    //シェーディングカラー
    float4 shade_color = float4(ambientColor * ambient, m_alpha);
    
    for (int i = 0; i < DIR_LIGHT_NUM; i++)
    {
        if (dirLights[i].active)
        {
	        //ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(dirLights[i].lightv, input.normal);
	        //反射光ベクトル
            float3 reflect = normalize(dirLights[i].lightv + 2.0f * dotlightnormal * input.normal);
   
	        //拡散反射光
            float3 diffuse = dotlightnormal * m_diffuse;
	        //鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyeDir)), shininess) * m_specular;
            //全加算
            shade_color.rgb += (diffuse + specular) * dirLights[i].lightcolor;
            shade_color.a = m_alpha;
        }
    }
    //点光源
    for (i = 0; i < POINTLIGHT_NUM; i++)
    {
        if (pointLights[i].active)
        {
	        //ライトへのベクトル
            float3 lightv = pointLights[i].lightpos - input.worldPos.xyz;
            //ベクトルの長さ
            float d = length(lightv);
            //正規化し、単位ベクトルにする
            lightv = normalize(lightv);
            //距離減衰係数
            float atten = 1.0f / (pointLights[i].lightatten.x
            + pointLights[i].lightatten.y * d + pointLights[i].lightatten.z * d * d);
            //ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightv, input.normal);
            //反射光ベクトル
            float3 reflect = normalize(-lightv + 2.0f * dotlightnormal * input.normal);
            //拡散反射光
            float3 diffuse = dotlightnormal * m_diffuse;
            //鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyeDir)), shininess) * m_specular;
            //全加算
            shade_color.rgb += atten * (diffuse + specular) * pointLights[i].lightcolor;
            shade_color.a = m_alpha;
        }
    }
    //スポットライト
    for (i = 0; i < SPOTLIGHT_NUM; i++)
    {
        if (spotLights[i].active)
        {
            //ライトへの方向ベクトル
            float3 lightv = spotLights[i].lightpos - input.worldPos.xyz;
            float d = length(lightv);
            lightv = normalize(lightv);
            //距離減衰係数
            float atten = saturate(1.0f / (spotLights[i].lightatten.x + spotLights[i].lightatten.y * d +
            spotLights[i].lightatten.z * d * d));
            //角度減衰
            float cos = dot(lightv, spotLights[i].lightv);
            //減衰開始角度から、減衰終了角度にかけて減衰
            //減衰開始角度の内側は1倍、減衰終了角度の外側は0倍の輝度
            float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y, spotLights[i].lightfactoranglecos.x, cos);
            //角度減衰を乗算
            atten *= angleatten;
            //ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightv, input.normal);
            //反射光ベクトル
            float3 reflect = normalize(-lightv + 2.0f * dotlightnormal * input.normal);
            //拡散反射光
            float3 diffuse = dotlightnormal * m_diffuse;
            //鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyeDir)), shininess) * m_specular;
            //全て加算する
            shade_color.rgb += atten * (diffuse + specular) * spotLights[i].lightcolor;
            
        }
    }
    //丸影
    for (i = 0; i < CIRCLESHADOW_NUM; i++)
    {
        if (circleShadows[i].active)
        {
            //オブジェクト表面からキャスターへのベクトル
            float3 casterv = circleShadows[i].casterPos - input.worldPos.xyz;
            //投影方向での距離
            float d = dot(casterv, circleShadows[i].dir);
            //距離減衰係数
            float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y * d + circleShadows[i].atten.z * d * d));
            //距離がマイナスなら0にする
            atten *= step(0.0f, d);
            //仮想ライトの座標
            float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir * circleShadows[i].distanceCasterLight;
            //オブジェクト表面からライトへのベクトル(単位ベクトル)
            float3 lightv = normalize(lightpos - input.worldPos.xyz);
            //角度減衰
            float cos = dot(lightv, circleShadows[i].dir);
            //減衰開始角度から、減衰終了角度にかけて減衰
            //減衰開始角度の内側は1倍、減衰終了角度の外側は0倍の輝度
            float angleatten = smoothstep(circleShadows[i].factorAngleCos.y, circleShadows[i].factorAngleCos.x, cos);
            //角度減衰を乗算
            atten *= angleatten;
            //全て減算する
            shade_color.rgb -= atten;
        }
    }
	//シェーディングカラーで描画
    return shade_color * texcolor;
}