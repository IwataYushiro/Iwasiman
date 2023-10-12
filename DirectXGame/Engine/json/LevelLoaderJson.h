#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>

/*

*	LevelLoaderJson.h

*	jsonファイルによるレベルデータ構造体

*/
struct LevelData
{
	//オブジェクト情報
	struct ObjectData
	{
		//平行移動
		DirectX::XMVECTOR trans;
		//回転角
		DirectX::XMVECTOR rot;
		//スケール
		DirectX::XMVECTOR scale;

		//コライダー
		//中心点
		DirectX::XMVECTOR centerCollider;
		//サイズ
		DirectX::XMVECTOR sizeCollider;
		
		//ファイル名
		std::string fileName;
		//オブジェクトタイプ
		std::string objectType;
		//オブジェクトパターン
		std::string objectPattern;
	};
	//オブジェクト配列
	std::vector<ObjectData> objects;
};

//Jsonレベルローダー
class LevelLoader
{
public:
	//デフォルトの読み込みディレクトリ
	static const std::string defaultBaseDirectory;
	//ファイル拡張子(json)
	static const std::string extension;

public:
	// レベルデータファイルの読み込み
	static LevelData* LoadFile(const std::string& fileName);

};
