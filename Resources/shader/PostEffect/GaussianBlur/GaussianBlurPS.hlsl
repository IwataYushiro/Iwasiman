#include "GaussianBlur.hlsli"

Texture2D<float4> tex : register(t0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp      : register(s0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 main(VSOutput input) : SV_TARGET
{
    float totalWeight = 0.0f, _Sigma = 0.005f, _StepWidth = 0.001f;
    float4 col = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (float py = -_Sigma * 2.0f; py <= _Sigma * 2.0f; py += _StepWidth)
    {
				//[loop]�A[unroll]�͏����Ȃ��Ă���
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

