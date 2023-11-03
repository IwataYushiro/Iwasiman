#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <fbxsdk.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <wrl.h>


/*

*	ModelFbx.h

*	ノード構造体

*/
struct Node
{
	
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
	
	//名前
	std::string name;
	//親ノード
	Node* parent = nullptr;
};

/*

*	ModelFbx.h

*	FBXモデル

*/
class ModelFbx
{
private://エイリアス
	//Microsoft::WRL::を省略
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;
	//std::を省略
	using string = std::string;
	template <class T>using vector = std::vector<T>;

public://定数
	//ボーンインデックスの最大数
	static const int32_t MAX_BONE_INDICES = 4;

public://フレンド、サブクラス
	//フレンドクラス
	friend class FbxLoader;
	//頂点データ構造体
	struct VertexPosNormalUvSkin
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
		UINT boneIndex[MAX_BONE_INDICES];	//ボーン番号
		float boneWeight[MAX_BONE_INDICES];	//スキンウェイト
	};
	//ボーン構造体
	struct Bone
	{
		//名前
		std::string name;
		//初期姿勢の逆行列
		DirectX::XMMATRIX invInitialPose;
		//クラスター
		FbxCluster* fbxCluster;
		//コンストラクタ(名前)
		Bone(const std::string& name) {
			this->name = name;
		}
	};

public://メンバ関数
	//デストラクタ
	~ModelFbx();
	//描画(コマンドリスト)
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//バッファ生成(デバイス)
	void CreateBuffers(ID3D12Device* device);
	//デバイス
	static void SetDevice(ID3D12Device* device) { ModelFbx::device_ = device; }

private://メンバ変数
	// デバイス
	static ID3D12Device* device_;
	//モデル名
	string name_;
	//ノード配列
	vector<Node> nodes_;
	//メッシュを持つノード
	Node* meshNode_ = nullptr;
	//ボーン配列
	std::vector<Bone> bones_;
	 //FBXシーン
	FbxScene* fbxScene_ = nullptr;
	//頂点データ配列
	vector<VertexPosNormalUvSkin> vertices_;
	//頂点インデックス配列
	vector<unsigned short> indices_;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff_;
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_;
	// SRV用のデスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV_;
	
	//アンビエント係数
	const XMFLOAT3 presetAmbient_ = { 1.0f,1.0f,1.0f };//プリセット
	XMFLOAT3 ambient_ = presetAmbient_;
	//ディフューズ係数
	const XMFLOAT3 presetDiffuse_ = { 1.0f,1.0f,1.0f };//プリセット
	XMFLOAT3 diffuse_ = presetDiffuse_;
	//テクスチャメタデータ
	TexMetadata metadata_ = {};
	//スクラッチイメージ
	ScratchImage scratchImg_ = {};

public://アクセッサ置き場
	//モデル変形行列
	const XMMATRIX& GetModelTransform() { return meshNode_->globalTransform; }
	//全ボーン
	std::vector<Bone>& GetBones() { return bones_; }
	//FBXシーン
	FbxScene* GetFbxScene() { return fbxScene_; }
};
