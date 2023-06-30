#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>

//json�t�@�C���ɂ�郌�x���f�[�^
struct LevelData
{
	struct ObjectData
	{
		//�t�@�C����
		std::string fileName;
		//���s�ړ�
		DirectX::XMVECTOR trans;
		//��]�p
		DirectX::XMVECTOR rot;
		//�X�P�[��
		DirectX::XMVECTOR scale;
	};
	//�I�u�W�F�N�g�z��
	std::vector<ObjectData> objects;
};

//���x�����[�_�[
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
