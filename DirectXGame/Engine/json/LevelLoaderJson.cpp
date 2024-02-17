#include "LevelLoaderJson.h"
#include "XYZ.h"

#include <json.hpp>
#include <fstream>
#include <cassert>

using namespace IwasiEngine;
/*

*	LevelLoaderJson.cpp

*	jsonファイルによるレベルデータ構造体

*/

//静的メンバ変数の実体
const std::string LevelLoader::DEFAULT_BASE_DIRECTORY = "Resources/json/levels/";
const std::string LevelLoader::EXTENSION = ".json";

std::unique_ptr<LevelData> LevelLoader::LoadFile(const std::string& fileName)
{
	//連結してフルパスを得る
	const std::string fullpath = DEFAULT_BASE_DIRECTORY + fileName + EXTENSION;

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullpath);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	//json文字列から解凍したデータ
	nlohmann::json deserialized;
	//解凍
	file >> deserialized;

	// 正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	// 正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	//レベルデータ格納用インスタンス
	std::unique_ptr<LevelData> lvData = std::make_unique<LevelData>();
	// "objects"の全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"])
	{
		//typeが無ければ停止
		assert(object.contains("type"));

		// 種別を取得
		std::string type = object["type"].get<std::string>();

		// type=MESH
		if (type.compare("MESH")==0)
		{
			lvData->objects.emplace_back(LevelData::ObjectData{});
			// 今追加した要素の参照を得る
			LevelData::ObjectData& objectData = lvData->objects.back();

			if (object.contains("file_name")) {
				// ファイル名
				objectData.fileName = object["file_name"];
			}
			if (object.contains("objecttype")) {
				// タイプ名
				objectData.objectType = object["objecttype"];
				
				if (object.contains("objectpattern")) {
				// パターン名
				objectData.objectPattern = object["objectpattern"];
			}
			}
			
			// トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			// 平行移動
			const float transW = 1.0f;
			objectData.trans.m128_f32[XYZW_X] = static_cast<float>(transform["translation"][XYZ_Y]);
			objectData.trans.m128_f32[XYZW_Y] = static_cast<float>(transform["translation"][XYZ_Z]);
			objectData.trans.m128_f32[XYZW_Z] = -static_cast<float>(transform["translation"][XYZ_X]);
			objectData.trans.m128_f32[XYZW_W] = transW;
			// 回転角
			const float rotW = 0.0f;
			objectData.rot.m128_f32[XYZW_X] = -static_cast<float>(transform["rotation"][XYZ_Y]);
			objectData.rot.m128_f32[XYZW_Y] = -static_cast<float>(transform["rotation"][XYZ_Z]);
			objectData.rot.m128_f32[XYZW_Z] = static_cast<float>(transform["rotation"][XYZ_X]);
			objectData.rot.m128_f32[XYZW_W] = rotW;
			// スケーリング
			const float scaleW = 0.0f;
			objectData.scale.m128_f32[XYZW_X] = static_cast<float>(transform["scaling"][XYZ_Y]);
			objectData.scale.m128_f32[XYZW_Y] = static_cast<float>(transform["scaling"][XYZ_Z]);
			objectData.scale.m128_f32[XYZW_Z] = static_cast<float>(transform["scaling"][XYZ_X]);
			objectData.scale.m128_f32[XYZW_W] = scaleW;

			// コライダーのパラメータ読み込み
			nlohmann::json& collider = object["collider"];
			if (type.compare("BOX") == 0)
			{
				// 平行移動
				const float centerColliderW = 1.0f;
				objectData.centerCollider.m128_f32[XYZW_X] = static_cast<float>(collider["center"][XYZ_Y]);
				objectData.centerCollider.m128_f32[XYZW_Y] = static_cast<float>(collider["center"][XYZ_Z]);
				objectData.centerCollider.m128_f32[XYZW_Z] = -static_cast<float>(collider["center"][XYZ_X]);
				objectData.centerCollider.m128_f32[XYZW_W] = centerColliderW;
				// 回転角
				const float sizeColliderW = 0.0f;
				objectData.sizeCollider.m128_f32[XYZW_X] = -static_cast<float>(collider["size"][XYZ_Y]);
				objectData.sizeCollider.m128_f32[XYZW_Y] = -static_cast<float>(collider["size"][XYZ_Z]);
				objectData.sizeCollider.m128_f32[XYZW_Z] = static_cast<float>(collider["size"][XYZ_X]);
				objectData.sizeCollider.m128_f32[XYZW_W] = sizeColliderW;
			}
		}
		//オブジェクト走査を再帰関数で走査(一旦後で)
		if (object.contains("children")) {

		}
	}

	return lvData;
}
