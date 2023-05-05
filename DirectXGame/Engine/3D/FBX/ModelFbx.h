#pragma once
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <string>
#include <vector>

struct Node
{
	//���O
	std::string name;
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
	//�e�m�[�h
	Node* parent = nullptr;
};

class ModelFbx
{
private://�G�C���A�X

public://�t�����h�A�T�u�N���X
	//�t�����h�N���X
	friend class FbxLoader;
	//���_�f�[�^�\����
	struct VertexPosNormalUv
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
	};


public://�����o�֐�

private://�����o�ϐ�
	//���f����
	std::string name;
	//�m�[�h�z��
	std::vector<Node> nodes;
	//���b�V�������m�[�h
	Node* meshNode = nullptr;
	//���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;
	//���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;
	//�A���r�G���g�W��
	DirectX::XMFLOAT3 ambient = { 1.0f,1.0f,1.0f };
	//�f�B�t���[�Y�W��
	DirectX::XMFLOAT3 diffuse = { 1.0f,1.0f,1.0f };
	//�e�N�X�`�����^�f�[�^
	DirectX::TexMetadata metadata = {};
	//�X�N���b�`�C���[�W
	DirectX::ScratchImage scratchImg = {};

};
