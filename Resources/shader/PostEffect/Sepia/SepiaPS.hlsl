#include "Sepia.hlsli"

Texture2D<float4> tex : register(t0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp      : register(s0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    float sepia = 0.1f;
    float grayScale = texcolor.r * 0.299f + texcolor.g * 0.587f + texcolor.b * 0.114f;
    float4 gray = float4(grayScale + sepia, grayScale, grayScale - sepia, 1.0f);
    return gray;
}
