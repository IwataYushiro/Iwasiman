#include "Test.hlsli"

Texture2D<float4> tex : register(t0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp      : register(s0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    return float4(texcolor.rgb, 1.0f);
}

/*

���x�ύX
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
     return float4(texcolor.rgb * 2.0f, 1.0f);

}
�΂����ύX
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    return float4(texcolor.r, texcolor.g * 2.0f, texcolor.b, 1.0f);

}

*/