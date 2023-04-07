#include "Sprite.h"

using namespace DirectX;

void Sprite::Initialize(SpriteCommon* spCommon, uint32_t textureIndex)
{
	HRESULT result;
	assert(spCommon);
	this->spCommon_ = spCommon;
	
	if (textureIndex != UINT32_MAX)
	{
		textureIndex_ = textureIndex;
		AdjustTextureSize();
		//テクスチャサイズをスプライトのサイズに適用
		size_ = textureSize_;
	}

	//サイズ
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	//頂点バッファ
	D3D12_HEAP_PROPERTIES heapProp{};	//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU転送用
	//リソース設定

	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファ生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリを取得
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	//全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];	//座標をコピー
	}
	//繋がりを解除
	vertBuff->Unmap(0, nullptr);

	//定数バッファ
	//マテリアル
	CreateConstBufferMaterial();
	constMapMaterial->color = color_;

	//変換行列
	CreateConstBufferTransform();

	matProjection = XMMatrixOrthographicOffCenterLH(0, (float)WinApp::window_width, (float)WinApp::window_height, 0, 0, 1);


	//スケーリング等計算
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(rotationZ_);

	matTrans = XMMatrixTranslation(position_.x, position_.y, 0.0f);
	//ワールド行列合成
	matWorld = XMMatrixIdentity();
	matWorld *= matRot;		//回転反映
	matWorld *= matTrans;	//平行移動反映

	constMapTransform->mat = matWorld * matProjection;

	//頂点バッファビュー
	//GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	//頂点一つ分のサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);

}
void Sprite::Update()
{
	//色情報をGPUに転送
	constMapMaterial->color = color_;

	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;
	//左右反転
	if (isFlipX_)
	{
		left = -left;
		right = -right;
	}
	//上下反転
	if (isFlipY_)
	{
		top = -top;
		bottom = -bottom;
	}
	
	//頂点データ
	vertices[LB].pos = { left,bottom,0.0f };
	vertices[LT].pos = { left,top,0.0f };
	vertices[RB].pos = { right,bottom,0.0f };
	vertices[RT].pos = { right,top,0.0f };

	ComPtr<ID3D12Resource> texBuff = spCommon_->GetTextureBuffer(textureIndex_);
	//指定番号の画像が読み込み済みなら
	if (texBuff)
	{
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = texBuff->GetDesc();

		float tex_left = textureLeftTop_.x / resDesc.Width;
		float tex_right = (textureLeftTop_.x + textureSize_.x) / resDesc.Width;
		float tex_top = textureLeftTop_.y / resDesc.Height;
		float tex_bottom = (textureLeftTop_.y + textureSize_.y) / resDesc.Height;
		//頂点のUVに反映する
		vertices[LB].uv = { tex_left,tex_bottom, };		//左下
		vertices[LT].uv = { tex_left,tex_top, };		//左上
		vertices[RB].uv = { tex_right,tex_bottom, };	//右下
		vertices[RT].uv = { tex_right,tex_top, };		//右上
	}

	//頂点データをGPUに転送
	std::copy(std::begin(vertices), std::end(vertices), vertMap);

	//平行投影変換
	matProjection = XMMatrixOrthographicOffCenterLH(0, (float)WinApp::window_width, (float)WinApp::window_height, 0, 0, 1);

	//スケーリング等計算
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(rotationZ_);

	matTrans = XMMatrixTranslation(position_.x, position_.y, 0.0f);

	//ワールド行列合成
	matWorld = XMMatrixIdentity();
	matWorld *= matRot;		//回転反映
	matWorld *= matTrans;	//平行移動反映

	constMapTransform->mat = matWorld * matProjection;
	

}
void Sprite::Draw()
{
	//非表示
	if (isInvisible_)
	{
		return;
	}
	//テクスチャコマンド
	spCommon_->SetTextureCommands(textureIndex_);
	//頂点バッファビューの設定コマンド
	spCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	//定数バッファビュー(CBVの設定コマンド)
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());
	//描画コマンド
	spCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(_countof(vertices), 1, 0, 0);
}

void Sprite::CreateConstBufferMaterial()
{
	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//ヒープ設定
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourseDesc{};
	cbResourseDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourseDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;//256バイトアラインメント
	cbResourseDesc.Height = 1;
	cbResourseDesc.DepthOrArraySize = 1;
	cbResourseDesc.MipLevels = 1;
	cbResourseDesc.SampleDesc.Count = 1;
	cbResourseDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourseDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);//マッピング
	assert(SUCCEEDED(result));
}
void Sprite::CreateConstBufferTransform()
{
	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//ヒープ設定
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourseDesc{};
	cbResourseDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourseDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;//256バイトアラインメント
	cbResourseDesc.Height = 1;
	cbResourseDesc.DepthOrArraySize = 1;
	cbResourseDesc.MipLevels = 1;
	cbResourseDesc.SampleDesc.Count = 1;
	cbResourseDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourseDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);//マッピング
	assert(SUCCEEDED(result));
}
void Sprite::AdjustTextureSize() {
	ID3D12Resource* texBuff = spCommon_->GetTextureBuffer(textureIndex_);
	assert(texBuff);

	//テクスチャ情報取得
	D3D12_RESOURCE_DESC resDesc = texBuff->GetDesc();

	//テクスチャサイズ取得
	textureSize_.x = static_cast<float>(resDesc.Width);
	textureSize_.y = static_cast<float>(resDesc.Height);
}