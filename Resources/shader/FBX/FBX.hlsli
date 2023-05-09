cbuffer cbuff0 : register(b0)
{
    matrix viewproj;        //�r���v���s��
    matrix world;           //���[���h�s��
    float3 camerapos;       //�J�������W
}

//���_�o�b�t�@�̓���
struct VSInput
{
    float4 pos : POSITION;      //�ʒu
    float3 normal : NORMAL;     //���_�@��
    float2 uv : TEXCOORD;       //�e�N�X�`�����W
};

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
    float3 normal : NORMAL;     // �@���x�N�g��
    float2 uv : TEXCOORD;       // uv�l
};