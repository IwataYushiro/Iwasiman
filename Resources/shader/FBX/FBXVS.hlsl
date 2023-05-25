#include "FBX.hlsli"
//�X�L�j���O��̒��_�E�@��������
struct SkinOutPut
{
    float4 pos;
    float3 normal;
};
//�X�L�j���O�v�Z
SkinOutPut ComputeSkin(VSInput input)
{
    //�[���N���A
    SkinOutPut output = (SkinOutPut) 0;
    
    uint iBone;         //�v�Z����{�[���ԍ�
    float weight;       //�{�[���E�F�C�g
    matrix m;           //�X�L�j���O�s��
    //�{�[��0
    iBone = input.boneIndices.x;
    weight = input.boneWeights.x;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);
    //�{�[��1
    iBone = input.boneIndices.y;
    weight = input.boneWeights.y;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);
    //�{�[��2
    iBone = input.boneIndices.z;
    weight = input.boneWeights.z;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);
    //�{�[��3
    iBone = input.boneIndices.w;
    weight = input.boneWeights.w;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);
    
    return output;
}
VSOutput main( VSInput input )
{
    //�X�L�j���O�v�Z
    SkinOutPut skinned = ComputeSkin(input); 
	//�@���Ƀ��[���h�s��ɂ��X�P�[�����O�A��]��K�p
    float4 wnormal = normalize(mul(world, float4(input.normal, 0.0f)));
	//�s�N�Z���V�F�[�_�ɓn���l
    VSOutput output;
	//�s��ɂ����W�ϊ�
    output.svpos = mul(mul(viewproj, world), input.pos);
	//���[���h�s������̃X�e�[�W�ɓn��
    output.normal = wnormal.xyz;
	//���͒l�����̂܂܎��̃X�e�[�W�ɓn��
    output.uv = input.uv;
	
    return output;
}