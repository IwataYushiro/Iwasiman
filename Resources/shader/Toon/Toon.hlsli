cbuffer cbuff0 : register(b0)
{
    matrix viewproj;
    matrix world; // ３Ｄ変換行列
    float3 cameraPos;
};

cbuffer cbuff1 : register(b1)
{
    float3 m_ambient : packoffset(c0); //アンビエント係数
    float3 m_diffuse : packoffset(c1); //ディフューズ係数
    float3 m_specular : packoffset(c2); //スペキュラー係数
    float m_alpha : packoffset(c2.w); //アルファ値
};
static const uint DIR_LIGHT_NUM = 3;

struct DirLight
{
    float3 lightv; //ライトへの方向の単位ベクトル
    float3 lightcolor; //ライトの色(RGB)
    uint active;
};

cbuffer cbuff2 : register(b2)
{
    float3 ambientColor;
    DirLight dirLights[DIR_LIGHT_NUM];
};
// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float4 worldPos : POSITION;
    float3 normal : NORMAL; // 法線ベクトル
    float2 uv : TEXCOORD; // uv値
};
