#include "Sprite.h"

using namespace DirectX;

/*

*	Sprite.cpp

*	1���̃X�v���C�g

*/

void Sprite::Initialize(const SpriteCommon* spCommon, const uint32_t textureIndex)
{
	HRESULT result;
	assert(spCommon);
	this->spCommon_ = const_cast<SpriteCommon*>(spCommon);
	
	if (textureIndex != UINT32_MAX)
	{
		textureIndex_ = textureIndex;
		AdjustTextureSize();
		//�e�N�X�`���T�C�Y���X�v���C�g�̃T�C�Y�ɓK�p
		size_ = textureSize_;
	}

	//�T�C�Y
	UINT sizeVB = static_cast<UINT>(sizeof(vertices_[LB]) * _countof(vertices_));

	//���_�o�b�t�@
	D3D12_HEAP_PROPERTIES heapProp{};	//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU�]���p
	//���\�[�X�ݒ�

	resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc_.Width = sizeVB;
	resDesc_.Height = resDescPreset_.height;
	resDesc_.DepthOrArraySize = resDescPreset_.arraysize;
	resDesc_.MipLevels = resDescPreset_.mipLevels;
	resDesc_.SampleDesc.Count = resDescPreset_.sampleCount;
	resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���_�o�b�t�@����
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc_,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_));

	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
	assert(SUCCEEDED(result));

	//�S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices_); i++)
	{
		vertMap_[i] = vertices_[i];	//���W���R�s�[
	}
	//�q���������
	vertBuff_->Unmap(0, nullptr);

	//�萔�o�b�t�@
	//�}�e���A��
	CreateConstBufferMaterial();
	constMapMaterial_->color = color_;

	//�ϊ��s��
	CreateConstBufferTransform();

	matProjection_ = XMMatrixOrthographicOffCenterLH(projectionPreset_.viewLeft, projectionPreset_.viewRight,
 projectionPreset_.viewBottom, projectionPreset_.viewTop, projectionPreset_.nearZ, projectionPreset_.farZ);


	//�X�P�[�����O���v�Z
	matRot_ = XMMatrixIdentity();
	matRot_ *= XMMatrixRotationZ(rotationZ_);

	matTrans_ = XMMatrixTranslation(position_.x, position_.y, 0.0f);
	//���[���h�s�񍇐�
	matWorld_ = XMMatrixIdentity();
	matWorld_ *= matRot_;		//��]���f
	matWorld_ *= matTrans_;	//���s�ړ����f

	constMapTransform_->mat = matWorld_ * matProjection_;

	//���_�o�b�t�@�r���[
	//GPU���z�A�h���X
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView_.SizeInBytes = sizeVB;
	//���_����̃T�C�Y
	vbView_.StrideInBytes = sizeof(vertices_[LB]);

}
void Sprite::Update()
{
	//�F����GPU�ɓ]��
	constMapMaterial_->color = color_;

	struct AnchorPointPreset
	{
		const float left = 0.0f;
		const float right = 1.0f;
		const float top = 0.0f;
		const float bottom = 1.0f;
	};
	AnchorPointPreset anchorPointPreset;

	float left = (anchorPointPreset.left - anchorPoint_.x) * size_.x;
	float right = (anchorPointPreset.right - anchorPoint_.x) * size_.x;
	float top = (anchorPointPreset.top - anchorPoint_.y) * size_.y;
	float bottom = (anchorPointPreset.bottom - anchorPoint_.y) * size_.y;
	//���E���]
	if (isFlipX_)
	{
		left = -left;
		right = -right;
	}
	//�㉺���]
	if (isFlipY_)
	{
		top = -top;
		bottom = -bottom;
	}
	
	//���_�f�[�^
	vertices_[LB].pos = { left,bottom,0.0f };
	vertices_[LT].pos = { left,top,0.0f };
	vertices_[RB].pos = { right,bottom,0.0f };
	vertices_[RT].pos = { right,top,0.0f };

	ComPtr<ID3D12Resource> texBuff = spCommon_->GetTextureBuffer(textureIndex_);
	//�w��ԍ��̉摜���ǂݍ��ݍς݂Ȃ�
	if (texBuff)
	{
		//�e�N�X�`�����擾
		D3D12_RESOURCE_DESC texResDesc = texBuff->GetDesc();

		float tex_left = textureLeftTop_.x / texResDesc.Width;
		float tex_right = (textureLeftTop_.x + textureSize_.x) / texResDesc.Width;
		float tex_top = textureLeftTop_.y / texResDesc.Height;
		float tex_bottom = (textureLeftTop_.y + textureSize_.y) / texResDesc.Height;
		//���_��UV�ɔ��f����
		vertices_[LB].uv = { tex_left,tex_bottom, };		//����
		vertices_[LT].uv = { tex_left,tex_top, };		//����
		vertices_[RB].uv = { tex_right,tex_bottom, };	//�E��
		vertices_[RT].uv = { tex_right,tex_top, };		//�E��
	}

	//���_�f�[�^��GPU�ɓ]��
	std::copy(std::begin(vertices_), std::end(vertices_), vertMap_);

	//���s���e�ϊ�
	matProjection_ = XMMatrixOrthographicOffCenterLH(projectionPreset_.viewLeft, projectionPreset_.viewRight,
		projectionPreset_.viewBottom, projectionPreset_.viewTop, projectionPreset_.nearZ, projectionPreset_.farZ);

	//�X�P�[�����O���v�Z
	matRot_ = XMMatrixIdentity();
	matRot_ *= XMMatrixRotationZ(rotationZ_);

	matTrans_ = XMMatrixTranslation(position_.x, position_.y, 0.0f);

	//���[���h�s�񍇐�
	matWorld_ = XMMatrixIdentity();
	matWorld_ *= matRot_;	//��]���f
	matWorld_ *= matTrans_;	//���s�ړ����f

	constMapTransform_->mat = matWorld_ * matProjection_;
	

}
void Sprite::Draw()const
{
	//��\��
	if (isInvisible_)
	{
		return;
	}
	//�e�N�X�`���R�}���h
	spCommon_->SetTextureCommands(textureIndex_);
	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	const UINT viewsNum = 1;
	spCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, viewsNum, &vbView_);

	//�萔�o�b�t�@�r���[(CBV�̐ݒ�R�}���h)
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(spCommon_->RPI_ConstBuffMaterial, constBuffMaterial_->GetGPUVirtualAddress());

	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(spCommon_->RPI_ConstBuffTransform, constBuffTransform_->GetGPUVirtualAddress());
	//�`��R�}���h
	const UINT instanceCount = 1;
	spCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(_countof(vertices_), instanceCount, 0, 0);
}

