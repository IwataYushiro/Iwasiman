#include "Particle.h"
#include <DirectXTex.h>
#include <cassert>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <Windows.h>

using namespace DirectX;
using namespace std;
using namespace IwasiEngine;

/*

*	Particle.cpp

*	�ꗱ�̃p�[�e�B�N��

*/

//�ÓI�����o�ϐ��̎���
ID3D12Device* Particle::device_ = nullptr;
std::string Particle::defaultTextureDirectoryPath_ = "Resources/particle/";

std::unique_ptr<Particle> Particle::LoadFromParticleTexture(const std::string& fileName)
{
	//�V����Model�^�̃C���X�^���X�̃��������m��
	std::unique_ptr<Particle> particle = std::make_unique<Particle>();

	//�f�X�N���v�^�q�[�v����
	particle->InitializeDescriptorHeap();

	//OBJ�t�@�C������̃f�[�^�ǂݍ���
	particle->LoadTexture(fileName);

	//�ǂݍ��񂾃f�[�^�����Ɋe��o�b�t�@����
	particle->CreateBuffers();

	return particle;
}

void Particle::InitializeDescriptorHeap()
{

	HRESULT result = S_FALSE;

	// �f�X�N���v�^�q�[�v�𐶐�	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
	descHeapDesc.NumDescriptors = 1; // �V�F�[�_�[���\�[�X�r���[1��
	result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap_));//����
	if (FAILED(result)) {
		assert(0);
	}

	// �f�X�N���v�^�T�C�Y���擾
	descriptorHandleIncrementSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}

void Particle::LoadTexture(const std::string& fileName)
{
	HRESULT result;

	//�f�B���N�g���p�X�ƃt�@�C������A�����ăt���p�X�𓾂�
	std::string fullPath = defaultTextureDirectoryPath_ + fileName;

	const int cbMultiByte = -1;
	//���C�h������ɕϊ������ۂ̕�����o�b�t�@�T�C�Y���v�Z
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), cbMultiByte, nullptr, 0);

	//���C�h������ɕϊ�
	std::vector<wchar_t> wfilePath(filePathBufferSize);
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), cbMultiByte, wfilePath.data(), filePathBufferSize);

	//�摜�t�@�C���̗p��
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		wfilePath.data(),	//Resources�t�H���_��texture.png
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	//�~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);

	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;							//��
	textureResourceDesc.Height = (UINT)metadata.height;				//����
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	const UINT sampleDescCount = 1;
	textureResourceDesc.SampleDesc.Count = sampleDescCount;

	//�e�N�X�`���o�b�t�@�̐���
	result = device_->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff_));
	assert(SUCCEEDED(result));

	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texBuff_->WriteToSubresource(
			(UINT)i,
			nullptr,							//�S�̈�փR�s�[
			img->pixels,						//���f�[�^�A�h���X
			(UINT)img->rowPitch,				//1���C���T�C�Y
			(UINT)img->slicePitch				//1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}


	//SRV�q�[�v�̃n���h�����擾
	cpuDescHandleSRV_ = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap_->GetCPUDescriptorHandleForHeapStart(), 0, descriptorHandleIncrementSize_);
	gpuDescHandleSRV_ = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap_->GetGPUDescriptorHandleForHeapStart(), 0, descriptorHandleIncrementSize_);

	//�V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};				//�ݒ�\����
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	//�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	device_->CreateShaderResourceView(texBuff_.Get(), &srvDesc, cpuDescHandleSRV_);

}

void Particle::CreateBuffers()
{
	HRESULT result = S_FALSE;

	std::vector<VertexPosScale> realVertices;

	UINT sizeVB = static_cast<UINT>(sizeof(vertices_));

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// ���_�o�b�t�@����
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosScale* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices_, sizeof(vertices_));
		vertBuff_->Unmap(0, nullptr);
	}

	// ���_�o�b�t�@�r���[�̍쐬
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(vertices_);
	const int32_t startVerticesNum = 0;
	vbView_.StrideInBytes = sizeof(vertices_[startVerticesNum]);

	
}

void Particle::Update()
{
	HRESULT result;
	//�������s�����p�[�e�B�N����S�폜
	particles_.remove_if([](OneParticle& x) {return x.frame >= x.num_frame; });
	//�S�p�[�e�B�N���X�V
	for (std::forward_list<OneParticle>::iterator it = particles_.begin();
		it != particles_.end(); it++)
	{
		//�o�߃t���[�������J�E���g
		it->frame++;
		//���x�ɉ����x�����Z
		it->velocity = it->velocity + it->accel;
		//���x�ɂ��ړ�
		it->position = it->position + it->velocity;
		

		//�i�s�x��0~1�͈̔͂Ɋ��Z
		float f = (float)it->frame / it->num_frame;
		//�X�P�[���̐��`���
		it->scale = (it->e_scale - it->s_scale) * f;
		it->scale += it->s_scale;
		
		//R�F�̐��`���
		it->color.x = (it->e_color.x - it->s_color.x) * f;
		it->color.x += it->s_color.x;

		//G�F�̐��`���
		it->color.y = (it->e_color.y - it->s_color.y) * f;
		it->color.y += it->s_color.y;

		//B�F�̐��`���
		it->color.z = (it->e_color.z - it->s_color.z) * f;
		it->color.z += it->s_color.z;

		//�A���t�@�l�̐��`���
		it->color.w = (it->e_color.w - it->s_color.w) * f;
		it->color.w += it->s_color.w;
	}
	//���_�o�b�t�@�փf�[�^�]��
	VertexPosScale* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		//�p�[�e�B�N���̏���1�����f
		for (std::forward_list<OneParticle>::iterator it = particles_.begin();
			it != particles_.end(); it++)
		{
			//���W
			vertMap->pos = it->position;
			//�X�P�[��
			vertMap->scale = it->scale;
			//�F
			vertMap->color = it->color;
			//���̒��_��
			vertMap++;
		}
		vertBuff_->Unmap(0, nullptr);
	}
	
}

void Particle::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// nullptr�`�F�b�N
	assert(device_);
	assert(cmdList);

	// ���_�o�b�t�@�̐ݒ�
	const UINT viewsNum = 1;
	cmdList->IASetVertexBuffers(0, viewsNum, &vbView_);
	
	// �f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap_.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	
	// �V�F�[�_���\�[�X�r���[���Z�b�g
	cmdList->SetGraphicsRootDescriptorTable(RPI_TexBuff, gpuDescHandleSRV_);
	
	// �`��R�}���h
	const UINT instanceCount = 1;
	cmdList->DrawInstanced((UINT)std::distance(particles_.begin(), particles_.end()), instanceCount, 0, 0);

}

void Particle::Add(const int life, const XMFLOAT3& position, const XMFLOAT3& velocity, const XMFLOAT3& accel,
	const float start_scale, const float end_scale, const XMFLOAT4& start_color, const XMFLOAT4& end_color)
{
	//���X�g�ɗv�f��ǉ�
	particles_.emplace_front();
	//�ǉ������v�f�̎Q��
	OneParticle& p = particles_.front();
	//�l�Z�b�g
	p.position = position;
	p.velocity = velocity;
	p.accel = accel;
	p.num_frame = life;

	p.s_scale = start_scale;
	p.e_scale = end_scale;
	p.scale = p.s_scale;

	p.s_color = start_color;
	p.e_color = end_color;
	p.color = p.s_color;
}

const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
	XMFLOAT3 result;

	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	result.z = lhs.z + rhs.z;
	return result;
}