#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>

class Model
{
private://エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
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
	//マテリアル
	struct Material
	{
		std::string name;				//マテリアル名
		XMFLOAT3 ambient;				//アンビエント影響度
		XMFLOAT3 diffuse;				//ディフューズ影響度
		XMFLOAT3 specular;				//スペキュラー影響度
		float alpha;					//アルファ値
		std::string textureFilename;	//テクスチャファイル名

		//コンストラクタ
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

public://静的メンバ関数
	//OBJファイルから3Dモデルを読み込む
	static Model* LoadFromOBJ();
	
public://メンバ関数
	// マテリアル読み込み
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	/// テクスチャ読み込み
	void LoadTexture(const std::string& directoryPath, const std::string& filename);

private://メンバ変数
	// デバイス
	static ID3D12Device* device;
	// デスクリプタサイズ
	UINT descriptorHandleIncrementSize;
	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices;
	// 頂点インデックス配列
	std::vector<unsigned short> indices;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	//マテリアル
	Material material;

public://アクセッサ置き場
	//デバイス
	static void SetDevice(ID3D12Device* device) { Model::device = device; }
private://メンバ関数(カプセル化)
	//OBJファイルから3Dモデルを読み込む(非公開)
	void LoadFromOBJInternal();
	
};

