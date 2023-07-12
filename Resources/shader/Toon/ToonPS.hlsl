#include "Toon.hlsli"

Texture2D<float4> tex : register(t0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	//�����o�[�g����
	//float3 light = normalize(float3(1,-1,1)); // �E�����@�����̃��C�g
	//float light_diffuse = saturate(dot(-light, input.normal));
	//float shade_color;
	//shade_color = m_ambient;					//�A���r�G���g�l
	//shade_color += m_diffuse * light_diffuse;	//�f�B�t���[�Y�l
	//float4 texcolor = tex.Sample(smp, input.uv);
	//return float4(texcolor.rgb * shade_color, texcolor.a * m_alpha);
	
	//�t�H���V�F�[�f�B���O
    float4 texcolor = tex.Sample(smp, input.uv);
	//����x
    const float shininess = 4.0f;
	//���_���王�_�ւ̕����x�N�g��
    float3 eyeDir = normalize(cameraPos - input.worldPos.xyz);
	//�����ˌ�
    float3 ambient = m_ambient;
    //�V�F�[�f�B���O�J���[
    float4 shade_color = float4(ambientColor * ambient, m_alpha);
    
    for (uint i = 0; i < DIR_LIGHT_NUM; i++)
    {
        if (dirLights[i].active)
        {
	        //���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = smoothstep(0.3f, 0.35f, saturate(dot(dirLights[i].lightv, input.normal)));
	        //���ˌ��x�N�g��
            float3 reflect = normalize(dirLights[i].lightv + 2.0f * dotlightnormal * input.normal);
   
	        //�g�U���ˌ�
            float3 diffuse = dotlightnormal * m_diffuse;
	        //���ʔ��ˌ�
            float3 specular = smoothstep(0.5f,0.55f,pow(saturate(dot(reflect, eyeDir)), shininess)) * m_specular;
            //�S���Z
            shade_color.rgb += (1.0f - dotlightnormal) * dirLights[i].lightcolor+(diffuse + specular)  ;
            shade_color.a = m_alpha;
        }
    }
	//�V�F�[�f�B���O�J���[�ŕ`��
    return shade_color * texcolor;
}