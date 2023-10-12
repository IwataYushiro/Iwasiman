#include "LightGroup.h"
#include <cassert>

using namespace DirectX;

//���C�g�O���[�v

//�ÓI�����o�ϐ��̎���
ID3D12Device* LightGroup::device_ = nullptr;

void LightGroup::StaticInitialize(ID3D12Device* device)
{
	//�ď������`�F�b�N
	assert(!LightGroup::device_);
	//nullptr�`�F�b�N
	assert(device);

	LightGroup::device_ = device;
}

LightGroup* LightGroup::Create()
{
	//3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	LightGroup* instance = new LightGroup();
	//������
	instance->Initialize();
	//���������C���X�^���X��Ԃ�
	return instance;
}

void LightGroup::Initialize()
{
	HRESULT result;

	//�W���̃��C�g�ݒ�
	DefaultLightSetting();
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};		//�q�[�v�ݒ�
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourseDesc{};
	cbResourseDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourseDesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff;//256�o�C�g�A���C�������g
	cbResourseDesc.Height = 1;
	cbResourseDesc.DepthOrArraySize = 1;
	cbResourseDesc.MipLevels = 1;
	cbResourseDesc.SampleDesc.Count = 1;
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

void LightGroup::Update()
{
	//�l�̍X�V���������������萔�o�b�t�@�ɓ]��
	if (dirty_)
	{
		TransferConstBuffer();
		dirty_ = false;
	}
}

void LightGroup::Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex)
{
	//�萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(rootParameterIndex, constBuff_->GetGPUVirtualAddress());
}

void LightGroup::TransferConstBuffer()
{
	HRESULT result;
	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferData* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		//����
		constMap->ambientColor = ambientColor_;
		//���s����
		for (int i = 0; i < DirLightNum; i++)
		{
			//light���L���Ȃ�ݒ��]��
			if (dirLights_[i].IsActive())
			{
				constMap->dirLights[i].active = 1;
				constMap->dirLights[i].lightV = -dirLights_[i].GetLightDir();
				constMap->dirLights[i].lightColor = dirLights_[i].GetLightColor();
			}
			//�����Ȃ�]�����Ȃ�
			else
			{
				constMap->dirLights[i].active = 0;
			}
		}
		//�_����
		for (int i = 0; i < PointLightNum; i++)
		{
			//light���L���Ȃ�ݒ��]��
			if (pointLights_[i].IsActive())
			{
				constMap->pointLights[i].active = 1;
				constMap->pointLights[i].lightPos = pointLights_[i].GetLightPos();
				constMap->pointLights[i].lightColor = pointLights_[i].GetLightColor();
				constMap->pointLights[i].lightatten = pointLights_[i].GetLightAtten();
			}
			//�����Ȃ�]�����Ȃ�
			else
			{
				constMap->pointLights[i].active = 0;
			}
		}
		constBuff_->Unmap(0, nullptr);
	}
}

void LightGroup::DefaultLightSetting()
{
	dirLights_[0].SetActive(true);
	dirLights_[0].SetLightColor({ 1.0f,1.0f,1.0f });
	dirLights_[0].SetLightDir({ 0.0f,-1.0f,0.0f,0.0f });

	dirLights_[1].SetActive(true);
	dirLights_[1].SetLightColor({ 1.0f,1.0f,1.0f });
	dirLights_[1].SetLightDir({ 0.5f,0.1f,0.2f,0.0f });

	dirLights_[2].SetActive(true);
	dirLights_[2].SetLightColor({ 1.0f,1.0f,1.0f });
	dirLights_[2].SetLightDir({ -0.5f,0.1f,-0.2f,0.0f });

	pointLights_[0].SetActive(false);
	pointLights_[0].SetLightColor({ 1.0f,1.0f,1.0f });
	pointLights_[0].SetLightPos({ 0.0f,0.0f,0.0f });
	pointLights_[0].SetLightAtten({ 1.0f,1.0f,1.0f });
}

void LightGroup::SetAmbientColor(const XMFLOAT3& color)
{
	ambientColor_ = color;
	dirty_ = true;
}

void LightGroup::SetDirLightActive(int index, bool active)
{
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetActive(active);
}

void LightGroup::SetDirLightDir(int index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetLightDir(lightdir);
	dirty_ = true;
}

void LightGroup::SetDirLightColor(int index, const XMFLOAT3& lightcolor)
{
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetLightColor(lightcolor);
	dirty_ = true;
}

void LightGroup::SetPointLightPos(int index, const XMFLOAT3& lightPos)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetLightPos(lightPos);
	dirty_ = true;
}

void LightGroup::SetPointLightColor(int index, const XMFLOAT3& lightColor)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetLightColor(lightColor);
	dirty_ = true;

}

void LightGroup::SetPointLightAtten(int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetLightAtten(lightAtten);
	dirty_ = true;
}

void LightGroup::SetPointLightActive(int index, bool active)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetActive(active);
}
