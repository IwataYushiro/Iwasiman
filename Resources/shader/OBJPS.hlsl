#include "OBJ.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

float4 Lambert(VSOutput input)
{
    //ランバート反射
    float3 light = normalize(float3(1, -1, 1)); // 右下奥　向きのライト
    float light_diffuse = saturate(dot(-light, input.normal));
    float shade_color;
    shade_color = m_ambient; //アンビエント値
    shade_color += m_diffuse * light_diffuse; //ディフューズ値
    float4 texcolor = tex.Sample(smp, input.uv);
    return float4(texcolor.rgb * shade_color, texcolor.a * m_alpha);
}

float4 Phong(VSOutput input)
{
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
	//シェーディングカラーで描画
    return shade_color * texcolor;
}

float4 cold(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    float cold = 0.2f;
    float grayScale = texcolor.r * 0.299f + texcolor.g * 0.587f + texcolor.b * 0.114f;
    float4 coldColor = float4(grayScale - cold, grayScale, grayScale + cold, 1.0f);
    return coldColor;
}

float4 highLumi(VSOutput input)
{
    float4 col = tex.Sample(smp, input.uv);
    float grayScale = col.r * 0.299f + col.g * 0.587f + col.b * 0.114f;
    float extract = smoothstep(0.6f, 0.9f, grayScale);
    return col * extract;
}

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 GaussianBlur(VSOutput input) : SV_TARGET
{
    float totalWeight = 0.0f, _Sigma = 0.005f, _StepWidth = 0.005f;
    float4 col = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (float py = -_Sigma * 2.0f; py <= _Sigma * 2.0f; py += _StepWidth)
    {
				//[loop]、[unroll]は書かなくていい
        for (float px = -_Sigma * 2.0f; px <= _Sigma * 2.0f; px += _StepWidth)
        {
            float2 pickUV = input.uv + float2(px, py);
            float weight = Gaussian(input.uv, pickUV, _Sigma);
            col += saturate(tex.Sample(smp, pickUV)) * weight;
            totalWeight += weight;
        }

    }
    col.rgb = col.rgb / totalWeight;
    //col.a = 1.0f;
    return col;
}

float4 Bloom(VSOutput input) : SV_TARGET
{
    float4 texcolor0 = GaussianBlur(input);
    float4 texcolor1 = highLumi(input);
    return texcolor0 + texcolor1;
}

float4 Mosaic(VSOutput input) : SV_TARGET
{
    //正方形モザイク
    float2 windowsSize = { 1280.0f, 720.0f };
    
    float density = 20.0f;
    float4 col = tex.Sample(smp, floor(input.uv * windowsSize / density) * density / windowsSize);
    return col;
}


float4 main(VSOutput input) : SV_TARGET
{
    float4 shade = Phong(input) * Bloom(input);
    return shade;
}