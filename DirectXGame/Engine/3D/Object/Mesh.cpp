#include "Mesh.h"
#include "XYZ.h"
#include <d3dcompiler.h>
#include <cassert>

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

/*

*	Mesh.cpp

*	�`��(���b�V��)

*/

//�ÓI�����o�ϐ��̎���
ID3D12Device* Mesh::device_ = nullptr;

void Mesh::StaticInitialize(ID3D12Device* device)
{
	Mesh::device_ = device;
	//�}�e���A��������
	Material::StaticInitialize(device);
}

void Mesh::AddSmoothData(unsigned short indexPosition, unsigned short indexVertex)
{
	smoothData_[indexPosition].emplace_back(indexVertex);
}

void Mesh::CalculateSmoothedVertexNormals()
{
	auto itr = smoothData_.begin();
	for (; itr != smoothData_.end(); ++itr)
	{
		//�e�ʗp�̋��ʒ��_�R���N�V����
		std::vector<unsigned short>& v = itr->second;
		//�S���_�̖@���𕽋ς���
		XMVECTOR normal = {};
		for (unsigned short index : v)
		{
			normal += XMVectorSet(vertices_[index].normal.x, vertices_[index].normal.y, vertices_[index].normal.z, 0.0f);
		}
		normal = XMVector3Normalize(normal / (float)v.size());

		//���ʖ@�����g�p����S�Ă̒��_�f�[�^�ɏ�������
		for (unsigned short index : v)
		{
			vertices_[index].normal = { normal.m128_f32[XYZ_X],normal.m128_f32[XYZ_Y],normal.m128_f32[XYZ_Z] };
		}
	}
}

void Mesh::CreateBuffers()
{
	HRESULT result = S_FALSE;

	//std::vector<VertexPosNormalUv> realVertices;

	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices_.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices_.size());

	// �q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	// ���\�[�X�ݒ�
	//���\�[�X�f�X�N�̃v���Z�b�g
	struct ResDescPreset
	{
		const UINT height = 1;
		const UINT16 arraysize = 1;
		const UINT16 mipLevels = 1;
		const UINT sampleCount = 1;

	};
	ResDescPreset resDescPreset;
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeVB;
	resourceDesc.Height = resDescPreset.height;
	resourceDesc.DepthOrArraySize = resDescPreset.arraysize;
	resourceDesc.MipLevels = resDescPreset.mipLevels;
	resourceDesc.SampleDesc.Count = resDescPreset.sampleCount;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


	// ���_�o�b�t�@����
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}

	// ���_�o�b�t�@�r���[�̍쐬(���_�A�C���f�b�N�X�r���[�̓f�X�N���v�^�O�ō��)
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	const int32_t startVerticesNum = 0;
	vbView_.StrideInBytes = sizeof(vertices_[startVerticesNum]);

	// ���\�[�X�ݒ�
	resourceDesc.Width = sizeIB;
	// �C���f�b�N�X�o�b�t�@����
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
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

}

void Mesh::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// ���_�o�b�t�@�̐ݒ�
	const UINT viewsNum = 1;
	cmdList->IASetVertexBuffers(0, viewsNum, &vbView_);
	// �C���f�b�N�X�o�b�t�@�̐ݒ�
	cmdList->IASetIndexBuffer(&ibView_);
	// �V�F�[�_���\�[�X�r���[���Z�b�g
	const int32_t SRVNum = 2;
	cmdList->SetGraphicsRootDescriptorTable(SRVNum, material_->GetGpuHandle());

	//�萔�o�b�t�@�r���[�̃Z�b�g(�}�e���A��)
	ID3D12Resource* constBuff = material_->GetConstantBuffer();
	const int32_t constBuffMaterialNum = 1;
	cmdList->SetGraphicsRootConstantBufferView(constBuffMaterialNum, constBuff->GetGPUVirtualAddress());
	// �`��R�}���h
	const UINT instanceCount = 1;
	cmdList->DrawIndexedInstanced((UINT)indices_.size(), instanceCount, 0, 0, 0);

}
