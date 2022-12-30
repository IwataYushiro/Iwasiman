#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>

class Model
{
private://エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private://サブクラス
	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos; // xyz座標
		XMFLOAT3 normal; // 法線ベクトル
		XMFLOAT2 uv;  // uv座標
	};

public://静的メンバ関数
	//OBJファイルから3Dモデルを読み込む
	static Model* LoadFromOBJ();

private://メンバ変数
	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices;
	// 頂点インデックス配列
	std::vector<unsigned short> indices;

public://アクセッサ置き場

private://メンバ関数
	//OBJファイルから3Dモデルを読み込む(非公開)
	Model* LoadFromOBJInternal();
};

