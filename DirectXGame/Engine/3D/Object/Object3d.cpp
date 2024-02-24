#include "Object3d.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <fstream>
#include <sstream>
#include <string>
#include <Vector>
#include "BaseCollider.h"
#include "CollisionManager.h"
#include "ImGuiManager.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace IwasiEngine;
/*

*	Object3d.cpp

*	3Dオブジェクト(.obj)

*/

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const float Object3d::radius = 5.0f;				// 底面の半径
const float Object3d::prizmHeight = 8.0f;			// 柱の高さ
ID3D12Device* Object3d::device_ = nullptr;
ID3D12GraphicsCommandList* Object3d::cmdList_ = nullptr;
ComPtr<ID3D12RootSignature> Object3d::rootsignature_;
ComPtr<ID3D12PipelineState> Object3d::pipelinestate_;
ComPtr<ID3DBlob> Object3d::rootSigBlob_;
ComPtr<ID3DBlob> Object3d::vsBlob_; // 頂点シェーダオブジェクト
ComPtr<ID3DBlob> Object3d::psBlob_;	// ピクセルシェーダオブジェクト
ComPtr<ID3DBlob> Object3d::errorBlob_; // エラーオブジェクト
LightGroup* Object3d::lightGroup_ = nullptr;

Object3d::~Object3d()
{
	if (collider_)
	{
		//CollisionManagerから登録解除
		CollisionManager::GetInstance()->RemoveCollider(collider_);
		delete collider_;
	}
}

void Object3d::StaticInitialize(ID3D12Device* device)
{

	// nullptrチェック
	assert(device);
	Object3d::device_ = device;

	// パイプライン初期化
	InitializeGraphicsPipeline();

	Model::StaticInitialize(device);
}

void Object3d::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Object3d::cmdList_ == nullptr);

	// コマンドリストをセット
	Object3d::cmdList_ = cmdList;

	// パイプラインステートの設定
	cmdList->SetPipelineState(pipelinestate_.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootsignature_.Get());
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Object3d::PostDraw()
{
	// コマンドリストを解除
	Object3d::cmdList_ = nullptr;
}

std::unique_ptr<Object3d> Object3d::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	std::unique_ptr<Object3d> object3d = std::make_unique<Object3d>();
	if (object3d == nullptr) {
		return nullptr;
	}
	//スケールをセット
	const float scale_val = 1.0f;
	object3d->scale_ = { scale_val,scale_val ,scale_val };
	// 初期化
	if (!object3d->Initialize()) {
		assert(0);
		return nullptr;
	}

	return object3d;
}

void Object3d::InitializeGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	//ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	//ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
	//ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shader/OBJVS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob_, &errorBlob_);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob_->GetBufferSize());

		std::copy_n((char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shader/OBJPS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob_, &errorBlob_);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob_->GetBufferSize());

		std::copy_n((char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // 法線ベクトル(1行で書いたほうが見やすい)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv座標(1行で書いたほうが見やすい)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob_.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob_.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	const int defaultRenderTargetNum = 0;
	gpipeline.BlendState.RenderTarget[defaultRenderTargetNum] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他
	const UINT renderTargetNum = 1;
	const UINT sampleDescCount = 1;

	gpipeline.NumRenderTargets = renderTargetNum;	// 描画対象は1つ
	gpipeline.RTVFormats[defaultRenderTargetNum] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = sampleDescCount; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジ
	const UINT descriptorNum = 1;
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, descriptorNum, 0); // t0 レジスタ

	// ルートパラメータ
	const UINT CBDT_Register = 0;//座標定数バッファのレジスタ
	const UINT CBDM_Register = 1;//マテリアル定数バッファのレジスタ
	const UINT CBDL_Register = 2;//ライト定数バッファのレジスタ

	CD3DX12_ROOT_PARAMETER rootparams[RPI_Num];
	rootparams[RPI_ConstBuffTransform].InitAsConstantBufferView(CBDT_Register, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[RPI_ConstBuffMaterial].InitAsConstantBufferView(CBDM_Register, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[RPI_TexBuff].InitAsDescriptorTable(descriptorNum, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[RPI_ConstBuffLight].InitAsConstantBufferView(CBDL_Register, 0, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	const UINT staticSamplersNum = 1;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, staticSamplersNum, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob_, &errorBlob_);
	// ルートシグネチャの生成
	result = device_->CreateRootSignature(0, rootSigBlob_->GetBufferPointer(), rootSigBlob_->GetBufferSize(), IID_PPV_ARGS(&rootsignature_));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = rootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = device_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate_));
	assert(SUCCEEDED(result));

}


bool Object3d::Initialize()
{
	// nullptrチェック
	assert(device_);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	const CD3DX12_RESOURCE_DESC resourceDescB0 =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff);
	
	HRESULT result;

	// 定数バッファの生成
	result = device_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDescB0, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuffB0_));
	assert(SUCCEEDED(result));
	
	// 定数バッファのマッピング
	result = constBuffB0_->Map(0, nullptr, (void**)&constMap0_);
	assert(SUCCEEDED(result));

	//クラス名の文字列を取得
	name_ = typeid(*this).name();

	return true;
}

