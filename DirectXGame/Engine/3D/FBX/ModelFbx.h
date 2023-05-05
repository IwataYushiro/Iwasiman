#pragma once
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <string>
#include <vector>

struct Node
{
	//名前
	std::string name;
	//ローカルスケール
	DirectX::XMVECTOR scaling = { 1.0f,1.0f,1.0f,0.0f };
	//ローカル回転角
	DirectX::XMVECTOR rotation = { 0.0f,0.0f,0.0f,0.0f };
	//ローカル座標
	DirectX::XMVECTOR translation = { 0.0f,0.0f,0.0f,0.0f };
	//ローカル変形行列
	DirectX::XMMATRIX transform;
	//グローバル変形行列
	DirectX::XMMATRIX globalTransform;
	//親ノード
	Node* parent = nullptr;
};

class ModelFbx
{
private://エイリアス

public://フレンド、サブクラス
	//フレンドクラス
	friend class FbxLoader;
	//頂点データ構造体
	struct VertexPosNormalUv
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
	};


public://メンバ関数

private://メンバ変数
	//モデル名
	std::string name;
	//ノード配列
	std::vector<Node> nodes;
	//メッシュを持つノード
	Node* meshNode = nullptr;
	//頂点データ配列
	std::vector<VertexPosNormalUv> vertices;
	//頂点インデックス配列
	std::vector<unsigned short> indices;
	//アンビエント係数
	DirectX::XMFLOAT3 ambient = { 1.0f,1.0f,1.0f };
	//ディフューズ係数
	DirectX::XMFLOAT3 diffuse = { 1.0f,1.0f,1.0f };
	//テクスチャメタデータ
	DirectX::TexMetadata metadata = {};
	//スクラッチイメージ
	DirectX::ScratchImage scratchImg = {};

};
