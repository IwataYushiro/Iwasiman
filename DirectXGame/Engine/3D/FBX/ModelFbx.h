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

//ノード構造体
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
	//ボーン構造体
	struct Bone
	{
		//名前
		std::string name;
		//初期姿勢の逆行列
		DirectX::XMMATRIX invInitialPose;
		//クラスター
		FbxCluster* fbxCluster;
		//コンストラクタ
		Bone(const std::string& name) {
			this->name = name;
		}
	};
public://定数
	//ボーンインデックスの最大数
	static const int MAX_BONE_INDICES = 4;
public://メンバ関数
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//バッファ生成
	void CreateBuffers(ID3D12Device* device);
	//デバイス
	static void SetDevice(ID3D12Device* device) { ModelFbx::device_ = device; }

private://メンバ変数
	// デバイス
	static ID3D12Device* device_;
	//モデル名
	string name;
	//ノード配列
	vector<Node> nodes;
	//メッシュを持つノード
	Node* meshNode = nullptr;
	//ボーン配列
	std::vector<Bone> bones;
	 
	//頂点データ配列
	vector<VertexPosNormalUv> vertices;
	//頂点インデックス配列
	vector<unsigned short> indices;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView;
	// SRV用のデスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	
	//アンビエント係数
	XMFLOAT3 ambient = { 1.0f,1.0f,1.0f };
	//ディフューズ係数
	XMFLOAT3 diffuse = { 1.0f,1.0f,1.0f };
	//テクスチャメタデータ
	TexMetadata metadata = {};
	//スクラッチイメージ
	ScratchImage scratchImg = {};

public://アクセッサ置き場
	//モデル変形行列
	const XMMATRIX& GetModelTransform() { return meshNode->globalTransform; }
	//全ボーン
	std::vector<Bone>& GetBones() { return bones; }
};
