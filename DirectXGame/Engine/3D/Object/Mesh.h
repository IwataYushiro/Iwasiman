#pragma once
#include "Material.h"
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <unordered_map>
#include <vector>
#include <wrl.h>

/*

*	Mesh.h

*	形状(メッシュ)

*/
class Mesh
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public://サブクラス
	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos; // xyz座標
		XMFLOAT3 normal; // 法線ベクトル
		XMFLOAT2 uv;  // uv座標
	};
public:
	//静的初期化
	static void StaticInitialize(ID3D12Device* device);

private://静的メンバ変数
	static ID3D12Device* device_;

public://メンバ関数
	//頂点データ追加
	void AddVertex(const VertexPosNormalUv& vertex) { vertices_.emplace_back(vertex); }
	//頂点インデックス追加
	void AddIndex(unsigned short index) { indices_.emplace_back(index); }
	//エッジ平滑化データ追加
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);
	//平滑化された頂点法線の追加
	void CalculateSmoothedVertexNormals();
	//バッファ生成
	void CreateBuffers();
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList);

public://アクセッサ置き場
	//名前ゲット
	const std::string& GetName() { return name_; }
	//名前セット
	void SetName(const std::string& name) { this->name_ = name; }
	//マテリアルゲット
	Material* GetMaterial() { return material_; }
	//マテリアルセット
	void SetMaterial(Material* material) { this->material_ = material; }
	//頂点データ数ゲット
	inline size_t GetVertexCount() { return vertices_.size(); }
	//頂点バッファゲット
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView_; }
	//インデックスバッファゲット
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView_; }
	//頂点配列ゲット
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices_; }
	//インデックス配列ゲット
	inline const std::vector<unsigned short>& GetIndices() { return indices_; }

private://メンバ変数
	//名前
	std::string name_;
	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices_;
	// 頂点インデックス配列
	std::vector<unsigned short> indices_;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_;
	//マテリアル
	Material* material_ = nullptr;
	//頂点、インデックスバッファのマッピング
	VertexPosNormalUv* vertMap_ = nullptr;
	unsigned short* indexmap_ = nullptr;
	//頂点スムージングデータ
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData_;

};
