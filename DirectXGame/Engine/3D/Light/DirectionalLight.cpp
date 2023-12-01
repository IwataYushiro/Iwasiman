#include "DirectionalLight.h"
#include <cassert>

using namespace DirectX;

/*

*	DirectionalLight.cpp

*	���C�g

*/

//�ÓI�����o�ϐ��̎���
ID3D12Device* DirectionalLight::device_ = nullptr;

void DirectionalLight::StaticInitialize(ID3D12Device* device)
{
	//�ď������`�F�b�N
	assert(!DirectionalLight::device_);
	//nullptr�`�F�b�N
	assert(device);
	//�ÓI�����o�ϐ��̃Z�b�g
	DirectionalLight::device_ = device;
}

DirectionalLight* DirectionalLight::Create()
{
	//3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	DirectionalLight* instance = new DirectionalLight();
	//������
	instance->Initialize();
	//���������C���X�^���X��Ԃ�
	return instance;
}

void DirectionalLight::Initialize()
{

	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//�q�[�v�ݒ�
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
	//���\�[�X�ݒ�
	//���\�[�X�f�X�N�̃v���Z�b�g
	struct ResDescPreset
	{
		const UINT64 width = 0xff;
		const UINT height = 1;
		const UINT16 arraysize = 1;
		const UINT16 mipLevels = 1;
		const UINT sampleCount = 1;
		
	};
	ResDescPreset resDescPreset;

	D3D12_RESOURCE_DESC cbResourseDesc{};
	cbResourseDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourseDesc.Width = (sizeof(ConstBufferData) + resDescPreset.width) & ~resDescPreset.width;//256�o�C�g�A���C�������g
	cbResourseDesc.Height = resDescPreset.height;
	cbResourseDesc.DepthOrArraySize = resDescPreset.arraysize;
	cbResourseDesc.MipLevels = resDescPreset.mipLevels;
	cbResourseDesc.SampleDesc.Count = resDescPreset.sampleCount;
	cbResourseDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�萔�o�b�t�@�̐���
	result = device_->CreateCommittedResource(
		&cbHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourseDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�փf�[�^�]��
	TransferConstBuffer();
}

void DirectionalLight::Update()
{
	//�l�̍X�V���������������萔�o�b�t�@�ɓ]������
	if (dirty_)
	{
		TransferConstBuffer();
		dirty_ = false;
	}
}

void DirectionalLight::Draw(ID3D12GraphicsCommandList* cmdList, const UINT rootParameterIndex)
{
	//�萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(rootParameterIndex, constBuff_->GetGPUVirtualAddress());
}

void DirectionalLight::TransferConstBuffer()
{
	HRESULT result;
	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		constMap->lightV = -lightDir_;
		constMap->lightColor = lightColor_;
		constBuff_->Unmap(0, nullptr);
	}

}

void DirectionalLight::SetLightDir(const XMVECTOR& lightdir)
{
	//���K�����ăZ�b�g
	this->lightDir_ = XMVector3Normalize(lightdir);
	dirty_ = true;
}

void DirectionalLight::SetLightColor(const XMFLOAT3& lightcolor)
{
	//���K�����ăZ�b�g
	this->lightColor_ = lightcolor;
	dirty_ = true;
}
