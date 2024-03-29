#include "ObjectFbx.h"
#include "FbxLoader.h"
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace IwasiEngine;
/*

*	ObgextFbx.cpp

*	FBXオブジェクト

*/

//静的メンバ変数の実体
ID3D12Device* ObjectFbx::device_ = nullptr;
ID3D12GraphicsCommandList* ObjectFbx::cmdList_ = nullptr;
ComPtr<ID3D12RootSignature> ObjectFbx::rootSignature_;
ComPtr<ID3D12PipelineState> ObjectFbx::pipelineState_;

void ObjectFbx::StaticInitialize(ID3D12Device* device)
{
	// nullptrチェック
	assert(device);
	device_ = device;

	//モデルにデバイスをセット
	ModelFbx::SetDevice(device);

	// パイプライン初期化
	CreateGraphicsPipeline();
}

void ObjectFbx::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(ObjectFbx::cmdList_ == nullptr);

	// コマンドリストをセット
	ObjectFbx::cmdList_ = cmdList;

	//パイプラインステートの設定
	cmdList->SetPipelineState(pipelineState_.Get());
	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootSignature_.Get());
	//プリミティブ形状の設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void ObjectFbx::PostDraw()
{
	// コマンドリストを解除
	ObjectFbx::cmdList_ = nullptr;
}

ObjectFbx* ObjectFbx::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	ObjectFbx* objectFbx = new ObjectFbx();
	if (objectFbx == nullptr) {
		return nullptr;
	}
	//スケールをセット
	const float scale_val = 1.0f;
	objectFbx->scale_ = { scale_val,scale_val ,scale_val };
	// 初期化
	if (!objectFbx->Initialize()) {
		delete objectFbx;
		assert(0);
		return nullptr;
	}

	return objectFbx;
}

void ObjectFbx::CreateGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;    // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	assert(device_);

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shader/FBX/FBXVS.hlsl",    // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0",    // エントリーポイント名、シェーダーモデル指定
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
		assert(0);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shader/FBX/FBXPS.hlsl",    // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",    // エントリーポイント名、シェーダーモデル指定
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
		{ //影響を受けるボーン番号(4つ)
			"BONEINDICES",0,DXGI_FORMAT_R32G32B32A32_UINT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{ //ボーンのスキンウェイト(4つ)
			"BONEWEIGHTS",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

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
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	enum RenderTargetNum
	{
		RTN_RenderTarget0 = 0,
		RTN_RenderTarget1 = 1,
		RTN_Num = 2,//配列用、2個ある
	};
	gpipeline.BlendState.RenderTarget[RTN_RenderTarget0] = blenddesc;
	gpipeline.BlendState.RenderTarget[RTN_RenderTarget1] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = RTN_Num;    // 描画対象は1つ37 1->2
	gpipeline.RTVFormats[RTN_RenderTarget0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0〜255指定のRGBA37
	gpipeline.RTVFormats[RTN_RenderTarget1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0〜255指定のRGBA37
	const UINT samplingCountNum = 1;
	gpipeline.SampleDesc.Count = samplingCountNum; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジ
	const UINT descriptorNum = 1;
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, descriptorNum, 0); // t0 レジスタ

	const UINT CBDM_Register = 0;//マテリアル定数バッファのレジスタ
	const UINT CBDS_Register = 3;//スキン定数バッファのレジスタ
	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[RPI_Num]{};
	// CBV（座標変換行列用）
	rootparams[RPI_ConstBuffTransform].InitAsConstantBufferView(CBDM_Register, 0, D3D12_SHADER_VISIBILITY_ALL);
	// SRV（テクスチャ）
	rootparams[RPI_TexBuffSRV].InitAsDescriptorTable(descriptorNum, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	// CBV（スキニング用）
	rootparams[RPI_ConstBuffSkin].InitAsConstantBufferView(CBDS_Register, 0, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	const UINT staticSamplersNum = 1;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, staticSamplersNum, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ルートシグネチャの生成
	result = device_->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature_.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = rootSignature_.Get();

	// グラフィックスパイプラインの生成
	result = device_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }
}

bool ObjectFbx::Initialize()
{
	HRESULT result;

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	
	const CD3DX12_RESOURCE_DESC bufferTransform = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xff);
	// 定数バッファの生成
	//座標系
	result = device_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&bufferTransform,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBufferTransform_));

	const CD3DX12_RESOURCE_DESC bufferDataSkin = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff);
	//スキニング
	result = device_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&bufferDataSkin,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBufferSkin_));

	const int32_t startFlame = 1;
	frameTime_.SetTime(0, 0, 0, startFlame, 0, FbxTime::EMode::eFrames60);
	//定数バッファへデータを転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBufferSkin_->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < MAX_BONES; i++)
	{
		constMapSkin->bones[i] = XMMatrixIdentity();
	}
	constBufferSkin_->Unmap(0, nullptr);

	return true;
}

