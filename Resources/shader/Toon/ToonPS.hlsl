#include "Toon.hlsli"

Texture2D<float4> tex : register(t0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{ 
    ////�e�N�X�`��
    //float4 texcolor = tex.Sample(smp, input.uv);
    ////�V�F�[�f�B���O�J���[ 
    //float shade_color;
    ////����x
    //const float shininess = 4.0f;
    ////���_���王�_�ւ̕����x�N�g��
    //float3 eyeDir = normalize(cameraPos - input.worldPos.xyz);
    ////���C�g�Ɍ������x�N�g���Ɩ@���̓���
    ////float3 dotlightnormal=dot(lightv,input.normal)
    ////���ˌ��x�N�g��
    
    
    //float3 light = normalize(float3(-1.0f, -1.0f, 1.0f)); // �E�����@�����̃��C�g
    //float light_diffuse = saturate(dot(-light, input.normal));
   
    //float intensity = smoothstep(0.3f, 0.35f, saturate(dot(normalize(input.normal), light_diffuse)));
    
    //shade_color = (1.0f - intensity) * m_ambient; //�A���r�G���g�l
    //shade_color += m_diffuse * light_diffuse; //�f�B�t���[�Y�l
   
    //return float4(texcolor.rgb * shade_color, texcolor.a * m_alpha);
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}