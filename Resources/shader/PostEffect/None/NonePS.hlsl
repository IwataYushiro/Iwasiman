#include "None.hlsli"

Texture2D<float4> tex : register(t0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��

SamplerState smp      : register(s0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

//�f�t�H���g
float4 main(VSOutput input) : SV_TARGET
{
    float2 samplePoint = input.uv;
    float4 texcolor = tex.Sample(smp, samplePoint);
    samplePoint.x += 0.01;

    texcolor.r = tex.Sample(smp, samplePoint).r;
    texcolor.g = tex.Sample(smp, -samplePoint).g;
    texcolor.b = tex.Sample(smp, float2(0.0f,-samplePoint.x)).b;
    return texcolor;
    //float4 texcolor = tex.Sample(smp, input.uv);
    //return float4(texcolor.rgb, 1.0f);
}

//���������ۂ���
//float4 main(VSOutput input) : SV_TARGET
//{
//
//    float2 samplePoint = input.uv;
//    float4 texColor = tex.Sample(smp, samplePoint);
//    float sinv = sin(input.uv.y * 2 * -0.1);
//    float steped = step(1, sinv * sinv);
//    texColor.rgb -= (1 - steped) * abs(sin(input.uv.y * 50.0 * 1.0)) * 0.02;//�f�J���قǋ���
//    texColor.rgb -= (1 - steped) * abs(sin(input.uv.y * 100.0 * 2.0)) * 0.03;
//    texColor.rgb += steped * 0.04;
//    return texColor;
//}

//�r�l�b�g
//float4 main(VSOutput input) : SV_TARGET
//{
//    float2 samplePoint = input.uv;
//    float4 texColor = tex.Sample(smp, samplePoint);
//    float vignette = length(float2(0.5, 0.5) - input.uv);
//    float power = 0.3f;//�������قǋ���
//    vignette = clamp(vignette - power, 0, 1);
//    texColor.rgb -= vignette;
//    return texColor;
//}

//�M��p��
//float4 main(VSOutput input) : SV_TARGET
//{
//    float2 samplePoint = input.uv;
//    samplePoint -= float2(0.5, 0.5);
//    const float distort = 0.3f; //�c�ݗ�
//    float distPower = pow(length(samplePoint), 0.05f);
//    samplePoint *= float2(distPower, distPower);
//    samplePoint += float2(0.5, 0.5);
//    float4 texColor = tex.Sample(smp, samplePoint);
//    return texColor;
//}

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