#include "Mosaic.hlsli"

Texture2D<float4> tex : register(t0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp      : register(s0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
    //�����`���U�C�N
    float2 windowsSize = { 1280.0f, 720.0f };
    
    float density = 20.0f;
    float4 col = tex.Sample(smp, floor(input.uv * windowsSize / density) * density / windowsSize);
    return col;
}

/*
�����`���U�C�N
float4 main(VSOutput input) : SV_TARGET
{
    float density = 100;
    float4 col = tex.Sample(smp, floor(input.uv * density) / density);
}
*/