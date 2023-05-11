#include "OBJ.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	//�@���Ƀ��[���h�s��ɂ��X�P�[�����O�A��]��K�p
    float4 wnormal = normalize(mul(world, float4(normal, 0.0f)));
    float4 wpos = mul(world, pos);
    
    VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
    output.svpos = mul(mul(viewproj, world), pos);
    output.worldPos = wpos;
    output.normal = wnormal;
    output.uv = uv;
    return output;
}