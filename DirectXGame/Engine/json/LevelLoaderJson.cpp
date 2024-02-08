#include "LevelLoaderJson.h"
#include "XYZ.h"

#include <json.hpp>
#include <fstream>
#include <cassert>

using namespace IwasiEngine;
/*

*	LevelLoaderJson.cpp

*	json�t�@�C���ɂ�郌�x���f�[�^�\����

*/

//�ÓI�����o�ϐ��̎���
const std::string LevelLoader::DEFAULT_BASE_DIRECTORY = "Resources/json/levels/";
const std::string LevelLoader::EXTENSION = ".json";

std::unique_ptr<LevelData> LevelLoader::LoadFile(const std::string& fileName)
{
	//�A�����ăt���p�X�𓾂�
	const std::string fullpath = DEFAULT_BASE_DIRECTORY + fileName + EXTENSION;

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
	std::unique_ptr<LevelData> lvData = std::make_unique<LevelData>();
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
			if (object.contains("objecttype")) {
				// �^�C�v��
				objectData.objectType = object["objecttype"];
				
				if (object.contains("objectpattern")) {
				// �p�^�[����
				objectData.objectPattern = object["objectpattern"];
			}
			}
			
			// �g�����X�t�H�[���̃p�����[�^�ǂݍ���
			nlohmann::json& transform = object["transform"];
			// ���s�ړ�
			const float transW = 1.0f;
			objectData.trans.m128_f32[XYZW_X] = static_cast<float>(transform["translation"][XYZ_Y]);
			objectData.trans.m128_f32[XYZW_Y] = static_cast<float>(transform["translation"][XYZ_Z]);
			objectData.trans.m128_f32[XYZW_Z] = -static_cast<float>(transform["translation"][XYZ_X]);
			objectData.trans.m128_f32[XYZW_W] = transW;
			// ��]�p
			const float rotW = 0.0f;
			objectData.rot.m128_f32[XYZW_X] = -static_cast<float>(transform["rotation"][XYZ_Y]);
			objectData.rot.m128_f32[XYZW_Y] = -static_cast<float>(transform["rotation"][XYZ_Z]);
			objectData.rot.m128_f32[XYZW_Z] = static_cast<float>(transform["rotation"][XYZ_X]);
			objectData.rot.m128_f32[XYZW_W] = rotW;
			// �X�P�[�����O
			const float scaleW = 0.0f;
			objectData.scale.m128_f32[XYZW_X] = static_cast<float>(transform["scaling"][XYZ_Y]);
			objectData.scale.m128_f32[XYZW_Y] = static_cast<float>(transform["scaling"][XYZ_Z]);
			objectData.scale.m128_f32[XYZW_Z] = static_cast<float>(transform["scaling"][XYZ_X]);
			objectData.scale.m128_f32[XYZW_W] = scaleW;

			// �R���C�_�[�̃p�����[�^�ǂݍ���
			nlohmann::json& collider = object["collider"];
			if (type.compare("BOX") == 0)
			{
				// ���s�ړ�
				const float centerColliderW = 1.0f;
				objectData.centerCollider.m128_f32[XYZW_X] = static_cast<float>(collider["center"][XYZ_Y]);
				objectData.centerCollider.m128_f32[XYZW_Y] = static_cast<float>(collider["center"][XYZ_Z]);
				objectData.centerCollider.m128_f32[XYZW_Z] = -static_cast<float>(collider["center"][XYZ_X]);
				objectData.centerCollider.m128_f32[XYZW_W] = centerColliderW;
				// ��]�p
				const float sizeColliderW = 0.0f;
				objectData.sizeCollider.m128_f32[XYZW_X] = -static_cast<float>(collider["size"][XYZ_Y]);
				objectData.sizeCollider.m128_f32[XYZW_Y] = -static_cast<float>(collider["size"][XYZ_Z]);
				objectData.sizeCollider.m128_f32[XYZW_Z] = static_cast<float>(collider["size"][XYZ_X]);
				objectData.sizeCollider.m128_f32[XYZW_W] = sizeColliderW;
			}
		}
		//�I�u�W�F�N�g�������ċA�֐��ő���(��U���)
		if (object.contains("children")) {

		}
	}

	return lvData;
}
