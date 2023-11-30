#include "Material.h"
#include <DirectXTex.h>
#include <cassert>

using namespace DirectX;
using namespace std;

/*

*	Material.cpp

*	�}�e���A���Ǘ�

*/

// �ÓI�����o�ϐ��̎���
ID3D12Device* Material::device_ = nullptr;

Material* Material::Create()
{
	Material* mat = new Material;

	mat->Initialize();

	return mat;
}

void Material::StaticInitialize(ID3D12Device* device)
{
	Material::device_ = device;
}

void Material::LoadTexture(const std::string& directoryPath, const D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
	const D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
	HRESULT result = S_FALSE;

	
	// �e�N�X�`���Ȃ�
	if (textureFilename.size() == 0) {
		textureFilename = "white1x1.png";
	}
	// �V�F�[�_���\�[�X�r���[�쐬
	cpuDescHandleSRV_ = cpuHandle;
	gpuDescHandleSRV_ = gpuHandle;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	//�t�@�C���p�X������
	string filepath = directoryPath + textureFilename;

	//���j�R�[�h������ɕϊ�����
	const int maxPathNum = 128;
	wchar_t wfilePath[maxPathNum];
	const int cbMultiByte = -1;
	MultiByteToWideChar(CP_ACP, 0,
		filepath.c_str(), cbMultiByte, wfilePath, _countof(wfilePath));
	// WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(wfilePath, WIC_FLAGS_NONE, &metadata, scratchImg);
	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	// �~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);

	// �q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC texresDesc{};
	texresDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texresDesc.Format = metadata.format;
	texresDesc.Width = metadata.width;
	texresDesc.Height = (UINT)metadata.height;
	texresDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	texresDesc.MipLevels = (UINT16)metadata.mipLevels;
	const UINT sampleCountNum = 1;
	texresDesc.SampleDesc.Count = sampleCountNum;
	
	// �e�N�X�`���p�o�b�t�@�̐���
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // �e�N�X�`���p�w��
		nullptr, IID_PPV_ARGS(&texBuff_));
	assert(SUCCEEDED(result));

	// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0); // ���f�[�^���o
		result = texBuff_->WriteToSubresource(
			(UINT)i,
			nullptr,              // �S�̈�փR�s�[
			img->pixels,          // ���f�[�^�A�h���X
			(UINT)img->rowPitch,  // 1���C���T�C�Y
			(UINT)img->slicePitch // 1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // �ݒ�\����
	D3D12_RESOURCE_DESC resDesc = texBuff_->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	const UINT tex2DMipLevelsNum = 1;
	srvDesc.Texture2D.MipLevels = tex2DMipLevelsNum;

	device_->CreateShaderResourceView(texBuff_.Get(), //�r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc, //�e�N�X�`���ݒ���
		cpuDescHandleSRV_
	);
}

void Material::Update()
{
	//�萔�o�b�t�@�֓]��
	constMap_->ambient = ambient;
	constMap_->diffuse = diffuse;
	constMap_->specular = specular;
	constMap_->alpha = alpha;
}

void Material::Initialize()
{
	//�萔�o�b�t�@����
	CreateConstBuffer();
}

void Material::CreateConstBuffer()
{
	HRESULT result;
	//�萔�o�b�t�@(�}�e���A��)
	//�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

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
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = (sizeof(ConstBufferDataB1) + resDescPreset.width) & ~resDescPreset.width;
	resourceDesc.Height = resDescPreset.height;
	resourceDesc.DepthOrArraySize = resDescPreset.arraysize;
	resourceDesc.MipLevels = resDescPreset.mipLevels;
	resourceDesc.SampleDesc.Count = resDescPreset.sampleCount;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�萔�o�b�t�@����
	result = device_->CreateCommittedResource(
		&heapProps, // �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	// �萔�o�b�t�@�փf�[�^�]��
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));
}
