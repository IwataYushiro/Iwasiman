#include "Sprite.h"

using namespace DirectX;

/*

*	Sprite.cpp

*	1枚のスプライト

*/

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
	UINT sizeVB = static_cast<UINT>(sizeof(vertices_[0]) * _countof(vertices_));

	//頂点バッファ
	D3D12_HEAP_PROPERTIES heapProp{};	//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU転送用
	//リソース設定

	resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc_.Width = sizeVB;
	resDesc_.Height = 1;
	resDesc_.DepthOrArraySize = 1;
	resDesc_.MipLevels = 1;
	resDesc_.SampleDesc.Count = 1;
	resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファ生成
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc_,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_));

	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリを取得
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
	assert(SUCCEEDED(result));

	//全頂点に対して
	for (int i = 0; i < _countof(vertices_); i++)
	{
		vertMap_[i] = vertices_[i];	//座標をコピー
	}
	//繋がりを解除
	vertBuff_->Unmap(0, nullptr);

	//定数バッファ
	//マテリアル
	CreateConstBufferMaterial();
	constMapMaterial_->color = color_;

	//変換行列
	CreateConstBufferTransform();

	matProjection_ = XMMatrixOrthographicOffCenterLH(0, (float)WinApp::WINDOW_WIDTH, (float)WinApp::WINDOW_HEIGHT, 0, 0, 1);


	//スケーリング等計算
	matRot_ = XMMatrixIdentity();
	matRot_ *= XMMatrixRotationZ(rotationZ_);

	matTrans_ = XMMatrixTranslation(position_.x, position_.y, 0.0f);
	//ワールド行列合成
	matWorld_ = XMMatrixIdentity();
	matWorld_ *= matRot_;		//回転反映
	matWorld_ *= matTrans_;	//平行移動反映

	constMapTransform_->mat = matWorld_ * matProjection_;

	//頂点バッファビュー
	//GPU仮想アドレス
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView_.SizeInBytes = sizeVB;
	//頂点一つ分のサイズ
	vbView_.StrideInBytes = sizeof(vertices_[0]);

}
void Sprite::Update()
{
	//色情報をGPUに転送
	constMapMaterial_->color = color_;

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
	vertices_[LB].pos = { left,bottom,0.0f };
	vertices_[LT].pos = { left,top,0.0f };
	vertices_[RB].pos = { right,bottom,0.0f };
	vertices_[RT].pos = { right,top,0.0f };

	ComPtr<ID3D12Resource> texBuff = spCommon_->GetTextureBuffer(textureIndex_);
	//指定番号の画像が読み込み済みなら
	if (texBuff)
	{
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC texResDesc = texBuff->GetDesc();

		float tex_left = textureLeftTop_.x / texResDesc.Width;
		float tex_right = (textureLeftTop_.x + textureSize_.x) / texResDesc.Width;
		float tex_top = textureLeftTop_.y / texResDesc.Height;
		float tex_bottom = (textureLeftTop_.y + textureSize_.y) / texResDesc.Height;
		//頂点のUVに反映する
		vertices_[LB].uv = { tex_left,tex_bottom, };		//左下
		vertices_[LT].uv = { tex_left,tex_top, };		//左上
		vertices_[RB].uv = { tex_right,tex_bottom, };	//右下
		vertices_[RT].uv = { tex_right,tex_top, };		//右上
	}

	//頂点データをGPUに転送
	std::copy(std::begin(vertices_), std::end(vertices_), vertMap_);

	//平行投影変換
	matProjection_ = XMMatrixOrthographicOffCenterLH(0, (float)WinApp::WINDOW_WIDTH, (float)WinApp::WINDOW_HEIGHT, 0, 0, 1);

	//スケーリング等計算
	matRot_ = XMMatrixIdentity();
	matRot_ *= XMMatrixRotationZ(rotationZ_);

	matTrans_ = XMMatrixTranslation(position_.x, position_.y, 0.0f);

	//ワールド行列合成
	matWorld_ = XMMatrixIdentity();
	matWorld_ *= matRot_;	//回転反映
	matWorld_ *= matTrans_;	//平行移動反映

	constMapTransform_->mat = matWorld_ * matProjection_;
	

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
	spCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	//定数バッファビュー(CBVの設定コマンド)
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());
	//描画コマンド
	spCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(_countof(vertices_), 1, 0, 0);
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
		IID_PPV_ARGS(&constBuffMaterial_));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuffMaterial_->Map(0, nullptr, (void**)&constMapMaterial_);//マッピング
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
		IID_PPV_ARGS(&constBuffTransform_));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuffTransform_->Map(0, nullptr, (void**)&constMapTransform_);//マッピング
	assert(SUCCEEDED(result));
}
void Sprite::AdjustTextureSize() {
	ID3D12Resource* texBuff = spCommon_->GetTextureBuffer(textureIndex_);
	assert(texBuff);

	//テクスチャ情報取得
	D3D12_RESOURCE_DESC texResDesc = texBuff->GetDesc();

	//テクスチャサイズ取得
	textureSize_.x = static_cast<float>(texResDesc.Width);
	textureSize_.y = static_cast<float>(texResDesc.Height);
}