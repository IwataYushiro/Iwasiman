#include "Vignette.hlsli"

Texture2D<float4> tex : register(t0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��

SamplerState smp      : register(s0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

//�r�l�b�g�ʏ�
float4 main(VSOutput input) : SV_TARGET
{
    float2 samplePoint = input.uv;
    float4 texColor = tex.Sample(smp, samplePoint) * texturecolor;
    //�^�񒆂Ƀr�l�b�g
    float vignette = length(float2(0.5f, 0.5f) - input.uv);
    float vignettepower = 1.01f-power; //�������قǋ���
    vignette = clamp(vignette - vignettepower, 0.0f, 1.0f);
    texColor.rgb -= vignette * (1.0f - color.rgb);
    return texColor;
}