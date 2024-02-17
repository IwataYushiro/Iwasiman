#pragma once
#include "Sprite.h"

namespace IwasiEngine//IwasiEngineのネームスペース
{
	/*

	*	PostEffect.h

	*	ポストエフェクト

	*/
	class PostEffect
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
			XMFLOAT4 color; //色(RGBA)
			float power;    //ポストエフェクトの強さ
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
		//画面クリアカラー
		enum ClearColorRGBW
		{
			CCRGBW_R = 0,
			CCRGBW_G = 1,
			CCRGBW_B = 2,
			CCRGBW_W = 3,
			CCRGBW_Num = 4,
		};

		//ルートパラメータインデックス
		enum RootParameterIndex
		{
			RPI_ConstBuffMaterial = 0,
			RPI_TexBuff0 = 1,
			RPI_ConstBuffTransform = 2,
			RPI_TexBuff1 = 3,
			RPI_Num = 4,
		};
	private://静的メンバ変数
		//ベースディレクトリ
		static const std::string baseDirectory_;
		//頂点シェーダー
		static const std::string directoryVS_;
		//ピクセルシェーダー
		static const std::string directoryPS_;

		static const float clearColor_[CCRGBW_Num];

	public:
		//コンストラクタ
		PostEffect();
		//初期化(使用ポストエフェクトパス)
		void Initialize(const std::string& fileName = "None");
		//更新
		void Update();

		//描画(コマンドリスト)
		void Draw();
		//描画前処理(コマンドリスト)
		void PreDraw(ID3D12GraphicsCommandList* cmdList);

		//描画後処理(コマンドリスト)
		void PostDraw(ID3D12GraphicsCommandList* cmdList);

	private:
		//頂点データのプリセット
		const Vertex presetVerticesPost_[verticesCount] = {
			{{-1.0f,-1.0f,0.0f},{0.0f,1.0f}},	//左下
			{{-1.0f,+1.0f,0.0f},{0.0f,0.0f}},	//左上
			{{+1.0f,-1.0f,0.0f},{1.0f,1.0f}},	//右下
			{{+1.0f,+1.0f,0.0f},{1.0f,0.0f}},	//右上
		};
		//頂点データ
		Vertex verticesPost_[verticesCount] = {
			presetVerticesPost_[LB],	//左下
			presetVerticesPost_[LT],	//左上
			presetVerticesPost_[RB],	//右下
			presetVerticesPost_[RT],	//右上
		};

		//定数バッファのGPUリソースのポインタ
		ComPtr<ID3D12Resource> constBuffMaterialPost_ = nullptr;
		//マッピング用のポインタ
		ConstBufferDataMaterial* constMapMaterialPost_ = nullptr;
		//定数バッファのGPUリソースのポインタ
		ComPtr<ID3D12Resource> constBuffTransformPost_ = nullptr;
		//マッピング用のポインタ
		ConstBufferDataTransform* constMapTransformPost_ = nullptr;

		//スプライト基盤
		const SpriteCommon* spCommon_ = nullptr;
		//頂点バッファ
		ComPtr<ID3D12Resource> vertBuff_;
		//頂点バッファビュー
		D3D12_VERTEX_BUFFER_VIEW vbView_{};
		//リソースデスクの設定
		struct ResDescPreset
		{
			const UINT16 arraysize = 1;
			const UINT16 mipLevels = 0;
			const UINT sampleCount = 1;
			const UINT sampleQuality = 0;
		};
		ResDescPreset resDescPreset_;

		//テクスチャバッファ
		enum TexBuffCount
		{
			TBC_TexBuff0 = 0,
			TBC_TexBuff1 = 1,
			TBC_Num = 2,
		};
		ComPtr<ID3D12Resource> texBuff_[TBC_Num];
		//SRV用デスクリプタヒープ
		ComPtr<ID3D12DescriptorHeap> descHeapSRV_;
		//深度バッファ
		ComPtr<ID3D12Resource> depthBuff_;
		//RTV用デスクリプタヒープ
		ComPtr<ID3D12DescriptorHeap> descHeapRTV_;
		//DSV用デスクリプタヒープ
		ComPtr<ID3D12DescriptorHeap> descHeapDSV_;
		//グラフィックスパイプライン
		ComPtr<ID3D12PipelineState> pipelineState_;
		//ルートシグネチャ
		ComPtr<ID3D12RootSignature> rootSignature_;

		//色(RGBA)
		const XMFLOAT4 defaultColor_ = { 1.0f,1.0f,1.0f,1.0f };
		XMFLOAT4 color_ = defaultColor_;
		//ポストエフェクトの強さ(グレースケールの濃さとか　1.0fで最大)
		const float defaultPower_ = 1.0f;
		float power_ = defaultPower_;

	public://アクセッサ置き場
		//カラーセット
		void SetColor(const XMFLOAT4& color) { color_ = color; }
		//カラーゲット
		const XMFLOAT4& GetColor()const { return color_; }
		//パワーセット
		void SetPower(const float power) { power_ = power; }
		//パワーゲット
		const float GetPower()const { return power_; }

	private://カプセル化メンバ関数
		//頂点バッファ生成
		void CreateVertexBuffer();
		//定数バッファ生成
		void CreateConstBuffer();
		//マテリアル
		void CreateConstBufferMaterialPost();
		//座標
		void CreateConstBufferTransformPost();
		//テクスチャ生成
		void CreateTexture();
		// SRV生成
		void CreateSRV();
		// RTV生成
		void CreateRTV();
		// 深度バッファ生成
		void CreateDepthBuffer();
		// DSV生成
		void CreateDSV();
		//パイプライン生成(ポストエフェクトファイルパス)
		void CreateGraphicsPipelineState(const std::string& fileName);
	};
}