#include "Particle.hlsli"

//�l�p�`�̒��_��
static const uint vnum = 4;

//�Z���^�[����̃I�t�Z�b�g
static const float4 offset_array[vnum] = {
	float4(-0.5f,-0.5f,0.0f,0.0f),		//����
	float4(-0.5f,+0.5f,0.0f,0.0f),		//����
	float4(+0.5f,-0.5f,0.0f,0.0f),		//�E��
	float4(+0.5f,+0.5f,0.0f,0.0f)		//�E��
};

//���オ0.0f,0.0f �E����1.0f,1.0f
static const float2 uv_array[vnum] = {
	float2(0.0f, 1.0f),				//����
	float2(0.0f, 0.0f),				//����
	float2(1.0f, 1.0f),				//�E��
	float2(1.0f, 0.0f)				//�E��
};

//�_���l�p�`���o�͂���
[maxvertexcount(vnum)]//�ő�o�͒��_��
void main(
	point VSOutput input[1] : SV_POSITION, //���Ƃ��F�X����
	//�O�p�`�X�g���[��
	inout TriangleStream< GSOutput > output//�����ŏo��
)
{
	GSOutput element;					//�o�͗p���_�f�[�^
	//4�_���܂킷
	for (uint i = 0; i < vnum; i++) {
		//���S����̃I�t�Z�b�g���r���{�[�h��](���f�����W)
		float4 offset = offset_array[i] * input[0].scale;

		offset = mul(matBillboard, offset);
		//���[���h���W�̃I�t�Z�b�g���A���炷
		element.svpos = input[0].pos + offset;
		//�r���[�A�ˉe�ϊ�
		element.svpos = mul(mat, element.svpos);
		element.uv = uv_array[i];
		//���_��1�o�͂���(�o�̓��X�g�ɒǉ�)
		output.Append(element);
	}


}