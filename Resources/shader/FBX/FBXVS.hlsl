#include "FBX.hlsli"

VSOutput main( VSInput input )
{
	//法線にワールド行列によるスケーリング、回転を適用
    float4 wnormal = normalize(mul(world, float4(input.normal, 0.0f)));
	//ピクセルシェーダに渡す値
    VSOutput output;
	//行列による座標変換
    output.svpos = mul(mul(viewproj, world), input.pos);
	//ワールド行列を次のステージに渡す
    output.normal = wnormal.xyz;
	//入力値をそのまま次のステージに渡す
    output.uv = input.uv;
	
    return output;
}