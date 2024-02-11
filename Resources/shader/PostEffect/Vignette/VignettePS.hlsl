#include "Vignette.hlsli"

Texture2D<float4> tex : register(t0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��

SamplerState smp      : register(s0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

//�r�l�b�g
float4 main(VSOutput input) : SV_TARGET
{
    float2 samplePoint = input.uv;
    float4 texColor = tex.Sample(smp, samplePoint);
    float vignette = length(float2(0.5, 0.5) - input.uv);
    float power = 0.3f; //�������قǋ���
    vignette = clamp(vignette - power, 0, 1);
    texColor.rgb -= vignette;
    return texColor;
}