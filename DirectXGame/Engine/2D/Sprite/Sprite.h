#pragma once
#include "SpriteCommon.h"
#include "Input.h"
#include <DirectXMath.h>

/*

*	Sprite.h

*	1枚のスプライト

*/
class Sprite
{
public: // メンバ関数
	//エイリアステンプレートとかで色々省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://構造体類
	//マテリアル
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;
	};

	//座標
	struct ConstBufferDataTransform
	{
		XMMATRIX mat;	//3D変換行列
	};

//頂点番号
	enum VertexNumber
	{
		LB,		//左下
		LT,		//左上
		RB,		//右下
		RT,		//右上
		verticesCount,//要素数
	};

	//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;		//xyz座標
		XMFLOAT2 uv;		//uv座標
	};
protected://構造体類に関係あるメンバ変数
	//定数バッファのGPUリソースのポインタ
	ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;
	//マッピング用のポインタ
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;

	//アンカーポイント
	XMFLOAT2 anchorPoint_ = { 0.0f,0.0f };
	//表示サイズ
	XMFLOAT2 size_ = { 100.0f,100.0f };
	
	

	//回転
	float rotationZ_ = 0.0f;
	//座標
	XMFLOAT2 position_ = { 0.0f,0.0f };
	//色(RGBA)
	XMFLOAT4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	//左右フリップ
	bool isFlipX_ = false;
	//上下フリップ
	bool isFlipY_ = false;

	//非表示フラグ
	bool isInvisible_ = false;

	//テクスチャ左上座標
	XMFLOAT2 textureLeftTop_ = { 0.0f,0.0f };
	//テクスチャ切り出しサイズ
	XMFLOAT2 textureSize_ = { 100.0f,100.0f };

	//射影行列
	XMMATRIX matProjection_;

	//ワールド変換行列
	XMMATRIX matRot_;
	XMMATRIX matTrans_;
	XMMATRIX matWorld_ = DirectX::XMMatrixIdentity();


	//定数バッファのGPUリソースのポインタ
	ComPtr<ID3D12Resource> constBuffTransform_ = nullptr;
	//マッピング用のポインタ
	ConstBufferDataTransform* constMapTransform_ = nullptr;

public://メンバ関数

	//初期化(スプライト基盤、テクスチャインデックス)
	void Initialize(SpriteCommon* spCommon, uint32_t textureIndex = UINT32_MAX);
	//更新
	void Update();
	//描画
	void Draw();

	//定数バッファ
	//マテリアル
	void CreateConstBufferMaterial();
	//3D座標
	void CreateConstBufferTransform();

//private://メンバ変数
protected:
	//頂点データ
	Vertex vertices_[verticesCount] = {
		{{  0.0f,size_.y,0.0f},{0.0f,1.0f}},	//左下
		{{  0.0f,  0.0f,0.0f},{0.0f,0.0f}},		//左上
		{{size_.x,size_.y,0.0f},{1.0f,1.0f}},	//右下
		{{size_.x,  0.0f,0.0f},{1.0f,0.0f}},	//右上
	};

	//GPU上のバッファに対応した仮想メモリを取得
	Vertex* vertMap_ = nullptr;
	//インプット
	Input* input_ = nullptr;
	//スプライト基盤
	SpriteCommon* spCommon_ = nullptr;

	//頂点バッファのリソース設定
	D3D12_RESOURCE_DESC resDesc_{};

	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	//テクスチャ番号
	uint32_t textureIndex_ = 0;

public://アクセッサ置き場
	//アンカーポイントセット
	void SetAnchorPoint(const XMFLOAT2& anchorPoint) { anchorPoint_ = anchorPoint; }
	//アンカーポイントゲット
	const XMFLOAT2& GetAnchorPoint()const { return anchorPoint_; }

	//スケーリングセット
	void SetSize(const XMFLOAT2& size) { size_ = size; }
	//スケーリングゲット
	const XMFLOAT2& GetSize()const { return size_; }

	//ポジションセット
	void SetPosition(const XMFLOAT2& position) { position_ = position; }
	//ポジションゲット
	const XMFLOAT2& GetPosition()const { return position_; }

	//ローテーションセット(Z軸)
	void SetRotationZ(const float& rotationZ) { rotationZ_ = rotationZ; }
	//ローテーションゲット(Z軸)
	const float& GetRotationZ()const { return rotationZ_; }

	//カラーセット
	void SetColor(const XMFLOAT4& color) { color_ = color; }
	//カラーゲット
	const XMFLOAT4& GetColor()const { return color_; }

	//左右フリップセット
	void SetFlipX(const bool& isFlipX) { isFlipX_ = isFlipX; }
	//左右フリップゲット
	const bool& GetFlipX()const { return isFlipX_; }
	
	//上下フリップセット
	void SetFlipY(const bool& isFlipY) { isFlipY_ = isFlipY; }
	//上下フリップゲット
	const bool& GetFlipY()const { return isFlipY_; }

	//視覚化セット
	void SetInvisible(const bool& isInvisible) { isInvisible_ = isInvisible; }
	//視覚化ゲット
	const bool& GetInvisible()const { return isInvisible_; }

	//テクスチャ番号セット
	void SetTextureIndex(const uint32_t& textureIndex) { textureIndex_ = textureIndex; }
	//テクスチャ番号ゲット
	const uint32_t& GetTextureIndex()const { return textureIndex_; }

	//テクスチャ左上座標セット
	void SetTextureLeftTop(const XMFLOAT2& textureLeftTop) { textureLeftTop_ = textureLeftTop; }
	//テクスチャ左上座標ゲット
	const XMFLOAT2& GetTextureLeftTop()const { return textureLeftTop_; }

	//テクスチャ切り出しサイズセット
	void SetTextureSize(const XMFLOAT2& textureSize) { textureSize_ = textureSize; }
	//テクスチャ切り出しサイズゲット
	const XMFLOAT2& GetTextureSize()const { return textureSize_; }

protected:
	//テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();

};
