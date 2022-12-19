#pragma once
#include "SpriteCommon.h"
#include "Input.h"
#include <DirectXMath.h>

//1枚1枚のスプライトクラス

class Sprite
{
public: // メンバ関数
	//エイリアステンプレートとかで色々省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private://構造体類
	//マテリアル
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;
	};

	//定数バッファのGPUリソースのポインタ
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;
	//マッピング用のポインタ
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	//マテリアル
	struct ConstBufferDataTransform
	{
		XMMATRIX mat;	//3D変換行列
		
	};
//アフィン変換情報
		XMFLOAT2 scale = { 1.0f,1.0f};
		float rotationZ = 0.0f;
		XMFLOAT2 position = { 0.0f,0.0f};
		//ワールド変換行列
		XMMATRIX matWorld=DirectX::XMMatrixIdentity();
		

	//定数バッファのGPUリソースのポインタ
	ComPtr<ID3D12Resource> constBuffTransform = nullptr;
	//マッピング用のポインタ
	ConstBufferDataTransform* constMapTransform = nullptr;

	//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;		//xyz座標
		XMFLOAT2 uv;		//uv座標
	};


public://メンバ関数

	//初期化
	void Initialize(SpriteCommon* spCommon, Input* input);
	//更新
	void Update();
	//描画
	void Draw();

	//定数バッファ
	//マテリアル
	void CreateConstBufferMaterial();
	//3D座標
	void CreateConstBufferTransform();
private://メンバ変数
	//頂点数
	static const size_t verticesCount = 4;

	//頂点データ
	Vertex vertices[verticesCount] = {
		{{  0.0f,100.0f,0.0f},{0.0f,1.0f}},	//左下
		{{  0.0f,  0.0f,0.0f},{0.0f,0.0f}},		//左上
		{{100.0f,100.0f,0.0f},{1.0f,1.0f}},	//右下
		{{100.0f,  0.0f,0.0f},{1.0f,0.0f}},	//右上
	};

	//インプット
	Input* input_ = nullptr;
	SpriteCommon* spCommon_ = nullptr;
	
	//頂点バッファのリソース設定
	D3D12_RESOURCE_DESC resDesc{};

	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
};
