#include "LightGroup.h"
#include <cassert>

using namespace DirectX;

/*

*	LightGroup.cpp

*	���C�g�O���[�v

*/

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
		enum LightActiveNum
		{
			LAN_False=0,
			LAN_True=1,
		};
		//����
		constMap->ambientColor = ambientColor_;
		//���s����
		for (int i = 0; i < DirLightNum; i++)
		{
			//light���L���Ȃ�ݒ��]��
			if (dirLights_[i].IsActive())
			{
				constMap->dirLights[i].active = LAN_True;
				constMap->dirLights[i].lightV = -dirLights_[i].GetLightDir();
				constMap->dirLights[i].lightColor = dirLights_[i].GetLightColor();
			}
			//�����Ȃ�]�����Ȃ�
			else
			{
				constMap->dirLights[i].active = LAN_False;
			}
		}
		//�_����
		for (int i = 0; i < PointLightNum; i++)
		{
			//light���L���Ȃ�ݒ��]��
			if (pointLights_[i].IsActive())
			{
				constMap->pointLights[i].active = LAN_True;
				constMap->pointLights[i].lightPos = pointLights_[i].GetLightPos();
				constMap->pointLights[i].lightColor = pointLights_[i].GetLightColor();
				constMap->pointLights[i].lightatten = pointLights_[i].GetLightAtten();
			}
			//�����Ȃ�]�����Ȃ�
			else
			{
				constMap->pointLights[i].active = LAN_False;
			}
		}
		constBuff_->Unmap(0, nullptr);
	}
}

void LightGroup::DefaultLightSetting()
{
	//���C�g�ԍ�
	enum LightNum
	{
		LN_Light0=0, 
		LN_Light1 = 1,
		LN_Light2 = 2,
	};
	//�_�����ԍ�
	enum PointLightNum
	{
		PLN_Light0=0,
	};
	//�f�t�H���g�v���Z�b�g
	struct DefaultLightPreset
	{
		//0��
		const XMVECTOR light0Dir = { 0.0f,-1.0f,0.0f,0.0f };
		const XMFLOAT3 light0Color = { 1.0f,1.0f,1.0f };
		const float pad0 = 0.0f;
		//1��
		const XMVECTOR light1Dir = { 0.5f,0.1f,0.2f,0.0f };
		const XMFLOAT3 light1Color = { 1.0f,1.0f,1.0f };
		const float pad1 = 0.0f;
		//2��
		const XMVECTOR light2Dir = { -0.5f,0.1f,-0.2f,0.0f };
		const XMFLOAT3 light2Color = { 1.0f,1.0f,1.0f };
		const float pad2 = 0.0f;

		//�_����
		//0��
		const XMFLOAT3 pointLight0Color = { 1.0f,1.0f,1.0f };
		const float padp0 = 0.0f;
		const XMFLOAT3 pointLight0Pos = { 0.0f,0.0f,0.0f };
		const float padp1 = 0.0f;
		const XMFLOAT3 pointLight0Atten = { 1.0f,1.0f,1.0f };
		const float padp2 = 0.0f;
	};
	DefaultLightPreset preset;

	dirLights_[LN_Light0].SetActive(true);
	dirLights_[LN_Light0].SetLightColor(preset.light0Color);
	dirLights_[LN_Light0].SetLightDir(preset.light0Dir);

	dirLights_[LN_Light1].SetActive(true);
	dirLights_[LN_Light1].SetLightColor(preset.light1Color);
	dirLights_[LN_Light1].SetLightDir(preset.light1Dir);

	dirLights_[LN_Light2].SetActive(true);
	dirLights_[LN_Light2].SetLightColor(preset.light2Color);
	dirLights_[LN_Light2].SetLightDir(preset.light2Dir);

	pointLights_[PLN_Light0].SetActive(false);
	pointLights_[PLN_Light0].SetLightColor(preset.pointLight0Color);
	pointLights_[PLN_Light0].SetLightPos(preset.pointLight0Pos);
	pointLights_[PLN_Light0].SetLightAtten(preset.pointLight0Atten);
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