void ObjectFbx::Update()
{
	HRESULT result;
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
	
	//ビュプロ行列
	const XMMATRIX& matViewProjection = camera_->GetMatViewProjection();
	//モデルのメッシュトランスフォーム
	const XMMATRIX& modelTransform = modelF_->GetModelTransform();
	//カメラ座標
	const XMFLOAT3& cameraPos = camera_->GetEye();

	//定数バッファへのデータ転送
	ConstBufferDataTransform* constMapTransform = nullptr;
	result = constBufferTransform_->Map(0, nullptr, (void**)&constMapTransform);
	if (SUCCEEDED(result))
	{
		constMapTransform->viewProj = matViewProjection;
		constMapTransform->world = modelTransform * matWorld_;
		constMapTransform->cameraPos = cameraPos;
		constBufferTransform_->Unmap(0, nullptr);
	}

	//ボーン配列
	std::vector<ModelFbx::Bone>& bones = modelF_->GetBones();
	//アニメーション
	if (isPlayAnimation_)
	{
		//1フレーム進める
		currentTime_ += frameTime_;
		//最後まで再生したら先頭に戻す
		if (currentTime_ > endTime_)
		{
			currentTime_ = startTime_;
		}
	}
	//定数バッファへのデータ転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBufferSkin_->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < bones.size(); i++)
	{
	
		//現姿勢行列
		XMMATRIX matCurrentPose;
		//現姿勢行列の取得
		FbxAMatrix fbxCurrentPose 
			= bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime_);
		//XMMATRIXに変換
		FbxLoader::ConvertMatrixFromFBX(&matCurrentPose, fbxCurrentPose);
		//合成してスキニング行列に
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	
	}
	constBufferSkin_->Unmap(0, nullptr);
}

void ObjectFbx::Draw()
{
	assert(device_);
	assert(ObjectFbx::cmdList_);
	//モデルの割り当てが無ければ描画しない
	if (modelF_ == nullptr)
	{
		return;
	}
	//定数バッファビューセット
	cmdList_->SetGraphicsRootConstantBufferView(RPI_ConstBuffTransform, constBufferTransform_->GetGPUVirtualAddress());
	cmdList_->SetGraphicsRootConstantBufferView(RPI_ConstBuffSkin, constBufferSkin_->GetGPUVirtualAddress());

	//モデル描画
	modelF_->Draw(cmdList_);
}

void ObjectFbx::PlayAnimation()
{
	FbxScene* fbxScene = modelF_->GetFbxScene();
	//0番のアニメーション取得
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	//アニメーション情報がないなら終了
	if (animstack == nullptr)
	{
		return;
	}
	//アニメーションの名前を取得
	const char* animstackname = animstack->GetName();
	//アニメーションの時間情報
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);

	//開始時間取得
	startTime_ = takeinfo->mLocalTimeSpan.GetStart();
	//終了時間取得
	endTime_ = takeinfo->mLocalTimeSpan.GetStop();
	//開始時間に合わせる
	currentTime_ = startTime_;
	//アニメーション再生中状態にする
	isPlayAnimation_ = true;
}
