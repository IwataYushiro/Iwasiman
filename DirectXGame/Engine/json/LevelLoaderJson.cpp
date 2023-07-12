#include "LevelLoaderJson.h"

#include <json.hpp>
#include <fstream>
#include <cassert>

const std::string LevelLoader::defaultBaseDirectory = "Resources/json/levels/";
const std::string LevelLoader::extension = ".json";

LevelData* LevelLoader::LoadFile(const std::string& fileName)
{
	//連結してフルパスを得る
	const std::string fullpath = defaultBaseDirectory + fileName + extension;

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
	LevelData* lvData = new LevelData();
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

			// トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			// 平行移動
			objectData.trans.m128_f32[0] = (float)transform["translation"][1];
			objectData.trans.m128_f32[1] = (float)transform["translation"][2];
			objectData.trans.m128_f32[2] = -(float)transform["translation"][0];
			objectData.trans.m128_f32[3] = 1.0f;
			// 回転角
			objectData.rot.m128_f32[0] = -(float)transform["rotation"][1];
			objectData.rot.m128_f32[1] = -(float)transform["rotation"][2];
			objectData.rot.m128_f32[2] = (float)transform["rotation"][0];
			objectData.rot.m128_f32[3] = 0.0f;
			// スケーリング
			objectData.scale.m128_f32[0] = (float)transform["scaling"][1];
			objectData.scale.m128_f32[1] = (float)transform["scaling"][2];
			objectData.scale.m128_f32[2] = (float)transform["scaling"][0];
			objectData.scale.m128_f32[3] = 0.0f;

			// コライダーのパラメータ読み込み
			nlohmann::json& collider = object["collider"];
			if (type.compare("BOX") == 0)
			{
				// 平行移動
				objectData.centerCollider.m128_f32[0] = (float)collider["center"][1];
				objectData.centerCollider.m128_f32[1] = (float)collider["center"][2];
				objectData.centerCollider.m128_f32[2] = -(float)collider["center"][0];
				objectData.centerCollider.m128_f32[3] = 1.0f;
				// 回転角
				objectData.sizeCollider.m128_f32[0] = -(float)collider["size"][1];
				objectData.sizeCollider.m128_f32[1] = -(float)collider["size"][2];
				objectData.sizeCollider.m128_f32[2] = (float)collider["size"][0];
				objectData.sizeCollider.m128_f32[3] = 0.0f;
			}
		}
		//オブジェクト走査を再帰関数で走査(一旦後で)
		if (object.contains("children")) {

		}
	}

	return lvData;
}
