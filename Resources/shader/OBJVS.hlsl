#include "OBJ.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	//法線にワールド行列によるスケーリング、回転を適用
    float4 wnormal = normalize(mul(world, float4(normal, 0.0f)));
    float4 wpos = mul(world, pos);
    
    VSOutput output; // ピクセルシェーダーに渡す値
    output.svpos = mul(mul(viewproj, world), pos);
    output.worldPos = wpos;
    output.normal = wnormal;
    output.uv = uv;
    return output;
}