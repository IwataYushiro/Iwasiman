#include "ParticleManager.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

/*

*	ParticleManager.cpp

*	パーティクルマネージャー

*/

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const float ParticleManager::RADIUS = 5.0f;				// 底面の半径
const float ParticleManager::PRIZM_HEIGHT = 8.0f;			// 柱の高さ
ID3D12Device* ParticleManager::device_ = nullptr;
ID3D12GraphicsCommandList* ParticleManager::cmdList_ = nullptr;
ComPtr<ID3D12RootSignature> ParticleManager::rootsignature_;
ComPtr<ID3D12PipelineState> ParticleManager::pipelinestate_;

void ParticleManager::StaticInitialize(ID3D12Device* device)
{
	// nullptrチェック
	assert(device);

	device_ = device;

	Particle::SetDevice(device_);

	// パイプライン初期化
	InitializeGraphicsPipeline();


}

void ParticleManager::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(ParticleManager::cmdList_ == nullptr);

	// コマンドリストをセット
	ParticleManager::cmdList_ = cmdList;

	// パイプラインステートの設定
	cmdList->SetPipelineState(pipelinestate_.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootsignature_.Get());
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void ParticleManager::PostDraw()
{
	// コマンドリストを解除
	ParticleManager::cmdList_ = nullptr;
}

ParticleManager* ParticleManager::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	ParticleManager* ins = new ParticleManager();
	if (ins == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!ins->Initialize()) {
		delete ins;
		assert(0);
		return nullptr;
	}

	return ins;
}

void ParticleManager::InitializeGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> gsBlob; // ジオメトリシェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shader/Particle/ParticleVS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ジオメトリシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shader/Particle/ParticleGS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "gs_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&gsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}
	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shader/Particle/ParticlePS.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{//スケール
			"TEXCOORD", 0, DXGI_FORMAT_R32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{//色
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	//デプスの書き込みを禁止
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//加算合成
	/*blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	blenddesc.DestBlend = D3D12_BLEND_ONE;*/
	//減算合成
	/*blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	blenddesc.DestBlend = D3D12_BLEND_ONE;*/

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[2];
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ルートシグネチャの生成
	result = device_->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature_));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = rootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = device_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate_));
	assert(SUCCEEDED(result));

}


bool ParticleManager::Initialize()
{
	// nullptrチェック
	assert(device_);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

	HRESULT result;

	// 定数バッファの生成
	result = device_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	return true;
}

void ParticleManager::Update()
{

	HRESULT result;

	particle_->Update();
	XMMATRIX matView = camera_->GetMatViewProjection();
	XMMATRIX matBillboard = camera_->GetMatBillboard();

	// 定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	constMap->mat = matView;	// 行列の合成
	constMap->matBillboard = matBillboard;
	constBuff_->Unmap(0, nullptr);
}

void ParticleManager::Draw()
{
	// nullptrチェック
	assert(device_);
	assert(ParticleManager::cmdList_);

	// 定数バッファビューをセット
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

	particle_->Draw(cmdList_);
}

