#include "LevelLoaderJson.h"

#include <json.hpp>
#include <fstream>
#include <cassert>

const std::string LevelLoader::defaultBaseDirectory = "Resources/json/levels/";
const std::string LevelLoader::extension = ".json";

LevelData* LevelLoader::LoadFile(const std::string& fileName)
{
	//�A�����ăt���p�X�𓾂�
	const std::string fullpath = defaultBaseDirectory + fileName + extension;

	// �t�@�C���X�g���[��
	std::ifstream file;

	// �t�@�C�����J��
	file.open(fullpath);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	//json�����񂩂�𓀂����f�[�^
	nlohmann::json deserialized;
	//��
	file >> deserialized;

	// ���������x���f�[�^�t�@�C�����`�F�b�N
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"�𕶎���Ƃ��Ď擾
	std::string name = deserialized["name"].get<std::string>();
	// ���������x���f�[�^�t�@�C�����`�F�b�N
	assert(name.compare("scene") == 0);

	//���x���f�[�^�i�[�p�C���X�^���X
	LevelData* lvData = new LevelData();
	// "objects"�̑S�I�u�W�F�N�g�𑖍�
	for (nlohmann::json& object : deserialized["objects"])
	{
		//type��������Β�~
		assert(object.contains("type"));

		// ��ʂ��擾
		std::string type = object["type"].get<std::string>();

		// type=MESH
		if (type.compare("MESH")==0)
		{
			lvData->objects.emplace_back(LevelData::ObjectData{});
			// ���ǉ������v�f�̎Q�Ƃ𓾂�
			LevelData::ObjectData& objectData = lvData->objects.back();

			if (object.contains("file_name")) {
				// �t�@�C����
				objectData.fileName = object["file_name"];
			}

			// �g�����X�t�H�[���̃p�����[�^�ǂݍ���
			nlohmann::json& transform = object["transform"];
			// ���s�ړ�
			objectData.trans.m128_f32[0] = (float)transform["translation"][1];
			objectData.trans.m128_f32[1] = (float)transform["translation"][2];
			objectData.trans.m128_f32[2] = -(float)transform["translation"][0];
			objectData.trans.m128_f32[3] = 1.0f;
			// ��]�p
			objectData.rot.m128_f32[0] = -(float)transform["rotation"][1];
			objectData.rot.m128_f32[1] = -(float)transform["rotation"][2];
			objectData.rot.m128_f32[2] = (float)transform["rotation"][0];
			objectData.rot.m128_f32[3] = 0.0f;
			// �X�P�[�����O
			objectData.scale.m128_f32[0] = (float)transform["scaling"][1];
			objectData.scale.m128_f32[1] = (float)transform["scaling"][2];
			objectData.scale.m128_f32[2] = (float)transform["scaling"][0];
			objectData.scale.m128_f32[3] = 0.0f;

			// �R���C�_�[�̃p�����[�^�ǂݍ���
			nlohmann::json& collider = object["collider"];
			if (type.compare("BOX") == 0)
			{
				// ���s�ړ�
				objectData.centerCollider.m128_f32[0] = (float)collider["center"][1];
				objectData.centerCollider.m128_f32[1] = (float)collider["center"][2];
				objectData.centerCollider.m128_f32[2] = -(float)collider["center"][0];
				objectData.centerCollider.m128_f32[3] = 1.0f;
				// ��]�p
				objectData.sizeCollider.m128_f32[0] = -(float)collider["size"][1];
				objectData.sizeCollider.m128_f32[1] = -(float)collider["size"][2];
				objectData.sizeCollider.m128_f32[2] = (float)collider["size"][0];
				objectData.sizeCollider.m128_f32[3] = 0.0f;
			}
		}
		//�I�u�W�F�N�g�������ċA�֐��ő���(��U���)
		if (object.contains("children")) {

		}
	}

	return lvData;
}
