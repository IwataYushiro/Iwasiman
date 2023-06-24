//マテリアル
cbuffer ConstBufferDataMaterial : register(b0)
{
};
//3D変換行列
cbuffer ConstBufferDataTransform : register(b1)
{
};

//頂点シェーダの出力構造体
//(頂点シェーダーからピクセルシェーダーへのやり取りに使用)
struct VSOutput
{
	//システム用頂点座標
	float4 svpos	: SV_POSITION;
	//uv値
	float2 uv		: TEXCOORD;
};