void ParticleManager::Active(Particle* p, const XMFLOAT3& setmove,const XMFLOAT3& setpos, const XMFLOAT3& setvel,
 const XMFLOAT3& setacc, const int& setnum, const XMFLOAT2& setscale, const XMFLOAT4& start_color, const XMFLOAT4& end_color)
{
	for (int i = 0; i < setnum; i++)
	{
		//X,Y,Z全て{-20.0f,20.0f}でランダムに分布
		const XMFLOAT3 md_pos = setpos;
		const XMFLOAT3 md_move = setmove;
		XMFLOAT3 pos{};
		pos.x = ((float)rand() / RAND_MAX * md_pos.x - md_pos.x / 2.0f) + setmove.x;
		pos.y = ((float)rand() / RAND_MAX * md_pos.y - md_pos.y / 2.0f) + setmove.y;
		pos.z = ((float)rand() / RAND_MAX * md_pos.z - md_pos.z / 2.0f) + setmove.z;
		//X,Y,Z全て{0.1f,0.1f}でランダムに分布
		const XMFLOAT3 md_vel = setvel;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel.x - md_vel.x / 2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel.y - md_vel.y / 2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel.z - md_vel.z / 2.0f;
		//重力に見立ててYのみ{0.001f,0}でランダムに分布
		XMFLOAT3 acc{};
		const XMFLOAT3 md_acc = setacc;
		acc.y = -(float)rand() / RAND_MAX * md_acc.y;

		//追加
		p->Add(60, pos, vel, acc, setscale.x, setscale.y, start_color, end_color);
	}
}
void ParticleManager::ActiveX(Particle* p, const XMFLOAT3& setmove, const XMFLOAT3& setpos, const XMFLOAT3& setvel,
	const XMFLOAT3& setacc, const int& setnum, const XMFLOAT2& setscale, const XMFLOAT4& start_color, const XMFLOAT4& end_color)
{
	for (int i = 0; i < setnum; i++)
	{
		//X,Y,Z全て{-20.0f,20.0f}でランダムに分布
		const XMFLOAT3 md_pos = setpos;
		XMFLOAT3 pos{};
		pos.x = ((float)rand() / RAND_MAX * md_pos.x / 2.0f) + setmove.x;
		pos.y = ((float)rand() / RAND_MAX * md_pos.y - md_pos.y / 2.0f) + setmove.y;
		pos.z = ((float)rand() / RAND_MAX * md_pos.z - md_pos.z / 2.0f) + setmove.z;
		//X,Y,Z全て{0.1f,0.1f}でランダムに分布
		const XMFLOAT3 md_vel = setvel;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel.x / 2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel.y - md_vel.y / 2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel.z - md_vel.z / 2.0f;
		//重力に見立ててYのみ{0.001f,0}でランダムに分布
		XMFLOAT3 acc{};
		const XMFLOAT3 md_acc = setacc;
		acc.y = -(float)rand() / RAND_MAX * md_acc.y;

		//追加
		p->Add(60, pos, vel, acc, setscale.x, setscale.y, start_color, end_color);
	}
}

void ParticleManager::ActiveY(Particle* p, const XMFLOAT3& setmove, const XMFLOAT3& setpos, const XMFLOAT3& setvel,
	const XMFLOAT3& setacc, const int& setnum, const XMFLOAT2& setscale, const XMFLOAT4& start_color, const XMFLOAT4& end_color)
{
	for (int i = 0; i < setnum; i++)
	{
		//X,Y,Z全て{-20.0f,20.0f}でランダムに分布
		const XMFLOAT3 md_pos = setpos;
		XMFLOAT3 pos{};
		pos.x = ((float)rand() / RAND_MAX * md_pos.x - md_pos.x / 2.0f) + setmove.x;
		pos.y = ((float)rand() / RAND_MAX * md_pos.y / 2.0f) + setmove.y;
		pos.z = ((float)rand() / RAND_MAX * md_pos.z - md_pos.z / 2.0f) + setmove.z;
		//X,Y,Z全て{0.1f,0.1f}でランダムに分布
		const XMFLOAT3 md_vel = setvel;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel.x - md_vel.x / 2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel.y / 2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel.z - md_vel.z / 2.0f;
		//重力に見立ててYのみ{0.001f,0}でランダムに分布
		XMFLOAT3 acc{};
		const XMFLOAT3 md_acc = setacc;
		acc.y = -(float)rand() / RAND_MAX * md_acc.y;

		//追加
		p->Add(60, pos, vel, acc, setscale.x, setscale.y, start_color, end_color);
	}
}

void ParticleManager::ActiveZ(Particle* p, const XMFLOAT3& setmove, const XMFLOAT3& setpos, const XMFLOAT3& setvel,
	const XMFLOAT3& setacc, const int& setnum, const XMFLOAT2& setscale, const XMFLOAT4& start_color, const XMFLOAT4& end_color)
{
	for (int i = 0; i < setnum; i++)
	{
		//X,Y,Z全て{-20.0f,20.0f}でランダムに分布
		const XMFLOAT3 md_pos = setpos;
		XMFLOAT3 pos{};
		pos.x = ((float)rand() / RAND_MAX * md_pos.x - md_pos.x / 2.0f) + setmove.x;
		pos.y = ((float)rand() / RAND_MAX * md_pos.y - md_pos.y / 2.0f) + setmove.y;
		pos.z = ((float)rand() / RAND_MAX * md_pos.z / 2.0f) + setmove.z;
		//X,Y,Z全て{0.1f,0.1f}でランダムに分布
		const XMFLOAT3 md_vel = setvel;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel.x - md_vel.x / 2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel.y - md_vel.y / 2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel.z / 2.0f;
		//重力に見立ててYのみ{0.001f,0}でランダムに分布
		XMFLOAT3 acc{};
		const XMFLOAT3 md_acc = setacc;
		acc.y = -(float)rand() / RAND_MAX * md_acc.y;

		//追加
		p->Add(60, pos, vel, acc, setscale.x, setscale.y, start_color, end_color);
	}
}
