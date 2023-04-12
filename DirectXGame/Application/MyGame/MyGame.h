#pragma once
#include "Framework.h"
#include "GamePlayScene.h"
#include "TitleScene.h"

class MyGame :public Framework
{
public:

	//‰Šú‰»
	void Initialize() override;

	//XV
	void Update() override;

	//•`‰æ
	void Draw() override;

	//I—¹
	void Finalize() override;
};

