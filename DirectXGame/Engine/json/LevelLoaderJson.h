#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>

/*

*	LevelLoaderJson.h

*	json�t�@�C���ɂ�郌�x���f�[�^�\����

*/
struct LevelData
{
	//�I�u�W�F�N�g���
	struct ObjectData
	{
		//���s�ړ�
		DirectX::XMVECTOR trans;
		//��]�p
		DirectX::XMVECTOR rot;
		//�X�P�[��
		DirectX::XMVECTOR scale;

		//�R���C�_�[
		//���S�_
		DirectX::XMVECTOR centerCollider;
		//�T�C�Y
		DirectX::XMVECTOR sizeCollider;
		
		//�t�@�C����
		std::string fileName;
		//�I�u�W�F�N�g�^�C�v
		std::string objectType;
		//�I�u�W�F�N�g�p�^�[��
		std::string objectPattern;
	};
	//�I�u�W�F�N�g�z��
	std::vector<ObjectData> objects;
};

//Json���x�����[�_�[
class LevelLoader
{
public:
	//�f�t�H���g�̓ǂݍ��݃f�B���N�g��
	static const std::string defaultBaseDirectory;
	//�t�@�C���g���q(json)
	static const std::string extension;

public:
	// ���x���f�[�^�t�@�C���̓ǂݍ���
	static LevelData* LoadFile(const std::string& fileName);

};
