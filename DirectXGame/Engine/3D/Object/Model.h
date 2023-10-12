#pragma once
#include "Mesh.h"
#include <DirectXMath.h>
#include <unordered_map>

//3D���f��(.obj)
class Model
{
private://�G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://�R���X�g���N�^��
	// �f�X�g���N�^
	~Model();

public://�ÓI�����o�֐�
	//�ÓI������
	static void StaticInitialize(ID3D12Device* device);
	//OBJ�t�@�C������3D���f����ǂݍ���
	static Model* LoadFromOBJ(const std::string& modelName, bool smoothing = false);
	
public://�����o�֐�
	
	// �f�X�N���v�^�q�[�v�̏�����
	void InitializeDescriptorHeap();

	// �}�e���A���ǂݍ���
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	//�}�e���A���o�^
	void AddMaterial(Material* material) { materials.emplace(material->name, material); }
	// �e�N�X�`���ǂݍ���
	void LoadTextures();	

	// �`��
	void Draw(ID3D12GraphicsCommandList* cmdList);

private://�ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device_;
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;
	//�x�[�X�f�B���N�g��
	static const std::string baseDirectory;
	
private:
	//���O
	std::string name;
	//���b�V���R���e�i
	std::vector<Mesh*> meshes;
	//�}�e���A���R���e�i
	std::unordered_map<std::string, Material*> materials;
	//�f�t�H���g�}�e���A��
	Material* defaultMaterial = nullptr;
	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap;
	
	
public://�A�N�Z�b�T�u����
	//�f�o�C�X
	static void SetDevice(ID3D12Device* device) { Model::device_ = device; }
	//���b�V���R���e�i�擾
	inline const std::vector<Mesh*>& GetMeshes() { return meshes; }

private://�����o�֐�(�J�v�Z����)
	//OBJ�t�@�C������3D���f����ǂݍ���(����J)
	void LoadFromOBJInternal(const std::string& modelName, bool smoothing = false);
	
};

