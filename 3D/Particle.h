#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>

class Particle
{
private://エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//OBJファイルからパーティクルモデルを読み込む
	static Particle* LoadFromParticleObj(const std::string& modelName);
	// デスクリプタヒープの初期化
	void InitializeDescriptorHeap();
	// テクスチャ読み込み
	void LoadTexture(const std::string& directoryPath, const std::string& filename);

	//各種バッファ生成
	void CreateBuffers();

	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndexMaterial);


private:

};

Particle::Particle()
{
}

Particle::~Particle()
{
}