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
	static const int MAX_BONE_INDICES = 4;

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
		//�R���X�g���N�^
		Bone(const std::string& name) {
			this->name = name;
		}
	};

public://�����o�֐�
	//�f�X�g���N�^
	~ModelFbx();
	//�`��
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//�o�b�t�@����
	void CreateBuffers(ID3D12Device* device);
	//�f�o�C�X
	static void SetDevice(ID3D12Device* device) { ModelFbx::device_ = device; }

private://�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device_;
	//���f����
	string name;
	//�m�[�h�z��
	vector<Node> nodes;
	//���b�V�������m�[�h
	Node* meshNode = nullptr;
	//�{�[���z��
	std::vector<Bone> bones;
	 //FBX�V�[��
	FbxScene* fbxScene = nullptr;
	//���_�f�[�^�z��
	vector<VertexPosNormalUvSkin> vertices;
	//���_�C���f�b�N�X�z��
	vector<unsigned short> indices;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView;
	// SRV�p�̃f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	
	//�A���r�G���g�W��
	XMFLOAT3 ambient = { 1.0f,1.0f,1.0f };
	//�f�B�t���[�Y�W��
	XMFLOAT3 diffuse = { 1.0f,1.0f,1.0f };
	//�e�N�X�`�����^�f�[�^
	TexMetadata metadata = {};
	//�X�N���b�`�C���[�W
	ScratchImage scratchImg = {};

public://�A�N�Z�b�T�u����
	//���f���ό`�s��
	const XMMATRIX& GetModelTransform() { return meshNode->globalTransform; }
	//�S�{�[��
	std::vector<Bone>& GetBones() { return bones; }
	//FBX�V�[��
	FbxScene* GetFbxScene() { return fbxScene; }
};
