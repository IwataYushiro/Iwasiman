#include "OBJ.hlsli"

Texture2D<float4> tex : register(t0);  // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	//float3 light = normalize(float3(1,-1,1)); // �E�����@�����̃��C�g
	//float light_diffuse = saturate(dot(-light, input.normal));
	//float shade_color;
	//shade_color = m_ambient;					//�A���r�G���g�l
	//shade_color += m_diffuse * light_diffuse;	//�f�B�t���[�Y�l
	//float4 texcolor = tex.Sample(smp, input.uv);
	//return float4(texcolor.rgb * shade_color, texcolor.a * m_alpha);
	 
    float4 texcolor = tex.Sample(smp, input.uv);
    //�V�F�[�f�B���O�J���[
	float4 shade_color;
	//����x
    const float shininess = 4.0f;
	//���_���王�_�ւ̕����x�N�g��
    float3 eyeDir = normalize(cameraPos - input.worldPos.xyz);
    //���C�g�Ɍ������x�N�g���Ɩ@���̓���
    float3 dotlightnormal = dot(lightv, input.normal);
	//���ˌ��x�N�g��
    float3 reflect = normalize(-lightv + 2.0f * dotlightnormal * input.normal);
    //�����ˌ�
    float3 ambient = m_ambient;
	//�g�U���ˌ�
    float3 diffuse = dotlightnormal * m_diffuse;
	//���ʔ��ˌ�
    float3 specular = pow(saturate(dot(reflect, eyeDir)), shininess) * m_specular;
    //�S���Z
    shade_color.rgb = (ambient + diffuse + specular) * lightcolor;
    shade_color.a = m_alpha;
	
	//�V�F�[�f�B���O�J���[�ŕ`��
    return shade_color *texcolor;
	
}