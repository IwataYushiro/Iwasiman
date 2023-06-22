#include "Test.hlsli"

Texture2D<float4> tex0 : register(t0);	//0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1); //1番スロットに設定されたテクスチャ

SamplerState smp      : register(s0);	//0番スロットに設定されたサンプラー

float4 Blur(Texture2D<float4> tex, VSOutput input)
{
    float _ShiftWidth = 0.005f;
    float _ShiftNumX = 4.0f; //num x num
    float _ShiftNumY = 5.0f; //num x num
    
    float4 col = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float num = 0;
    for (float py = -_ShiftNumY / 2.0f; py <= _ShiftNumY / 2.0f; py++)
    {
				//[loop]、[unroll]は書かなくていい
        for (float px = -_ShiftNumX / 2.0f; px <= _ShiftNumX / 2.0f; px++)
        {
            col += tex.Sample(smp, input.uv + float2(px, py) * _ShiftWidth);
            num++;
        }
    }
    col.rgb = col.rgb / num;
    col.a = 1.0f;
    
    return col;
}
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor0 = tex0.Sample(smp, input.uv);
    texcolor0 = float4(float3(1.0f, 1.0f, 1.0f) - texcolor0.rgb, 1.0f);
    float4 texcolor1 = tex1.Sample(smp, input.uv);
    texcolor1 = Blur(tex1, input);
    
    float4 color = texcolor0;
    
    if(fmod(input.uv.y,0.1f)<0.05f)
    {
        color = texcolor1;
    }
    
    return float4(color.rgb, 1.0f);
}

/*

明度変更
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
     return float4(texcolor.rgb * 2.0f, 1.0f);

}
緑だけ変更
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    return float4(texcolor.r, texcolor.g * 2.0f, texcolor.b, 1.0f);

}

*/