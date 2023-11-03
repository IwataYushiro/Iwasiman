#include "ModelFbx.h"

/*

*	ModelFbx.cpp

*	FBX���f��

*/

//�ÓI�����o�ϐ��̎���
ID3D12Device* ModelFbx::device_ = nullptr;

ModelFbx::~ModelFbx()
{
	//Fbx�V�[���J��
	fbxScene_->Destroy();
}

void ModelFbx::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// nullptr�`�F�b�N
	assert(cmdList);

	// ���_�o�b�t�@�̐ݒ�
	const UINT viewsNum = 1;
	cmdList->IASetVertexBuffers(0, viewsNum, &vbView_);
	// �C���f�b�N�X�o�b�t�@�̐ݒ�
	cmdList->IASetIndexBuffer(&ibView_);

	// �f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV_.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// �V�F�[�_���\�[�X�r���[���Z�b�g
	const int32_t texBuffSRV = 1;
	cmdList->SetGraphicsRootDescriptorTable(texBuffSRV,
		descHeapSRV_->GetGPUDescriptorHandleForHeapStart());
	
	// �`��R�}���h
	const UINT instanceCount = 1;
	cmdList->DrawIndexedInstanced((UINT)indices_.size(), instanceCount, 0, 0, 0);

}

void ModelFbx::CreateBuffers(ID3D12Device* device)
{
	HRESULT result;
	//���_�f�[�^�S�̂̃T�C�Y
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUvSkin) * vertices_.size());
	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
	//���_�o�b�t�@����
	result = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_));
	//���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}
	// ���_�o�b�t�@�r���[�̍쐬
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	const int32_t startVerticesNum = 0;
	vbView_.StrideInBytes = sizeof(vertices_[startVerticesNum]);

	//���_�C���f�b�N�X�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices_.size());
	// ���\�[�X�ݒ�
	resourceDesc.Width = sizeIB;
	resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);

	// �C���f�b�N�X�o�b�t�@����
	result = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff_));

	// �C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned short* indexMap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {

		// �S�C���f�b�N�X�ɑ΂���
		std::copy(indices_.begin(), indices_.end(), indexMap);
		indexBuff_->Unmap(0, nullptr);
	}

	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;

	//�e�N�X�`���摜�f�[�^
	const DirectX::Image* img = scratchImg_.GetImage(0, 0, 0);
	assert(img);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata_.format,
		metadata_.width,
		(UINT)metadata_.height,
		(UINT16)metadata_.arraySize,
		(UINT16)metadata_.mipLevels);

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES texHeapProps =
		CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	// �e�N�X�`���p�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&texHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // �e�N�X�`���p�w��
		nullptr,
		IID_PPV_ARGS(&texbuff_));
	// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texbuff_->WriteToSubresource(
		0,
		nullptr,              // �S�̈�փR�s�[
		img->pixels,          // ���f�[�^�A�h���X
		(UINT)img->rowPitch,  // 1���C���T�C�Y
		(UINT)img->slicePitch // 1���T�C�Y
	);

	// SRV�p�̃f�X�N���v�^�q�[�v�𐶐�
	const UINT descriptorNum = 1;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
	descHeapDesc.NumDescriptors = descriptorNum; // �V�F�[�_�[���\�[�X�r���[1��
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeapSRV_));//����
	
	// SRV�̃V�F�[�_���\�[�X�r���[�쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // �ݒ�\����
	D3D12_RESOURCE_DESC resDesc = texbuff_->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	const UINT tex2DMipLevelNum = 1;
	srvDesc.Texture2D.MipLevels = tex2DMipLevelNum;

	device->CreateShaderResourceView(texbuff_.Get(), //�r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc, //�e�N�X�`���ݒ���
		descHeapSRV_->GetCPUDescriptorHandleForHeapStart()
	);

}
