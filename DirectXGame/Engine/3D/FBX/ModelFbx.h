#pragma once
#include <string>

struct Node
{

};

class ModelFbx
{
private://エイリアス

public://フレンド、サブクラス
	//フレンドクラス
	friend class FbxLoader;


public://メンバ関数

private://メンバ変数
	//モデル名
	std::string name;
};
