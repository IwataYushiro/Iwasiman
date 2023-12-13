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

std::unique_ptr<LightGroup> LightGroup::Create()
{
	//3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	std::unique_ptr<LightGroup> instance = std::make_unique<LightGroup>();
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

void LightGroup::Draw(ID3D12GraphicsCommandList* cmdList,const UINT rootParameterIndex)
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
		//�X�|�b�g���C�g
		for (int i = 0; i < SpotLightNum; i++)
		{
			//light���L���Ȃ�ݒ��]��
			if (spotLights_[i].IsActive())
			{
				constMap->spotLights[i].active = LAN_True;
				constMap->spotLights[i].lightv = spotLights_[i].GetLightDir();
				constMap->spotLights[i].lightPos = spotLights_[i].GetLightPos();
				constMap->spotLights[i].lightColor = spotLights_[i].GetLightColor();
				constMap->spotLights[i].lightatten = spotLights_[i].GetLightAtten();
				constMap->spotLights[i].lightfactoranglecos =
					spotLights_[i].GetLightFactorAngleCos();
			}
			//�����Ȃ�]�����Ȃ�
			else
			{
				constMap->spotLights[i].active = LAN_False;
			}
		}
		//�ۉe
		for (int i = 0; i < CircleShadowNum; i++)
		{
			//light���L���Ȃ�ݒ��]��
			if (circleShadows_[i].IsActive())
			{
				constMap->circleShadows[i].active = LAN_True;
				constMap->circleShadows[i].dir = circleShadows_[i].GetDir();
				constMap->circleShadows[i].casterPos = circleShadows_[i].GetCasterPos();
				constMap->circleShadows[i].distanceCasterLight = circleShadows_[i].GetDistanceCasterLight();
				constMap->circleShadows[i].atten = circleShadows_[i].GetAtten();
				constMap->circleShadows[i].factorAngleCos = circleShadows_[i].GetFactorAngleCos();
			}
			//�����Ȃ�]�����Ȃ�
			else
			{
				constMap->circleShadows[i].active = LAN_False;
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

void LightGroup::SetDirLightActive(const int index, const bool active)
{
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetActive(active);
}

void LightGroup::SetDirLightDir(const int index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetLightDir(lightdir);
	dirty_ = true;
}

void LightGroup::SetDirLightColor(const int index, const XMFLOAT3& lightcolor)
{
	assert(0 <= index && index < DirLightNum);
	dirLights_[index].SetLightColor(lightcolor);
	dirty_ = true;
}

void LightGroup::SetPointLightPos(const int index, const XMFLOAT3& lightPos)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetLightPos(lightPos);
	dirty_ = true;
}

void LightGroup::SetPointLightColor(const int index, const XMFLOAT3& lightColor)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetLightColor(lightColor);
	dirty_ = true;

}

void LightGroup::SetPointLightAtten(const int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetLightAtten(lightAtten);
	dirty_ = true;
}

void LightGroup::SetPointLightActive(const int index,const bool active)
{
	assert(0 <= index && index < PointLightNum);
	pointLights_[index].SetActive(active);
}

void LightGroup::SetSpotLightDir(const int index, const XMVECTOR& lightDir)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights_[index].SetLightDir(lightDir);
	dirty_ = true;
}

void LightGroup::SetSpotLightPos(const int index, const XMFLOAT3& lightPos)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights_[index].SetLightPos(lightPos);
	dirty_ = true;
}

void LightGroup::SetSpotLightColor(const int index, const XMFLOAT3& lightColor)
{
	assert(0 <= index && index < SpotLightNum);
	
	spotLights_[index].SetLightColor(lightColor);
	dirty_ = true;
}

void LightGroup::SetSpotLightAtten(const int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights_[index].SetLightAtten(lightAtten);
	dirty_ = true;
}

void LightGroup::SetSpotLightFactorAngleCos(const int index, const XMFLOAT2& lightFactorAngleCos)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights_[index].SetLightFactorAngleCos(lightFactorAngleCos);
	dirty_ = true;
}

void LightGroup::SetSpotLightActive(const int index, const bool active)
{
	assert(0 <= index && index < SpotLightNum);

	spotLights_[index].SetActive(active);
}

void LightGroup::SetCircleShadowDir(const int index, const XMVECTOR& lightDir)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows_[index].SetDir(lightDir);
	dirty_ = true;
}

void LightGroup::SetCircleShadowCasterPos(const int index, const XMFLOAT3& casterPos)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows_[index].SetCasterPos(casterPos);
	dirty_ = true;
}

void LightGroup::SetCircleShadowDistanceCasterLight(const int index, const float distanceCasterLight)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows_[index].SetDistanceCasterLight(distanceCasterLight);
	dirty_ = true;
}

void LightGroup::SetCircleShadowAtten(const int index, const XMFLOAT3& lightAtten)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows_[index].SetAtten(lightAtten);
	dirty_ = true;
}

void LightGroup::SetCircleShadowFactorAngleCos(const int index, const XMFLOAT2& lightFactorAngleCos)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows_[index].SetFactorAngleCos(lightFactorAngleCos);
	dirty_ = true;
}

void LightGroup::SetCircleShadowActive(const int index, const bool active)
{
	assert(0 <= index && index < CircleShadowNum);

	circleShadows_[index].SetActive(active);
}
