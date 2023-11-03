#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <fbxsdk.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <wrl.h>


/*

*	ModelFbx.h

*	�m�[�h�\����

*/
struct Node
{
	
	//���[�J���X�P�[��
	DirectX::XMVECTOR scaling = { 1.0f,1.0f,1.0f,0.0f };
	//���[�J����]�p
	DirectX::XMVECTOR rotation = { 0.0f,0.0f,0.0f,0.0f };
	//���[�J�����W
	DirectX::XMVECTOR translation = { 0.0f,0.0f,0.0f,0.0f };
	//���[�J���ό`�s��
	DirectX::XMMATRIX transform;
	//�O���[�o���ό`�s��
	DirectX::XMMATRIX globalTransform;
	
	//���O
	std::string name;
	//�e�m�[�h
	Node* parent = nullptr;
};

/*

*	ModelFbx.h

*	FBX���f��

*/
class ModelFbx
{
private://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;
	//std::���ȗ�
	using string = std::string;
	template <class T>using vector = std::vector<T>;

public://�萔
	//�{�[���C���f�b�N�X�̍ő吔
	static const int32_t MAX_BONE_INDICES = 4;

public://�t�����h�A�T�u�N���X
	//�t�����h�N���X
	friend class FbxLoader;
	//���_�f�[�^�\����
	struct VertexPosNormalUvSkin
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
		UINT boneIndex[MAX_BONE_INDICES];	//�{�[���ԍ�
		float boneWeight[MAX_BONE_INDICES];	//�X�L���E�F�C�g
	};
	//�{�[���\����
	struct Bone
	{
		//���O
		std::string name;
		//�����p���̋t�s��
		DirectX::XMMATRIX invInitialPose;
		//�N���X�^�[
		FbxCluster* fbxCluster;
		//�R���X�g���N�^(���O)
		Bone(const std::string& name) {
			this->name = name;
		}
	};

public://�����o�֐�
	//�f�X�g���N�^
	~ModelFbx();
	//�`��(�R�}���h���X�g)
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//�o�b�t�@����(�f�o�C�X)
	void CreateBuffers(ID3D12Device* device);
	//�f�o�C�X
	static void SetDevice(ID3D12Device* device) { ModelFbx::device_ = device; }

private://�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device_;
	//���f����
	string name_;
	//�m�[�h�z��
	vector<Node> nodes_;
	//���b�V�������m�[�h
	Node* meshNode_ = nullptr;
	//�{�[���z��
	std::vector<Bone> bones_;
	 //FBX�V�[��
	FbxScene* fbxScene_ = nullptr;
	//���_�f�[�^�z��
	vector<VertexPosNormalUvSkin> vertices_;
	//���_�C���f�b�N�X�z��
	vector<unsigned short> indices_;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff_;
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff_;
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff_;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_;
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView_;
	// SRV�p�̃f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV_;
	
	//�A���r�G���g�W��
	const XMFLOAT3 presetAmbient_ = { 1.0f,1.0f,1.0f };//�v���Z�b�g
	XMFLOAT3 ambient_ = presetAmbient_;
	//�f�B�t���[�Y�W��
	const XMFLOAT3 presetDiffuse_ = { 1.0f,1.0f,1.0f };//�v���Z�b�g
	XMFLOAT3 diffuse_ = presetDiffuse_;
	//�e�N�X�`�����^�f�[�^
	TexMetadata metadata_ = {};
	//�X�N���b�`�C���[�W
	ScratchImage scratchImg_ = {};

public://�A�N�Z�b�T�u����
	//���f���ό`�s��
	const XMMATRIX& GetModelTransform() { return meshNode_->globalTransform; }
	//�S�{�[��
	std::vector<Bone>& GetBones() { return bones_; }
	//FBX�V�[��
	FbxScene* GetFbxScene() { return fbxScene_; }
};
