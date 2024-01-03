#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include "Model.h"
#include "Camera.h"
#include "LightGroup.h"
#include <unordered_map>
#include <memory>
#include "CollisionInfo.h"

namespace IwasiEngine//IwasiEngineのネームスペース
{
	class BaseCollider;

	/*

	*	Object3d.h

	*	3Dオブジェクト(.obj)

	*/
	class Object3d
	{
	private: // エイリアス
		// Microsoft::WRL::を省略
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		// DirectX::を省略
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMMATRIX = DirectX::XMMATRIX;

	public://コンストラクタ等
		//コンストラクタ
		Object3d() = default;
		//デストラクタ
		virtual ~Object3d();

	public: // サブクラス


		// 定数バッファ用データ構造体
		struct ConstBufferDataB0
		{
			XMMATRIX viewproj;		//ビュープロジェクション行列
			XMMATRIX world;			//ワールド行列
			XMFLOAT3 cameraPos;		//カメラ座標(ワールド座標)
		};

	private: // 定数
		static const int division = 50;									// 分割数
		static const float radius;										// 底面の半径
		static const float prizmHeight;									// 柱の高さ
		static const int planeCount = division * 2 + division * 2;		// 面の数
		static const int vertexCount = planeCount * 3;					// 頂点数

	public: // 静的メンバ関数

		// 静的初期化(デバイス)
		static void StaticInitialize(ID3D12Device* device);


		// 描画前処理(コマンドリスト)
		static void PreDraw(ID3D12GraphicsCommandList* cmdList);

		//描画後処理
		static void PostDraw();


		// 生成
		static std::unique_ptr<Object3d> Create();

	private: // 静的メンバ変数
		// デバイス
		static ID3D12Device* device_;

		// コマンドリスト
		static ID3D12GraphicsCommandList* cmdList_;
		// ルートシグネチャ
		static ComPtr<ID3D12RootSignature> rootsignature_;
		// パイプラインステートオブジェクト
		static ComPtr<ID3D12PipelineState> pipelinestate_;

		static ComPtr<ID3DBlob> rootSigBlob_;	//ルートシグネチャオブジェクト
		static ComPtr<ID3DBlob> vsBlob_;		// 頂点シェーダオブジェクト
		static ComPtr<ID3DBlob> psBlob_;		// ピクセルシェーダオブジェクト
		static ComPtr<ID3DBlob> errorBlob_;		// エラーオブジェクト
		//ライトグループ
		static LightGroup* lightGroup_;



	private:// 静的メンバ関数
		// グラフィックパイプライン生成
		static void InitializeGraphicsPipeline();

	public: // メンバ関数
		//初期化
		virtual bool Initialize();
		//更新
		virtual void Update();

		//行列更新
		void UpdateWorldMatrix();
		//描画
		virtual void Draw();

		//衝突時のコールバック(コリジョン情報、メイン属性、サブ属性)
		virtual void OnCollision([[maybe_unused]] const IwasiEngine::CollisionInfo& info,
			[[maybe_unused]] const unsigned short attribute, [[maybe_unused]] const unsigned short subAttribute) {}

	private://プリセット
		// 色
		const XMFLOAT4 presetColor_ = { 1.0f,1.0f,1.0f,1.0f };//プリセット
		// ローカルスケール
		const XMFLOAT3 presetScale_ = { 1.0f,1.0f,1.0f };//プリセット
		// X,Y,Z軸回りのローカル回転角
		const XMFLOAT3 presetRotation_ = { 0.0f,0.0f,0.0f };//プリセット
		// ローカル座標
		const XMFLOAT3 presetPosition_ = { 0.0f,0.0f,0.0f };//プリセット

		//ルートパラメータインデックス
		enum RootParameterIndex
		{
			RPI_ConstBuffTransform = 0,
			RPI_ConstBuffMaterial = 1,
			RPI_TexBuff = 2,
			RPI_ConstBuffLight = 3,
			RPI_Num = 4,
		};
	protected: // メンバ変数
		//モデル
		const Model* model_ = nullptr;
		//カメラ
		Camera* camera_;
		//ビルボード
		bool isBillboard_ = false;
		// 定数バッファ
		ComPtr<ID3D12Resource> constBuffB0_;
		//定数バッファのマッピング
		ConstBufferDataB0* constMap0_ = nullptr;
		// 色
		XMFLOAT4 color_ = presetColor_;
		// ローカルスケール
		XMFLOAT3 scale_ = presetScale_;
		// X,Y,Z軸回りのローカル回転角
		XMFLOAT3 rotation_ = presetRotation_;
		// ローカル座標
		XMFLOAT3 position_ = presetPosition_;
		// ローカルワールド変換行列
		XMMATRIX matWorld_;
		// 親オブジェクト
		Object3d* parent_ = nullptr;

		//クラス名
		const char* name_ = nullptr;
		//コライダー
		BaseCollider* collider_ = nullptr;

	public: //アクセッサ置き場
		//モデルセット
		void SetModel(const Model* model) { this->model_ = model; }
		//スケーリングゲット
		const XMFLOAT3& GetScale() const { return scale_; }
		//スケーリングセット
		void SetScale(const XMFLOAT3& scale) { this->scale_ = scale; }
		//ローテーションゲット
		const XMFLOAT3& GetRotation() const { return rotation_; }
		//ローテーションセット
		void SetRotation(const XMFLOAT3& rotation) { this->rotation_ = rotation; }

		//ワールド行列ゲット
		const XMMATRIX& GetWorld() const { return matWorld_; }
		//ワールド行列セット
		void SetWorld(const XMMATRIX& matWorld) { this->matWorld_ = matWorld; }

		// ポジションゲット
		const XMFLOAT3& GetPosition() const { return position_; }

		//ポジションセット
		void SetPosition(const XMFLOAT3& position) { this->position_ = position; }

		//カメラセット
		void SetCamera(Camera* camera) { this->camera_ = camera; }
		//ライトグループセット
		static void SetLightGroup(LightGroup* lightGroup) { Object3d::lightGroup_ = lightGroup; }
		//ビルボードセット
		void SetBillboard(const bool isBillboard) { this->isBillboard_ = isBillboard; }

		//コライダーセット
		void SetCollider(BaseCollider* collider);
		//モデルゲット
		const Model* GetModel() const { return model_; }

		//ワールド座標の取得
		const XMFLOAT3 GetWorldPosition() const {

			//ワールド座標を取得
			XMFLOAT3 worldPos;

			//ワールド行列の平行移動成分を取得
			worldPos.x = position_.x;
			worldPos.y = position_.y;
			worldPos.z = position_.z;

			return worldPos;
		}
	};

}