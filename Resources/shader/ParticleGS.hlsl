#include "Particle.hlsli"

//四角形の頂点数
static const uint vnum = 4;

//センターからのオフセット
static const float4 offset_array[vnum] = {
	float4(-0.5f,-0.5f,0.0f,0.0f),		//左下
	float4(-0.5f,+0.5f,0.0f,0.0f),		//左上
	float4(+0.5f,-0.5f,0.0f,0.0f),		//右下
	float4(+0.5f,+0.5f,0.0f,0.0f)		//右上
};

//左上が0.0f,0.0f 右下が1.0f,1.0f
static const float2 uv_array[vnum] = {
	float2(0.0f, 1.0f),				//左下
	float2(0.0f, 0.0f),				//左上
	float2(1.0f, 1.0f),				//右下
	float2(1.0f, 0.0f)				//右上
};

//点→四角形を出力する
[maxvertexcount(vnum)]//最大出力頂点数
void main(
	point VSOutput input[1] : SV_POSITION, //線とか色々ある
	//三角形ストリーム
	inout TriangleStream< GSOutput > output//ここで出力
)
{
	GSOutput element;					//出力用頂点データ
	//4点分まわす
	for (uint i = 0; i < vnum; i++) {
		//中心からのオフセットをビルボード回転(モデル座標)
		float4 offset = offset_array[i] * input[0].scale;

		offset = mul(matBillboard, offset);
		//ワールド座標のオフセット分、ずらす
		element.svpos = input[0].pos + offset;
		//ビュー、射影変換
		element.svpos = mul(mat, element.svpos);
		element.uv = uv_array[i];
		//頂点を1個出力する(出力リストに追加)
		output.Append(element);
	}


}