void Object3d::Update()
{
	HRESULT result;
	UpdateWorldMatrix();
	const XMMATRIX& matViewProjection = camera_->GetMatViewProjection();
	const XMFLOAT3& cameraPos = camera_->GetEye();

	// 定数バッファへデータ転送
	result = constBuffB0_->Map(0, nullptr, (void**)&constMap0_);
	constMap0_->color = color_;
	constMap0_->viewproj = matViewProjection;	// 行列の合成
	constMap0_->world = matWorld_;
	constMap0_->cameraPos = cameraPos;
	constBuffB0_->Unmap(0, nullptr);

	//当たり判定更新
	if (collider_)
	{
		collider_->Update();
	}
}

void Object3d::UpdateWorldMatrix()
{
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation_.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation_.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation_.y));
	matTrans = XMMatrixTranslation(position_.x, position_.y, position_.z);

	// ワールド行列の合成
	matWorld_ = XMMatrixIdentity(); // 変形をリセット
	matWorld_ *= matScale; // ワールド行列にスケーリングを反映
	matWorld_ *= matRot; // ワールド行列に回転を反映
	matWorld_ *= matTrans; // ワールド行列に平行移動を反映

	if (isBillboard_)
	{
		const XMMATRIX& matBillboard = camera_->GetMatBillboard();
		// ワールド行列の合成
		matWorld_ = XMMatrixIdentity(); // 変形をリセット
		matWorld_ *= matScale; // ワールド行列にスケーリングを反映
		matWorld_ *= matRot; // ワールド行列に回転を反映
		matWorld_ *= matBillboard;
		matWorld_ *= matTrans; // ワールド行列に平行移動を反映
	}
	// 親オブジェクトがあれば
	if (parent_ != nullptr) {
		// 親オブジェクトのワールド行列を掛ける
		matWorld_ *= parent_->matWorld_;
	}
}

void Object3d::Draw()
{
	// nullptrチェック
	assert(device_);
	assert(Object3d::cmdList_);

	//モデルがセットされていなければ描画をスキップ
	if (model_ == nullptr) return;

	// 定数バッファビューをセット
	cmdList_->SetGraphicsRootConstantBufferView(RPI_ConstBuffTransform, constBuffB0_->GetGPUVirtualAddress());
	//ライト描画
	lightGroup_->Draw(cmdList_, RPI_ConstBuffLight);
	//モデルを描画
	model_->Draw(cmdList_);
}

void Object3d::SetCollider(BaseCollider* collider)
{
	//コライダーのセット
	collider->SetObject(this);
	this->collider_ = collider;
	//コリジョンマネージャーに登録
	CollisionManager::GetInstance()->AddCollider(collider);
	//コライダー更新
	UpdateWorldMatrix();
	collider->Update();
}

const XMFLOAT3 Object3d::GetWorldPosition() const
{

	//ワールド座標を取得
	XMFLOAT3 worldPos;

	//ワールド行列の平行移動成分を取得
	worldPos.x = position_.x;
	worldPos.y = position_.y;
	worldPos.z = position_.z;

	return worldPos;
}

void Object3d::Trans()
{
	//行列更新
	//ワールド行列
	matWorld_ = XMMatrixIdentity();
	//スケール
	XMMATRIX matScale = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	//回転
	XMMATRIX matRot = XMMatrixRotationZ(rotation_.z)
		* XMMatrixRotationX(rotation_.x)
		* XMMatrixRotationY(rotation_.y);
	//座標
	XMMATRIX matTrans = XMMatrixTranslation(position_.x, position_.y, position_.z);

	//合成してローカル変数に転送
	matWorld_ = matScale * matRot * matTrans;
}

void Object3d::TestObjectColor()
{
#ifdef _DEBUG
	//ImGuiに渡す用の変数
	//エフェクトの色
	float icolor[XYZW_Num] = { color_.x,color_.y,color_.z, color_.w };
	//ウィンドウポジション
	struct ImGuiWindowPosition
	{
		const float X = 400.0f;
		const float Y = 0.0f;
	};
	ImGuiWindowPosition iPos;
	//ウィンドウサイズ
	struct ImguiWindowSize
	{
		const float width = 300.0f;
		const float height = 50.0f;
	};
	ImguiWindowSize iSize;
	//調整はスライダーで
	ImGui::Begin("objColorTest");
	ImGui::SetWindowPos(ImVec2(iPos.X, iPos.Y));
	ImGui::SetWindowSize(ImVec2(iSize.width, iSize.height));
	ImGui::SliderFloat4("color", icolor, 0.0f, 1.0f);
	ImGui::End();

	//値を適応
	color_={ icolor[XYZW_X],icolor[XYZW_Y],icolor[XYZW_Z],icolor[XYZW_W] };
	
#endif // _DEBUG
	Update();
}
