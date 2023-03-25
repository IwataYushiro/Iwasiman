#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>

class Particle
{
private://�G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//OBJ�t�@�C������p�[�e�B�N�����f����ǂݍ���
	static Particle* LoadFromParticleObj(const std::string& modelName);
	// �f�X�N���v�^�q�[�v�̏�����
	void InitializeDescriptorHeap();
	// �e�N�X�`���ǂݍ���
	void LoadTexture(const std::string& directoryPath, const std::string& filename);

	//�e��o�b�t�@����
	void CreateBuffers();

	// �`��
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndexMaterial);


private:

};

Particle::Particle()
{
}

Particle::~Particle()
{
}