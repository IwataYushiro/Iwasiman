#include "CrossFilter.hlsli"

Texture2D<float4> tex0 : register(t0); //0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1); //0番スロットに設定されたテクスチャ

SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 highLumi(Texture2D<float4> tex, VSOutput input)
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

float4 GaussianBlur(Texture2D<float4> tex, VSOutput input,float angle) : SV_TARGET
{
    float totalWeight = 0.0f;
    float4 col = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float2 pickUV = float2(0.0f, 0.0f);
    float pickRange = 0.06f;
    float a = angle * 3.141592f / 180.0f;
    
    for (float py = -pickRange; py <= pickRange; py += 0.005f)
    {
        float x = cos(a) * py;
        float y = sin(a) * py;
        pickUV = input.uv + float2(x, y);
        
        float weight = Gaussian(input.uv, pickUV, pickRange);
        col += saturate(tex.Sample(smp, pickUV)) * weight;
        totalWeight += weight;
    }
    col = col / totalWeight;
    //col.a = 1.0f;
    return col;
}

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor0 = GaussianBlur(tex0, input, 45.0f)+GaussianBlur(tex0, input, 135.0f);
    float4 texcolor1 = highLumi(tex1, input);
    return texcolor0 + texcolor1;
}

