#include "Basic.hlsli"

VSOutput main(float4 pos : POSITION,float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	VSOutput output;//ピクセルシェーダーに渡す値
	output.svpos	= mul(mat,pos);
	output.normal = normal;
	output.uv		= uv;
	return output;
}
//　return pos + float4(<-Xオフセット+>,Yオフセット,0,0) 指定しただけずれる
//　return pos * float4(X拡長, Y拡張, Y縮小, Y縮小) 指定しただけ拡縮される