void Sprite::CreateConstBufferMaterial()
{
	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//�q�[�v�ݒ�
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
	//���\�[�X�ݒ�
	const UINT64 buffWidth = 0xff;
	D3D12_RESOURCE_DESC cbResourseDesc{};
	cbResourseDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourseDesc.Width = (sizeof(ConstBufferDataMaterial) + buffWidth) & ~buffWidth;//256�o�C�g�A���C�������g
	cbResourseDesc.Height = resDescPreset_.height;
	cbResourseDesc.DepthOrArraySize = resDescPreset_.arraysize;
	cbResourseDesc.MipLevels = resDescPreset_.mipLevels;
	cbResourseDesc.SampleDesc.Count = resDescPreset_.sampleCount;
	cbResourseDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�萔�o�b�t�@�̐���
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourseDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial_));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuffMaterial_->Map(0, nullptr, (void**)&constMapMaterial_);//�}�b�s���O
	assert(SUCCEEDED(result));
}
void Sprite::CreateConstBufferTransform()
{
	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//�q�[�v�ݒ�
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
	//���\�[�X�ݒ�
	const UINT64 buffWidth = 0xff;
	D3D12_RESOURCE_DESC cbResourseDesc{};
	cbResourseDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourseDesc.Width = (sizeof(ConstBufferDataTransform) + buffWidth) & ~buffWidth;//256�o�C�g�A���C�������g
	cbResourseDesc.Height = resDescPreset_.height;
	cbResourseDesc.DepthOrArraySize = resDescPreset_.arraysize;
	cbResourseDesc.MipLevels = resDescPreset_.mipLevels;
	cbResourseDesc.SampleDesc.Count = resDescPreset_.sampleCount;
	cbResourseDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�萔�o�b�t�@�̐���
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourseDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform_));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuffTransform_->Map(0, nullptr, (void**)&constMapTransform_);//�}�b�s���O
	assert(SUCCEEDED(result));
}
void Sprite::AdjustTextureSize() {
	ID3D12Resource* texBuff = spCommon_->GetTextureBuffer(textureIndex_);
	assert(texBuff);

	//�e�N�X�`�����擾
	D3D12_RESOURCE_DESC texResDesc = texBuff->GetDesc();

	//�e�N�X�`���T�C�Y�擾
	textureSize_.x = static_cast<float>(texResDesc.Width);
	textureSize_.y = static_cast<float>(texResDesc.Height);
}