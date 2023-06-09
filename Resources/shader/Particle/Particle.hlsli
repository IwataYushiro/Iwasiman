cbuffer cbuff0 : register(b0)
{
	matrix mat; // ３Ｄ変換行列
	matrix matBillboard;	//ビルボード行列
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 pos : SV_POSITION; // システム用頂点座標
	float scale : TEXCOORD;	  // スケール
};

// ジオメトリシェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct GSOutput
{
	float4 svpos : SV_POSITION; // システム用頂点座標
	float2 uv  :TEXCOORD; // uv値
};
